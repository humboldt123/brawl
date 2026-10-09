// nw4r ef_animcurve.cpp (main.dol 0x80166EFC-0x8016B680). Only createChild is drafted so far.
#include <nw4r/ef.h>

namespace nw4r {
namespace ef {

// Child command record (0xA bytes): an inherit setting followed by the index of
// the child resource in the name table.
struct AnimCurveChild {
    EmitterInheritSetting mInherit; // at 0x0
    u16 mNameIndex;                 // at 0x8
};

// Child curves of random type select one record from the random table.
// Random-table records follow the 16-bit count and start at 0x4.
static AnimCurveChild* GetRandomChild(AnimCurveRandomTable* pTable, u32 index) {
    return reinterpret_cast<AnimCurveChild*>(pTable->datas + index * sizeof(AnimCurveChild));
}

void createChild(u8* pCmdList, u16 seed, AnimCurveHeader* pHeader,
                 AnimCurveNameTable* pNameTable,
                 AnimCurveRandomTable* pRandomTable, Particle* pParticle,
                 u32 life) {
    AnimCurveChild* pChild;
    u32 hash;
    u8 curveFlag = reinterpret_cast<AnimCurveHeader*>(pCmdList)->curveFlag;

    if (curveFlag == 0) {
        pChild = reinterpret_cast<AnimCurveChild*>(pCmdList + 0xC);
    } else {
        union {
            u32 w;
            u8 b[4];
        } h;

        hash = seed * 0x3F81F635 + pHeader->randomSeed * 0x30A74193 + 0x4BF53 +
               life * 0x7B929 + *reinterpret_cast<u16*>(pCmdList + 0xC) * 0x371097E7;
        h.w = hash;
        h.b[2] ^= h.b[3];
        h.b[1] ^= h.b[2];
        h.b[0] ^= h.b[1];
        hash = h.w;

        if (curveFlag & 2) {
            u16 count = pRandomTable->count;
            if (count == 0) {
                return;
            }
            pChild = GetRandomChild(pRandomTable, (hash >> 16) % count);
        } else {
            // HYPOTHESIS: the target reads the seed register as the record pointer on this path.
            pChild = reinterpret_cast<AnimCurveChild*>(static_cast<u32>(seed));
        }
    }

    u32 resource = pNameTable->datas[pChild->mNameIndex].work;
    if (resource != 0) {
        if (pChild->mInherit.type == 0) {
            pParticle->mParticleManager->mManagerEM->mManagerEF->mManagerES
                ->mCreationQueue.AddParticleCreation(
                    &pChild->mInherit, pParticle,
                    reinterpret_cast<EmitterResource*>(resource),
                    pParticle->mCalcRemain);
        } else {
            pParticle->mParticleManager->mManagerEM->mManagerEF->mManagerES
                ->mCreationQueue.AddEmitterCreation(
                    &pChild->mInherit, pParticle,
                    reinterpret_cast<EmitterResource*>(resource),
                    pParticle->mCalcRemain);
        }
    }
}

// Texture curve executor. Command block layout: header 0x0..0x1F, a key table
// at 0x20 (u16 count, 16-byte records from 0x24), then the record and value
// tables at the keyTable / rangeTable offsets. Results go to the particle's
// texture slots (0x88 / 0x8C / 0x90) and the packed fields at 0x94 / 0x96.
void AnimCurveExecuteTexture(u8* pCmdList, Particle* pParticle, u32 tick,
                             u16 seed, u32 life) {
    u8* pCmd = pCmdList;
    AnimCurveHeader* pHeader = reinterpret_cast<AnimCurveHeader*>(pCmd);
    u8 processFlag = pHeader->processFlag;
    u8* pBase = pCmd + 0x20;
    u8* pKeyRecords = pBase + pHeader->keyTable;
    u8* pRangeTable = pKeyRecords + pHeader->rangeTable;
    u8* pValueTable = pRangeTable + pHeader->randomTable;
    u32 frame;
    u32 index = 0;
    u32 value;
    u32 hash;
    u32 slot;
    u8* pSlotOut;
    int slotShift;
    u32 kind;
    u8* pPart = reinterpret_cast<u8*>(pParticle);

    if (!(processFlag & AnimCurveHeader::PROC_FLAG_INFLOOP) && pHeader->loopCount < 2) {
        if (!(processFlag & AnimCurveHeader::PROC_FLAG_FITTING)) {
            frame = pHeader->frameLength;
            if (tick < frame) {
                frame = tick & 0xFFFF;
            }
        } else {
            frame = (pHeader->frameLength * tick) / life & 0xFFFF;
        }
        index = 0;
    } else if (!(processFlag & AnimCurveHeader::PROC_FLAG_FITTING)) {
        index = 0;
        frame = pHeader->frameLength;
        if (frame < 2 || !(processFlag & AnimCurveHeader::PROC_FLAG_TURN)) {
            index = tick / frame;
            if (!(processFlag & AnimCurveHeader::PROC_FLAG_INFLOOP) &&
                pHeader->loopCount <= index) {
                index = (pHeader->loopCount - 1) & 0xFF;
            } else {
                frame = (tick - index * frame) & 0xFFFF;
            }
        } else {
            frame = frame - 1;
            index = tick / frame;
            if (!(processFlag & AnimCurveHeader::PROC_FLAG_INFLOOP)) {
                u8 loop = pHeader->loopCount;
                if (loop <= index) {
                    index = (loop - 1) & 0xFF;
                    frame = (frame & 0xFF) & ~-(__cntlzw(loop & 1) >> 5 & 1);
                    goto lookup;
                }
            }
            if ((index & 1) == 0) {
                frame = (tick - index * frame) & 0xFFFF;
            } else {
                frame = (frame * (index + 1) - tick) & 0xFFFF;
            }
        }
    } else if (tick < life) {
        frame = pHeader->frameLength;
        if (frame < 2 || !(processFlag & AnimCurveHeader::PROC_FLAG_TURN)) {
            f32 scale = (f32)pHeader->loopCount * ((f32)frame / (f32)life);
            index = (u32)((f32)tick * scale / (f32)frame);
            frame = (s32)((f32)tick * scale - (f32)(index * frame));
        } else {
            u8 loop = pHeader->loopCount;
            frame = frame - 1;
            index = (u32)((f32)tick * (((f32)(s32)frame * (f32)loop + 1.0f) /
                                        (f32)life));
            index = index / frame;
            if (index < loop) {
                if ((index & 1) == 0) {
                    frame = (tick - index * frame) & 0xFFFF;
                } else {
                    frame = (frame * (index + 1) - tick) & 0xFFFF;
                }
            } else {
                index = (loop - 1) & 0xFF;
                frame = (frame & 0xFF) & ~-(__cntlzw(loop & 1) >> 5 & 1);
            }
        }
    } else {
        if (!(processFlag & AnimCurveHeader::PROC_FLAG_TURN) ||
            (pHeader->loopCount & 1) != 0) {
            frame = pHeader->frameLength;
        } else {
            frame = 0;
        }
        index = (pHeader->loopCount - 1) & 0xFF;
    }

lookup:
    frame &= 0xFFFF;
    kind = pCmd[1];
    if (kind == 0xF) {
        pSlotOut = pPart + 0x8C; // HYPOTHESIS: texture slot 1 output word
        slotShift = 1;
    } else if (kind > 0xF) {
        if (kind > 0x10) {
            return;
        }
        pSlotOut = pPart + 0x90;
        slotShift = 2;
    } else {
        if (kind < 0xE) {
            return;
        }
        pSlotOut = pPart + 0x88;
        slotShift = 0;
    }

    {
        int last = *(u16*)pBase - 1;
        int i;
        if (frame < *(u16*)(pBase + 4)) {
            i = 0;
        } else {
            i = last;
            if (frame < *(u16*)(pBase + 4 + last * 16)) {
                int lo = 0;
                int hi = last;
                int mid = last / 2;
                while (lo < mid) {
                    if (*(u16*)(pBase + 4 + mid * 16) <= frame) {
                        lo = mid;
                    } else {
                        hi = mid;
                    }
                    mid = (lo + hi) / 2;
                }
                i = lo;
            }
        }

        u8* pRec = pBase + 4 + i * 16;
        if (pRec[2] == 0) {
            value = *(u16*)(pRec + 0xE);
            index = pRec[0xD];
            slot = pRec[0xC];
        } else {
            hash = (seed * 0x3F81F635 + reinterpret_cast<AnimCurveHeader*>(pCmd)->randomSeed * 0x30A74193) +
                   (index * 0x7B929 + (*(u16*)(pRec + 0xC) * 0x371097E7 + 0x4BF53));
            union {
                u32 w;
                u8 b[4];
            } h;
            h.w = hash;
            h.b[2] ^= h.b[3];
            h.b[1] ^= h.b[2];
            h.b[0] ^= h.b[1];

            if ((pRec[2] & 2) == 0) {
                u8* pRecord = pKeyRecords + *(u16*)(pRec + 0xC) * 8;
                u8 mode = pRecord[8];
                u8 pairB = pRecord[5];
                value = *(u16*)(pRecord + 6);
                slot = pRecord[4];
                index = pairB;
                if (mode == 2) {
                    index = (pairB & 1) | ((h.w >> 16) & 2);
                } else if (mode < 2) {
                    if (mode != 0) {
                        index = (pairB & 2) | ((h.w >> 16) & 1);
                    }
                } else if (mode < 4) {
                    index = (h.w >> 16) & 3;
                }
            } else {
                index = (((((h.w >> 24) ^ (h.w >> 16)) & 0xFF) << 8) | ((h.w >> 16) & 0xFF));
                u16 count2 = *(u16*)pRangeTable;
                u32 k = index % count2;
                value = *(u16*)(pRangeTable + 6 + k * 4);
                slot = pRangeTable[4 + k * 4];
                index = pRangeTable[5 + k * 4];
            }
        }

        *(u32*)pSlotOut = *(u32*)(pValueTable + (value & 0xFFFF) * 4 + 4);
        u16 packed = *(u16*)(pPart + 0x94);
        u8 packedB = *(u8*)(pPart + 0x96);
        *(u16*)(pPart + 0x94) = (packed & ~(0xF << (slotShift * 4))) |
                                    ((slot & 0xF) << (slotShift * 4));
        *(u8*)(pPart + 0x96) = (packedB & ~(3 << (slotShift * 2))) |
                                   ((index & 3) << (slotShift * 2));
    }
}

} // namespace ef
} // namespace nw4r
