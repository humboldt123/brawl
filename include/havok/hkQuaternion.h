#pragma once

#include <havok/hkVector4.h>

// Unit quaternion stored as (x, y, z, w) in m_vec.
struct hkQuaternion {
    hkVector4 m_vec; // 0x00

    hkQuaternion() {}
    hkQuaternion(hkReal x, hkReal y, hkReal z, hkReal w) { m_vec.set(x, y, z, w); }

    void setAxisAngle(const hkVector4& axis, hkReal angle);
    // Builds the quaternion from a rotation matrix (calls hkQuadReal::quaternionFromRotatation).
    void set(const hkRotation& r);
    void setFlippedRotation(const hkQuaternion& q);
    // Scales to unit length; defined in src/havok/hkFootPlacementIkSolver.cpp.
    void normalize();
};

// Helper class from the Havok TU map (hkQuadReal__quaternionFromRotatation).
struct hkQuadReal {
    static void quaternionFromRotatation(hkQuaternion* out, const hkRotation& r);
};
