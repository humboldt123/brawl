#include <types.h>
#include <ut/ut_bit_array.h>

// Index of the first clear bit of word, 0..31. When word is all ones the callers have already excluded it; the
// original then yields 33 (HYPOTHESIS: a sentinel one past the word), reproduced here.
static inline u32 utBitArrayFirstUnsetBit(u32 word) {
    int bit;
    for (bit = 0; bit < 32; bit++) {
        if ((word & 1) == 0) {
            return bit;
        }
        word >>= 1;
    }
    return 33;
}

// Mask of the low count bits.
static inline u32 utBitArrayLowMask(u32 count) {
    u32 mask = 0;
    for (int i = 0; i < count; i++) {
        mask |= 1 << i;
    }
    return mask;
}

void utBitArray::clear() {
    for (u32 i = 0; i < ((m_count + 1) >> 5) + 1; i++) {
        m_bits[i] = 0;
    }
}

bool utBitArray::hasUnsetBit() {
    u32 count = m_count;
    u32 numWords = count >> 5;
    u32 i = 0;
    for (int w = 0; w < numWords; w++, i++) {
        if (m_bits[i] != 0xFFFFFFFF) {
            return true;
        }
    }

    if (i * 32 < count) {
        u32 remain = count - i * 32;
        u32 mask = utBitArrayLowMask(remain);
        if ((m_bits[i] & mask) != mask) {
            return true;
        }
    }
    return false;
}

u32 utBitArray::getUnsetBit() {
    u32 count = m_count;
    u32 numWords = count >> 5;
    u32 base = 0;
    u32 i = 0;
    for (int w = 0; w < numWords; w++, base += 32, i++) {
        if (m_bits[i] != 0xFFFFFFFF) {
            return base + utBitArrayFirstUnsetBit(m_bits[i]);
        }
    }

    if (i * 32 < count) {
        u32 remain = count - i * 32;
        u32 mask = utBitArrayLowMask(remain);
        u32 word = m_bits[i] | ~mask;
        if (word != 0xFFFFFFFF) {
            return base + utBitArrayFirstUnsetBit(word);
        }
    }
    return NO_UNSET_BIT;
}

void utBitArrayStaticBase::initilize(u32 count) {
    m_count = count;
    for (int i = 0; i < (count + 1) >> 5; i++) {
        m_bits[i] = 0;
    }
}

u32 utBitArrayStaticBase::getUnsetBit() {
    u32 count = m_count;
    u32 numWords = count >> 5;
    u32 base = 0;
    u32 i = 0;
    for (int w = 0; w < numWords; w++, base += 32, i++) {
        if (m_bits[i] != 0xFFFFFFFF) {
            return base + utBitArrayFirstUnsetBit(m_bits[i]);
        }
    }

    if (i * 32 < count) {
        u32 remain = count - i * 32;
        u32 mask = utBitArrayLowMask(remain);
        u32 word = m_bits[i] | ~mask;
        if (word != 0xFFFFFFFF) {
            return base + utBitArrayFirstUnsetBit(word);
        }
    }
    return utBitArray::NO_UNSET_BIT;
}
