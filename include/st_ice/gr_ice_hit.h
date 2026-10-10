#pragma once

#include <so/collision/so_collision_hit_part.h>
#include <so/so_array.h>
#include <types.h>

// MATCH-ONLY: the members of soSet are private
struct grIceSetView {
    void* m_elements;
    u32 m_size;
};

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grIceHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: the copy of a hit sphere to its simple form (word by word, as the original does it)
static inline void grIceCopyHit(soCollisionHitData::Simple* dst, soCollisionHitData* src) {
    u32* srcWords = reinterpret_cast<u32*>(src);
    u32* dstWords = reinterpret_cast<u32*>(dst);
    for (int w = 0; w < 6; w += 3) {
        u32 w1 = srcWords[w + 1];
        u32 w0 = srcWords[w];
        dstWords[w] = w0;
        dstWords[w + 1] = w1;
        dstWords[w + 2] = srcWords[w + 2];
    }
    dst->m_size = src->m_size;
    reinterpret_cast<u8*>(dst)[0x1C] = reinterpret_cast<u8*>(src)[0x1C];
}
