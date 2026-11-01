#pragma once

#include <mt/mt_coll2d.h>
#include <mt/mt_vector.h>

// Paired-single minimum / maximum helpers shared by the 2D collision units (mt_coll2d, gr_collision*) and the
// Pokemon Stadium screens.
// BYTES: the originals take minima and maxima with ps_sub + ps_sel; MWCC GC/3.0a5.2 has no paired-single intrinsics
// and never emits ps_* from C (a ternary or fsel gives fcmpo / fsel), so the helpers are asm. HYPOTHESIS: names.

static inline float psMin(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, b, a
    }
    // clang-format on
    return result;
}

static inline float psMax(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, a, b
    }
    // clang-format on
    return result;
}

// The componentwise minimum (lo) and maximum (hi) of the two end points of a segment, from one subtraction.
static inline void segmentBounds(register const clSegment2D* segment, register Vec2f* lo, register Vec2f* hi) {
    register float p0, p1, diff, minPair, maxPair;
    // clang-format off
    asm {
        psq_l p0, 0(segment), 0, 0
        psq_l p1, 8(segment), 0, 0
        ps_sub diff, p0, p1
        ps_sel minPair, diff, p1, p0
        ps_sel maxPair, diff, p0, p1
        psq_st minPair, 0(lo), 0, 0
        psq_st maxPair, 0(hi), 0, 0
    }
    // clang-format on
}
