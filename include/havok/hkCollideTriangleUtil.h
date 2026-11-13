#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Triangle and segment closest-point helpers (Havok hkCollideTriangleUtil). Static functions, no state.
// Parameter and result layouts are HYPOTHESIS until the callers are recovered.
struct hkCollideTriangleUtil {
    // Per-triangle cache read by hkCollideCapsuleUtil_calcTrianglePlaneDirections (16 bytes). HYPOTHESIS: the
    // members (the Havok names of the edge-length reciprocals and the normal length).
    struct PointTriangleDistanceCache {
        hkReal m_invEdgeLen[3];
        hkReal m_normalLen;
    };

    // Result record of closestLineSegLineSeg. hkCollideCapsuleUtilCapsVsTri reads the closest point on the first
    // segment at 0x00, the difference vector at 0x10 and the squared distance at 0x20 (0x30 bytes on its stack).
    // HYPOTHESIS: the names and the two fractions after the distance.
    struct ClosestLineSegLineSegResult {
        ClosestLineSegLineSegResult() {}
        hkVector4 m_closestPointA;         // 0x00
        hkVector4 m_closestAminusClosestB; // 0x10
        hkReal m_distanceSquared;          // 0x20
        hkReal m_fractionA;                // 0x24
        hkReal m_fractionB;                // 0x28
    };

    // Closest points of the segments a..a+dA and b..b+dB. Returns a mask with bit 0 / bit 1 set when the closest
    // point of the first segment is its start / end (HYPOTHESIS from hkCollideCapsuleUtilCapsVsTri's use).
    static int closestLineSegLineSeg(const hkVector4& a, const hkVector4& dA, const hkVector4& b, const hkVector4& dB,
                                     ClosestLineSegLineSegResult& result);

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
