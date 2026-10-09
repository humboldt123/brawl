#pragma once

#include <havok/hkBase.h>

// Ball-and-socket chain schema. The chain count sits at 0x04; per-chain blocks are 0x90 bytes.
struct hkJacobianBallSocketChainSchema {
    u32 m_tag;        // 0x00
    u32 m_numChains;  // 0x04 HYPOTHESIS: name

    u8* getAccumulatorOffsetsBase() const;
    u8* getMatrixBuffer(u8* base) const;
    u8* getTempBuffer(u8* base) const;
    u8* getEnd(u8* base) const;
};
