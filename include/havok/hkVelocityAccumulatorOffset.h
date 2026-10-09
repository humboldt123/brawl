#pragma once

#include <havok/hkBase.h>

// Byte offset of one velocity accumulator from the accumulator base.
struct hkVelocityAccumulatorOffset {
    s32 m_offset; // 0x00 HYPOTHESIS: name

    u8* getAccumulator(u8* base) const;
};
