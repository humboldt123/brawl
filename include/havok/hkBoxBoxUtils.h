#pragma once

#include <havok/hkVector4.h>

// Vector helpers of the box-box collision detector (hkBoxBoxUtils in the Havok TU map). Static functions, no state.
struct hkBoxBoxUtils {
    // Writes 1 to out when any of the x, y, z lanes of a is greater than b, otherwise 0.
    // The scalars are references: checkIntersection passes a member and sdata2 literals by address.
    static void cmpAllGT3(const hkVector4& a, const hkReal& b, int& out);
    // Per lane: dst[i] = value when cond[i] > threshold, otherwise dst[i] is kept (w is not touched).
    static void selectIfGT3(hkVector4& dst, const hkReal& value, const hkVector4& cond, const hkReal& threshold);
    // Per lane x, y, z: v = 1 / sqrt(v + epsilon) (a non-positive sum gives +inf); w = 0. Weak copy in
    // hkBoxBoxCollisionDetection.o.
    static void rsqrtAll3(hkVector4& v) {
        v.x = hkVector4_sqrtInverse(1.1920929e-7f + v.x);
        v.y = hkVector4_sqrtInverse(1.1920929e-7f + v.y);
        v.z = hkVector4_sqrtInverse(1.1920929e-7f + v.z);
        v.w = 0.0f;
    }
    // i mod 3 for 0 <= i < 9, from a table (weak copy in hkBoxBoxCollisionDetection.o).
    static int mod3(int i) { return s_mod3Table[i]; }

    static const int s_mod3Table[];
};
