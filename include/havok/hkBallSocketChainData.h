#pragma once

#include <havok/hkConstraintData.h>

// Ball-and-socket chain constraint data. Field layout is partly recovered from getConstraintInfo/getRuntimeInfo:
// the chain atom count is an int at 0x1C. Other fields are not yet identified.
struct hkBallSocketChainData : hkConstraintData {
    u8 m_unk0C[0x10];   // 0x0C
    s32 m_chainLength;  // 0x1C
    u8 m_unk20[0x10];   // 0x20 (not yet identified)

    hkBallSocketChainData();
    virtual ~hkBallSocketChainData();

    u32 getType() const;
    void getConstraintInfo(hkConstraintInfo* info) const;
    void getRuntimeInfo(const hkBool* hasChain, hkConstraintRuntimeInfo* out) const;
    virtual void buildJacobian();
};
