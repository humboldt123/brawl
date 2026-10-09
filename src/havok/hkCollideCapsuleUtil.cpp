// Havok translation unit hkCollideCapsuleUtil.o (main.dol 0x803227F4-0x80325184).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x803227F4  3000  hkCollideCapsuleUtilManifoldCapsVsCaps   [map: hkCollideCapsuleUtil__hkCollideCapsuleUtilManifoldCapsVsCaps]
//   0x803233AC     8  setDistance   [map: hkContactPoint__setDistance]
//   0x803233B4   976  hkCollideCapsuleUtilClostestPointCapsVsCaps   [map: hkCollideCapsuleUtil__hkCollideCapsuleUtilClostestPointCapsVsCaps]
//   0x80323784     4  __ct   [map: hkCollideTriangleUtil27ClosestLineSegLineSegResultFv____ct]
//   0x80323788  2496  hkCollideCapsuleUtilCapsVsTri   [map: hkCollideCapsuleUtil__hkCollideCapsuleUtilCapsVsTri]
//   0x80324148   836  hkCollideCapsuleUtil_calcTrianglePlaneDirections   [map: hkVector4__hkCollideCapsuleUtil_calcTrianglePlaneDirections]
//   0x8032448C  1000  mul4xyz1Points   [map: hkVector4Util__mul4xyz1Points]
//   0x80324874    20  setAll   [map: hkVector4__setAll]
//   0x80324888    84  compareLessThanZero4   [map: hkVector4__compareLessThanZero4]
//   0x803248DC    92  setInterpolate4   [map: hkVector4__setInterpolate4]
//   0x80324938    48  getRow   [map: hkMatrix3__getRow]
//   0x80324968     4  __ct   [map: hkContactPoint____ct]
//   0x8032496C    24  getNextModulo3   [map: hkCollideTriangleUtil__getNextModulo3]
//   0x80324984    20  getPrevModulo3   [map: hkCollideTriangleUtil__getPrevModulo3]
//   0x80324998    68  __as   [map: hkContactPoint____as]
//   0x803249DC  1020  calcBarycentricCoordinates   [map: hkCollideTriangleUtil__calcBarycentricCoordinates]
//   0x80324DD8   604  closestLineSegLineSeg   [map: hkCollideTriangleUtil__closestLineSegLineSeg]
//   0x80325034   336  closestPointLineSeg   [map: hkCollideTriangleUtil__closestPointLineSeg]


#include <havok/hkCollideTriangleUtil.h>
#include <havok/hkContactPoint.h>

#pragma fp_contract on

// Lookup table of the modulo-3 successor/predecessor (other unit, referenced by its symbol).
extern "C" const s8 lbl_805A47DC[];

// hkContactPoint::setDistance: writes the w component of the normal (the contact distance).
void hkContactPoint::setDistance(hkReal distance) {
    m_normal.w = distance;
}

// The table holds the predecessors at [0..2] and the successors at [2..4].
int hkCollideTriangleUtil::getNextModulo3(int i) {
    return lbl_805A47DC[i + 2];
}

int hkCollideTriangleUtil::getPrevModulo3(int i) {
    return lbl_805A47DC[i];
}

// Closest point on the segment a..b to p. The parameter is the projection of (p - a) on (b - a).
int hkCollideTriangleUtil::closestPointLineSeg(const hkVector4& p, const hkVector4& a, const hkVector4& b, hkVector4& out) {
    hkVector4 ab;
    ab.setSub4(b, a);
    hkVector4 ap;
    ap.setSub4(p, a);
    hkReal dot = ab.dot3(ap);
    if (dot <= 0.0f) {
        out = a;
        return 8;
    }
    hkReal lenSq = ab.lengthSquared3();
    if (dot >= lenSq) {
        out = b;
        return 4;
    }
    hkReal t = dot / lenSq;
    hkVector4 step;
    step.setMul4(ab, t);
    out.setAdd4(a, step);
    return 0;
}
