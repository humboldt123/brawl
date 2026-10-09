#pragma once

#include <havok/hkVector4.h>

// Vector helpers of the box-box collision detector (hkBoxBoxUtils in the Havok TU map). Static functions, no state.
struct hkBoxBoxUtils {
    // Writes 1 to out when any lane of a is greater than the first lane of b (x of b), otherwise 0.
    static void cmpAllGT3(const hkVector4& a, const hkVector4& b, int& out);
    // Per lane: dst[i] = src.x when cond[i] > threshold.x, otherwise dst[i] is kept (w is not touched).
    // HYPOTHESIS: src is used as a broadcast of its x lane (the asm loads only src.x).
    static void selectIfGT3(hkVector4& dst, const hkVector4& src, const hkVector4& cond, const hkVector4& threshold);
};
