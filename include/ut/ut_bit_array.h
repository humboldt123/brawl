#pragma once

#include <types.h>

// Bit sets that hand out free indices (sora/ut/ut_bit_array.cpp): a bit count followed by the bit words.
// getUnsetBit returns the index of the first clear bit, or utBitArray::NO_UNSET_BIT when every bit is set.

class utBitArray {
public:
    enum { NO_UNSET_BIT = 0x0FFFFFFF };

    void clear();
    bool hasUnsetBit();
    u32 getUnsetBit();

    u32 m_count;   // 0x0, number of bits
    u32 m_bits[1]; // 0x4, (m_count + 1) / 32 + 1 words follow
};

// Base of the fixed-size variant; the derived template supplies the word storage right after m_count.
class utBitArrayStaticBase {
public:
    void initilize(u32 count); // sic, from the map
    u32 getUnsetBit();

    u32 m_count;   // 0x0
    u32 m_bits[1]; // 0x4
};

template <int N>
class utBitArrayStatic : public utBitArrayStaticBase {
public:
    u32 m_moreBits[(N + 1) / 32]; // 0x8, continues m_bits so that (N + 1) / 32 + 1 words are available
};
