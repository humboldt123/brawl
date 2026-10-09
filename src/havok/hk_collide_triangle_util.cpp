// Havok translation unit hk_collide_triangle_util.o (main.dol 0x80325184-0x80325BB0).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x80325184   432  setupClosestPointTriangleCache   [map: hkCollideTriangleUtil__setupClosestPointTriangleCache]
//   0x80325334   744  setupPointTriangleDistanceCache   [map: hkCollideTriangleUtil__setupPointTriangleDistanceCache]
//   0x8032561C  1428  closestPointTriangle   [map: hkCollideTriangleUtil__closestPointTriangle]

#include <havok/hkCollideTriangleUtil.h>
#include <math.h>

// +infinity (same helper as hkQuaternion.cpp).
static hkReal hkInfinity() {
    u32 bits = 0x7f800000;
    return *(hkReal*)&bits;
}

// 1/sqrt(x) with the hardware estimate and one Newton step; +infinity for non-positive x (same helper as hkQuaternion.cpp).
static hkReal hkRsqrtApprox(hkReal x) {
    if (0.0f < x) {
        hkReal r = (hkReal)__frsqrte(x);
        return (0.5f * r) * -(r * (x * r) - 3.0f);
    }
    return hkInfinity();
}

#pragma fp_contract on

// Gram terms of the two edges a = v0 - v1 and c = v2 - v1, normalised by the Gram determinant.
void hkCollideTriangleUtil::setupClosestPointTriangleCache(const hkVector4* tri, hkVector4* cache) {
    hkVector4 a;
    a.setSub4(tri[0], tri[1]);
    hkVector4 c;
    c.setSub4(tri[2], tri[1]);
    hkReal aa = a.y * a.y + a.x * a.x + a.z * a.z;
    hkReal cc = c.y * c.y + c.x * c.x + c.z * c.z;
    hkReal ac = c.y * a.y + c.x * a.x + c.z * a.z;
    hkReal inv = 1.0f / (aa * cc - ac * ac);
    cache->x = aa * inv;
    cache->y = cc * inv;
    cache->z = ac * inv;
    hkVector4 n;
    n.setCross(a, c);
    hkReal nn = n.y * n.y + n.x * n.x + n.z * n.z;
    hkReal s = 1.0f / hkRsqrtApprox(nn);
    cache->w = 1.0f / s;
}
