#pragma once

#include <havok/hkRotation.h>

// Rigid transform: rotation followed by translation (0x40 bytes).
struct hkTransform {
    hkRotation m_rotation;     // 0x00
    hkVector4 m_translation;   // 0x30

    // Out-of-line members defined in src/havok/hkInertiaTensorComputer.cpp.
    void setRotation(const hkQuaternion& q);
    void setIdentity();
    void setTranslation(const hkVector4& t);
    hkVector4& getTranslation();

    void get4x4ColumnMajor(hkReal* m) const;
    void set4x4ColumnMajor(const hkReal* m);
    void setInverse(const hkTransform& t);
    void setMul(const hkTransform& a, const hkTransform& b);
    void setMulInverseMul(const hkTransform& a, const hkTransform& b);
};
