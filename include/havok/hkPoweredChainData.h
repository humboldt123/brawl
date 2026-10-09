#pragma once

#include <havok/hkBase.h>
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintData.h>

// Powered chain constraint data. The chain length (int) at 0x1C drives the info and runtime sizes
// (hkPoweredChainData.cpp). The bridge atom at 0x0C is the same bridge layout as hkGenericConstraintData.
// HYPOTHESIS: the 0x18..0x1C words hold the chain header; other fields are not yet identified.
struct hkPoweredChainData : hkConstraintData {
    hkBridgeConstraintAtom m_bridgeAtom; // 0x0C
    u8 m_unk18[0x04];                    // 0x18
    s32 m_chainLength;                   // 0x1C

    u32 getType() const;
    void getConstraintInfo(hkConstraintInfo* info) const;
    void getRuntimeInfo(void* unusedA, hkConstraintRuntimeInfo* out) const;
    void* getConstraintFlags(void* base) const;
};
