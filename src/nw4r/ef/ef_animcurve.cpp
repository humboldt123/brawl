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

} // namespace ef
} // namespace nw4r
