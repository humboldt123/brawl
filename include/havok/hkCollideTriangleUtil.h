#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Triangle and segment closest-point helpers (Havok hkCollideTriangleUtil). Static functions, no state.
// Parameter and result layouts are HYPOTHESIS until the callers are recovered.
struct hkCollideTriangleUtil {
    // Result record of closestLineSegLineSeg (layout not recovered yet).
    struct ClosestLineSegLineSegResult {
        ClosestLineSegLineSegResult() {}
    };

    // Index helpers for the three triangle vertices: (i + 1) % 3 and (i + 2) % 3, read from a lookup table.
    static int getNextModulo3(int i);
    static int getPrevModulo3(int i);

    // Closest point of the segment a..b to p, written to out (w is copied from the point, see the asm).
    // Returns 8 when a is closest, 4 when b is closest and 0 for an interior point. HYPOTHESIS: out is the point.
    // Cache for the closest point of a triangle (tri: three vertices, 16 bytes apart; the edges are taken from
    // vertex 1). cache.x, y, z: |a|^2, |c|^2, a.c divided by the Gram determinant; cache.w: |a x c|.
    static void setupClosestPointTriangleCache(const hkVector4* tri, hkVector4* cache);
    static int closestPointLineSeg(const hkVector4& p, const hkVector4& a, const hkVector4& b, hkVector4& out);
};
