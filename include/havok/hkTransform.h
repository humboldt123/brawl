#pragma once

#include <havok/hkRotation.h>

// Rigid transform: rotation followed by translation (0x40 bytes).
struct hkTransform {
    hkRotation m_rotation;     // 0x00
    hkVector4 m_translation;   // 0x30

    // Empty default constructor (map name hkTransform::hkTransform; its out-of-line copy in hkBvTreeStreamAgent.o is a
    // bare blr).
    hkTransform() {}

    // Out-of-line members defined in src/havok/hkInertiaTensorComputer.cpp.
    void setRotation(const hkQuaternion& q);
    // Identity rotation, zero translation (out-of-line copy in hkInertiaTensorComputer.o).
    void setIdentity() {
        m_rotation.setIdentity();
        m_translation.setZero4();
    }
    void setTranslation(const hkVector4& t);
    hkVector4& getTranslation();

    void get4x4ColumnMajor(hkReal* m) const;
    void set4x4ColumnMajor(const hkReal* m);
    void setInverse(const hkTransform& t);
    void setMul(const hkTransform& a, const hkTransform& b);
    void setMulInverseMul(const hkTransform& a, const hkTransform& b);

    // Out-of-line copy (a bare blr) in hkPoweredChainData.o.
    const hkRotation& getRotation() const { return m_rotation; }
    // Column i of the rotation (out-of-line copy in hkSimpleContactConstraintInfo.o).
    const hkVector4& getColumn(int i) const { return (&m_rotation.m_col0)[i]; }
    // The identity transform in .data (out-of-line copy in hkSimpleContactConstraintInfo.o).
    static inline const hkTransform& getIdentity();
};

// The identity transform (unnamed in .data; hkMathUtil reads its rotation as an hkMatrix3).
extern hkMatrix3 lbl_804864F0;

inline const hkTransform& hkTransform::getIdentity() {
    return (const hkTransform&)lbl_804864F0;
}

inline void hkVector4::_setTransformedPos(const hkTransform& t, const hkVector4& v) {
    const hkRotation& r = t.m_rotation;
    hkReal vz = v.z;
    hkReal vy = v.y;
    hkReal vx = v.x;
    x = t.m_translation.x + (vx * r.m_col0.x + vy * r.m_col1.x + vz * r.m_col2.x);
    y = t.m_translation.y + (vx * r.m_col0.y + vy * r.m_col1.y + vz * r.m_col2.y);
    z = t.m_translation.z + (vx * r.m_col0.z + vy * r.m_col1.z + vz * r.m_col2.z);
    w = 0.0f;
}

inline void hkVector4::_setTransformedInversePos(const hkTransform& t, const hkVector4& v) {
    const hkRotation& r = t.m_rotation;
    hkReal dx = v.x - t.m_translation.x;
    hkReal dy = v.y - t.m_translation.y;
    hkReal dz = v.z - t.m_translation.z;
    x = dx * r.m_col0.x + dy * r.m_col0.y + dz * r.m_col0.z;
    y = dx * r.m_col1.x + dy * r.m_col1.y + dz * r.m_col1.z;
    z = dx * r.m_col2.x + dy * r.m_col2.y + dz * r.m_col2.z;
    w = 0.0f;
}
