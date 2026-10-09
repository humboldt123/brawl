// Havok translation unit hkBallSocketChainData.o (main.dol 0x802DA49C-0x802DAA68).
// Functions in address order (method names from the Havok TU map):
//   0x802DA49C    52  finishLoadedObjecthkBallSocketChainData   [map: hkBallSocketChainData__finishLoadedObjecthkBallSocketChainData]
//   0x802DA4D0    20  cleanupLoadedObjecthkBallSocketChainData   [map: hkBallSocketChainData__cleanupLoadedObjecthkBallSocketChainData]
//   0x802DA4E4    72  getVtablehkBallSocketChainData   [map: hkBallSocketChainData__getVtablehkBallSocketChainData]
//   0x802DA52C    88  __ct   [map: hkBallSocketChainData____ct]
//   0x802DA584   148  __dt   [map: hkBallSocketChainData____dt]
//   0x802DA618     8  getType   [map: hkBallSocketChainData__getType]
//   0x802DA620   100  getConstraintInfo   [map: hkBallSocketChainData__getConstraintInfo]
//   0x802DA684    56  getRuntimeInfo   [map: hkBallSocketChainData__getRuntimeInfo]
//   0x802DA6BC   940  buildJacobian   [map: hkBallSocketChainData__buildJacobian]
#include <havok/hkBallSocketChainData.h>

void cleanupLoadedObjecthkBallSocketChainData(hkBallSocketChainData* self) {
    delete self;
}

u32 hkBallSocketChainData::getType() const {
    return 101; // CONSTRAINT_TYPE_BALL_SOCKET_CHAIN
}

// Chain info block: six words. HYPOTHESIS: unk10 points at the chain header block (this + 0xC, 0xC bytes long).
void hkBallSocketChainData::getConstraintInfo(hkConstraintInfo* info) const {
    s32 n = m_chainLength;
    info->unk00 = 0;
    info->unk04 = 0;
    info->unk08 = 0x18;
    info->unk0C = 0;
    info->unk10 = (u8*)this + 0xC;
    info->unk14 = 0xC;
    info->unk0C = 3 * n;
    info->unk08 = 4 * (n + 1) + 0x28;
    info->unk04 = 16 * (n + 1) + 0x120 * n;
}

void hkBallSocketChainData::getRuntimeInfo(const hkBool* hasChain, hkConstraintRuntimeInfo* out) const {
    if (hasChain->m_bool != 0) {
        s32 n = m_chainLength;
        out->unk04 = 3 * n;
        out->unk00 = 24 * n;
    } else {
        out->unk04 = 0;
        out->unk00 = 0;
    }
}

// Not yet decompiled in this unit:
//   0x802DA49C    52  finishLoadedObjecthkBallSocketChainData   [map: hkBallSocketChainData__finishLoadedObjecthkBallSocketChainData]
//   0x802DA4E4    72  getVtablehkBallSocketChainData   [map: hkBallSocketChainData__getVtablehkBallSocketChainData]
//   0x802DA52C    88  __ct   [map: hkBallSocketChainData____ct]
//   0x802DA584   148  __dt   [map: hkBallSocketChainData____dt]
//   0x802DA6BC   940  buildJacobian   [map: hkBallSocketChainData__buildJacobian]
