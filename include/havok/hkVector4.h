#pragma once

#include <havok/hkBase.h>
#include <math.h>

// Havok math scalar. Shared by all hk math headers.
typedef float hkReal;
// Scalar result of the length reductions (lengthSquared3, length3, ...). HYPOTHESIS: the alignment attribute sits
// on the typedef. BYTES: CodeWarrior turns copy propagation off in any function holding a value of a type
// aligned this way, including the return value of an inline function whose return expression computes one
// (a return that just forwards another call does not count), which is what gives most vector-math functions their timer form:
// the hkBool of HK_TIMER_BEGIN/END becomes a bool before the branch (extsb; neg; or; srwi.). Functions without
// such a value keep the folded extsb. test. The attribute does not change the layout of a float.
typedef hkReal hkSimdReal __attribute__((aligned(4)));

struct hkRotation;
struct hkQuaternion;
struct hkTransform;

// 16-byte aligned 4-float vector (x, y, z, w). Layout from the hk*Class reflection units.
// Member names are the usual Havok names for x/y/z/w (HYPOTHESIS: the original names are not in the image).
struct hkVector4 {
    hkReal x; // 0x00
    hkReal y; // 0x04
    hkReal z; // 0x08
    hkReal w; // 0x0C

    hkVector4() {}
    hkVector4(hkReal xx, hkReal yy, hkReal zz, hkReal ww) { set(xx, yy, zz, ww); }

    // User-declared copy: the image has an out-of-line weak hkVector4::__as (4 lfs + 4 stfs) and copies
    // hkVector4 members as floats, never as words.
    void operator=(const hkVector4& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
    }

    void set(hkReal xx, hkReal yy, hkReal zz, hkReal ww) {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }

    // Inline helpers in the Havok style (HYPOTHESIS: semantics from the Havok names, not asm-verified yet).
    // Havok's FPU form: one chained assignment, which stores w first.
    void setZero4() { x = y = z = w = 0.0f; }
    // Lane order x..w through the double fabs (fabs + frsp), like its weak copy in hkSimpleContactConstraintInfo.
    void setAbs4(const hkVector4& a) {
        x = (hkReal)fabs(a.x);
        y = (hkReal)fabs(a.y);
        z = (hkReal)fabs(a.z);
        w = (hkReal)fabs(a.w);
    }
    void setNeg3(const hkVector4& a) { set(-a.x, -a.y, -a.z, a.w); }
    // Lane order x..w, like its out-of-line weak copy in hkSimulation.
    void setNeg4(const hkVector4& a) {
        x = -a.x;
        y = -a.y;
        z = -a.z;
        w = -a.w;
    }
    void setAdd4(const hkVector4& a, const hkVector4& b) { set(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
    void setSub4(const hkVector4& a, const hkVector4& b) { set(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
    // HYPOTHESIS: vector-first overload (no image evidence for this argument order).
    void setMul4(const hkVector4& a, hkReal s) { set(a.x * s, a.y * s, a.z * s, a.w * s); }
    // this = s * a. Scalar-first products, lane order x..w: hkSolver's out-of-line weak copy (vector in r4,
    // scalar in f1).
    void setMul4(hkReal s, const hkVector4& a) {
        x = s * a.x;
        y = s * a.y;
        z = s * a.z;
        w = s * a.w;
    }
    void setCross(const hkVector4& a, const hkVector4& b) {
        set(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, 0.0f);
    }
    // In place, lane order x..w (the out-of-line weak copy in hkSimpleContactConstraintInfo loads x first).
    void mul4(hkReal s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
    }
    void add4(const hkVector4& a) {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
    }
    void sub4(const hkVector4& a) {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
    }
    // Added from asm-verified callers (hkCollideCapsuleUtil, hkRigidMotionUtil, hkBoxBoxCollisionDetection).
    void setAll(hkReal v) { set(v, v, v, v); }
    // Bit mask of the lanes that are negative: x -> 8, y -> 4, z -> 2, w -> 1.
    int compareLessThanZero4() const {
        return (x < 0.0f ? 8 : 0) | (y < 0.0f ? 4 : 0) | (z < 0.0f ? 2 : 0) | (w < 0.0f ? 1 : 0);
    }
    // Bit mask of the lanes where this < a: x -> 8, y -> 4, z -> 2, w -> 1 (weak copy in hkBoxBoxCollisionDetection).
    int compareLessThan4(const hkVector4& a) const {
        return ((x < a.x) << 3) | ((y < a.y) << 2) | ((z < a.z) << 1) | (w < a.w);
    }
    // True when |this - v| <= epsilon in x, y and z (w is computed but masked out). HYPOTHESIS: the name; the shape
    // is inlined once per column in hkMatrix3::isApproximatelyEqual.
    hkBool equals3(const hkVector4& v, hkReal epsilon) const {
        hkVector4 eps;
        eps.setAll(epsilon);
        hkVector4 diff;
        diff.setSub4(*this, v);
        diff.setAbs4(diff);
        return (eps.compareLessThan4(diff) & 0xE) == 0;
    }
    // Lane-wise reciprocal (out of line: defined in src/havok/hkRigidMotionUtil.cpp).
    void setReciprocal4(const hkVector4& a);
    // Lane-wise product in place: this *= a (out of line: defined in src/havok/hkRigidMotionUtil.cpp).
    void mul4(const hkVector4& a);
    // Lane-wise product: this = a * b.
    void setMul4(const hkVector4& a, const hkVector4& b) {
        set(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
    }
    // Lane-wise minimum, a < b ? a : b per lane (sphere/box agent).
    void setMin4(const hkVector4& a, const hkVector4& b) {
        x = a.x < b.x ? a.x : b.x;
        y = a.y < b.y ? a.y : b.y;
        z = a.z < b.z ? a.z : b.z;
        w = a.w < b.w ? a.w : b.w;
    }
    // Flips the sign of each lane of this where the lane of s is negative (integer xor of the sign bits).
    void setFlipSign4(const hkVector4& a, const hkVector4& s) {
        ((u32*)this)[0] = ((const u32*)&a)[0] ^ (((const u32*)&s)[0] & 0x80000000);
        ((u32*)this)[1] = ((const u32*)&a)[1] ^ (((const u32*)&s)[1] & 0x80000000);
        ((u32*)this)[2] = ((const u32*)&a)[2] ^ (((const u32*)&s)[2] & 0x80000000);
        ((u32*)this)[3] = ((const u32*)&a)[3] ^ (((const u32*)&s)[3] & 0x80000000);
    }
    // this = a + s * b, lane order x..w (its weak out-of-line copy is in hkGsk).
    void setAddMul4(const hkVector4& a, const hkVector4& b, hkReal s) {
        x = a.x + s * b.x;
        y = a.y + s * b.y;
        z = a.z + s * b.z;
        w = a.w + s * b.w;
    }
    // this = (1 - t) * a + t * b (hkMultiRayConvexAgent's hit point).
    void setInterpolate4(const hkVector4& a, const hkVector4& b, hkReal t) {
        hkReal s = 1.0f - t;
        x = s * a.x + t * b.x;
        y = s * a.y + t * b.y;
        z = s * a.z + t * b.z;
        w = s * a.w + t * b.w;
    }
    // Component access by index (x, y, z, w).
    hkReal& operator[](int i) { return (&x)[i]; }
    hkReal getSimdAt(int i) const { return (&x)[i]; }
    hkReal dot3(const hkVector4& a) const { return x * a.x + y * a.y + z * a.z; }
    hkSimdReal lengthSquared3() const { return x * x + y * y + z * z; }
    hkSimdReal lengthSquared4() const { return dot3(*this) + w * w; }
    hkSimdReal length3() const { return sqrtf(lengthSquared3()); }
    void normalize3() {
        hkReal inv = 1.0f / sqrtf(dot3(*this));
        set(x * inv, y * inv, z * inv, w);
    }

    // Transforms by a rigid transform / rotation. Asm-verified in src/havok/hkVector4.cpp (once matched).
    // setTransformedPos: this = R * v + t.   setRotatedDir: this = R * v.
    void setTransformedPos(const hkTransform& t, const hkVector4& v);
    void setTransformedInversePos(const hkTransform& t, const hkVector4& v);
    // Inline form of setTransformedPos (same body); defined in hkTransform.h. Inlined by the sphere agents.
    inline void _setTransformedPos(const hkTransform& t, const hkVector4& v);
    // Inline form of setTransformedInversePos (same body); defined in hkTransform.h. Inlined by the sphere agents.
    inline void _setTransformedInversePos(const hkTransform& t, const hkVector4& v);
    void setRotatedDir(const hkRotation& r, const hkVector4& v);
    void setRotatedInverseDir(const hkRotation& r, const hkVector4& v);
    // Column-major product: this = m.col0 * v.x + m.col1 * v.y + m.col2 * v.z, w = 0. The inline behind
    // setRotatedDir (byte-identical); defined in hkRotation.h. hkSolver.cpp defines HK_VECTOR4_OUTLINE_SETMUL3
    // to keep its out-of-line weak copy until the solver functions that fail to inline it are decompiled.
    // this = R * v, w = 0. Named by its weak out-of-line copy in hkGenericConstraintData; defined in hkRotation.h.
    inline void _setRotatedDir(const hkRotation& r, const hkVector4& v);
    // this = R^T * v (each lane is v dotted with one column), w = 0. Named by its weak out-of-line copy in
    // hkBoxBoxCollisionDetection; defined in hkRotation.h.
    inline void _setRotatedInverseDir(const hkRotation& r, const hkVector4& v);
#ifdef HK_VECTOR4_OUTLINE_SETMUL3
    void _setMul3(const hkRotation& m, const hkVector4& v);
#else
    inline void _setMul3(const hkRotation& m, const hkVector4& v);
#endif
} __attribute__((aligned(16)));
