#pragma once

#include <havok/hkQuaternion.h>

// Translation, rotation quaternion and per-axis scale (0x30 bytes).
struct hkQsTransform {
    hkVector4 m_translation; // 0x00
    hkQuaternion m_rotation; // 0x10
    hkVector4 m_scale;       // 0x20

    // Non-inline accessors, defined in src/havok/hkFootPlacementIkSolver.cpp (asm: returns this / this+0x10).
    hkVector4& getTranslation();
    hkQuaternion& getRotation();

    void get4x4ColumnMajor(hkReal* m) const;
    void set4x4ColumnMajor(const hkReal* m);
    void setInverse(const hkQsTransform& t);
    void setMul(const hkQsTransform& a, const hkQsTransform& b);
    void setMulInverseMul(const hkQsTransform& a, const hkQsTransform& b);
};
