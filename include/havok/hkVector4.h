#pragma once

#include <havok/hkBase.h>
#include <math.h>

// Havok math scalar. Shared by all hk math headers.
typedef float hkReal;

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

    void set(hkReal xx, hkReal yy, hkReal zz, hkReal ww) {
        x = xx;
        y = yy;
        z = zz;
        w = ww;
    }

    // Inline helpers in the Havok style (HYPOTHESIS: semantics from the Havok names, not asm-verified yet).
    void setZero4() { set(0.0f, 0.0f, 0.0f, 0.0f); }
    void setAbs4(const hkVector4& a) { set(fabsf(a.x), fabsf(a.y), fabsf(a.z), fabsf(a.w)); }
    void setNeg3(const hkVector4& a) { set(-a.x, -a.y, -a.z, a.w); }
    void setAdd4(const hkVector4& a, const hkVector4& b) { set(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
    void setSub4(const hkVector4& a, const hkVector4& b) { set(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
    void setMul4(const hkVector4& a, hkReal s) { set(a.x * s, a.y * s, a.z * s, a.w * s); }
    void setCross(const hkVector4& a, const hkVector4& b) {
        set(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, 0.0f);
    }
    void mul4(hkReal s) { setMul4(*this, s); }
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
    // Component access by index (x, y, z, w).
    hkReal& operator[](int i) { return (&x)[i]; }
    hkReal getSimdAt(int i) const { return (&x)[i]; }
    hkReal dot3(const hkVector4& a) const { return x * a.x + y * a.y + z * a.z; }
    hkReal lengthSquared3() const { return dot3(*this); }
    hkReal lengthSquared4() const { return dot3(*this) + w * w; }
    hkReal length3() const { return sqrtf(lengthSquared3()); }
    void normalize3() {
        hkReal inv = 1.0f / length3();
        set(x * inv, y * inv, z * inv, w);
    }

    // Transforms by a rigid transform / rotation. Asm-verified in src/havok/hkVector4.cpp (once matched).
    // setTransformedPos: this = R * v + t.   setRotatedDir: this = R * v.
    void setTransformedPos(const hkTransform& t, const hkVector4& v);
    void setTransformedInversePos(const hkTransform& t, const hkVector4& v);
    void setRotatedDir(const hkRotation& r, const hkVector4& v);
    void setRotatedInverseDir(const hkRotation& r, const hkVector4& v);
} __attribute__((aligned(16)));
