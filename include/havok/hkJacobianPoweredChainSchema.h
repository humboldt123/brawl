#pragma once

#include <havok/hkBase.h>

// Powered chain schema. The chain count sits at 0x08.
struct hkJacobianPoweredChainSchema {
    u32 m_tag;       // 0x00
    u32 unk04;       // 0x04
    u32 m_numChains; // 0x08 HYPOTHESIS: name

    u8* getAngularJacobians(u8* base) const;
    u8* getAccumulatorOffsetsBase() const;
    u8* getMatrixBuffer(u8* base) const;
    u8* getChildConstraintStatusBase() const;
    u8* getTempBuffer(u8* base) const;
    u8* getVelocityBuffer(u8* base) const;
    u8* getEnd(u8* base) const;
};
