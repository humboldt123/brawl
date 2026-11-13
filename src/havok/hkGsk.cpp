// Havok translation unit hkGsk.o (main.dol 0x803183A0-0x8031C8CC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x803183A0   716  findClosestTriangleBackup   [map: hkVector4__findClosestTriangleBackup]
//   0x8031866C  1352  checkTriangleBoundaries   [map: hkGsk__checkTriangleBoundaries]
//   0x80318BB4    28  isNegative   [map: hkMath__isNegative]
//   0x80318BD0     8  setFeatureChange   [map: hkGsk__setFeatureChange]
//   0x80318BD8  1372  findClosestTriangle   [map: hkVector4__findClosestTriangle]
//   0x80319134  1940  processEdgeTriangle   [map: hkGsk__processEdgeTriangle]
//   0x803198C8  1352  getClosestFeature   [map: hkGsk__getClosestFeature]
//   0x80319E10   880  transformPoints   [map: hkVector4Util__transformPoints]
//   0x8031A180  1612  reduceDimension   [map: hkGsk__reduceDimension]
//   0x8031A7CC   724  process_point_edge   [map: hkVector4__process_point_edge]
//   0x8031AAA0    36  __as   [map: hkCdVertex____as]
//   0x8031AAC4   128  hkGskCalcSupportDistSquared   [map: hkVector4__hkGskCalcSupportDistSquared]
//   0x8031AB44   312  exitAndExportCacheImpl   [map: hkGsk__exitAndExportCacheImpl]
//   0x8031AC7C   208  convertFeatureToClosestDistance   [map: hkGsk__convertFeatureToClosestDistance]
//   0x8031AD4C    68  setAddMul4   [map: hkVector4__setAddMul4]
//   0x8031AD90   868  getClosestPoint   [map: hkGsk__getClosestPoint]
//   0x8031B0F4  2312  handlePenetration   [map: hkGsk__handlePenetration]
//   0x8031B9FC   932  reduceDimensionExtended   [map: hkGsk__reduceDimensionExtended]
//   0x8031BDA0  2860  hkGskRecalcContactInternal   [map: hkGsk__hkGskRecalcContactInternal]

#include <havok/hkGsk.h>

void hkGsk::setFeatureChange(int value) {
    unk14 = value;
}

// The plane gives the normal and the distance (its w). The point: the single vertex of A, the single vertex of B
// moved along the plane by the distance, or the point kept at 0x130 (weights 1, 0 and 0.5).
void hkGsk::convertFeatureToClosestDistance(const hkVector4& plane, hkGskClosestDistance& out) const {
    out.m_distance = plane.w;
    out.m_normal = plane;
    if (m_dimA == 1) {
        out.m_position = vectorAt(0x20);
        out.m_weight = 1.0f;
    } else if (m_dimB == 1) {
        out.m_position.setAddMul4(vectorAt(0xA0), plane, plane.w);
        out.m_weight = 0.0f;
    } else {
        out.m_position = vectorAt(0x130);
        out.m_weight = 0.5f;
    }
}

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// hkCdVertex::operator=
// TODO: cleanup (shape from jj-link/brawl fn_8031AAA0)
extern "C" void fn_8031AAA0(float* d, const float* s) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_80318BB4.cpp.
// hkMath::isNegative-style sign mask (returns 8 for a negative value).
// TODO: cleanup (shape from jj-link/brawl fn_80318BB4)
extern "C" int fn_80318BB4(float f) {
    if (f < 0.0f) {
        return 8;
    }
    return 0;
}

// TODO: cleanup (shape from jj-link/brawl fn_80319E10)
// fn_80319E10: body from https://github.com/jj-link/brawl src/havok/fn_80319E10.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
namespace jj_fn_80319E10 {
#pragma fp_contract on

struct V { float x,y,z,w; } __attribute__((aligned(16)));
struct M {
    V a,b,c,d;
    M() {}
    M(const M& o) { *this = o; }
    M& operator=(const M& o) {
        a.x=o.a.x; a.y=o.a.y; a.z=o.a.z; a.w=o.a.w;
        b.x=o.b.x; b.y=o.b.y; b.z=o.b.z; b.w=o.b.w;
        c.x=o.c.x; c.y=o.c.y; c.z=o.c.z; c.w=o.c.w;
        d.x=o.d.x; d.y=o.d.y; d.z=o.d.z; d.w=o.d.w;
        return *this;
    }
};
extern "C" const f32 lbl_805A46B8;

extern "C" void fn_80319E10(const M* mat, const float* in, int count, float* out) {
    M n;
    M m = *mat;
    n = m;
    for (int i = 0; i < count; ++i) {
        int j = i * 4;
        float* dst = out + j;
        const float* src = in + j;
        float z = src[2], y = src[1], x = src[0];
        dst[0] = n.d.x + (x*n.a.x + y*n.b.x + z*n.c.x);
        dst[1] = n.d.y + (x*n.a.y + y*n.b.y + z*n.c.y);
        dst[2] = n.d.z + (x*n.a.z + y*n.b.z + z*n.c.z);
        dst[3] = lbl_805A46B8;
    }
}
} // namespace jj_fn_80319E10
