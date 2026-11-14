#pragma once

#include <havok/hkVector4.h>

// Unit quaternion stored as (x, y, z, w) in m_vec.
struct hkQuaternion {
    hkVector4 m_vec; // 0x00

    hkQuaternion() {}
    hkQuaternion(hkReal x, hkReal y, hkReal z, hkReal w) { m_vec.set(x, y, z, w); }
    // Out of line (hkSimulation.o): rotation by angle around axis.
    hkQuaternion(const hkVector4& axis, hkReal angle);

    void setIdentity() { m_vec.set(0.0f, 0.0f, 0.0f, 1.0f); }
    void setAxisAngle(const hkVector4& axis, hkReal angle);
    // Builds the quaternion from a rotation matrix (calls hkQuadReal::quaternionFromRotatation).
    void set(const hkRotation& r);
    void setFlippedRotation(const hkQuaternion& q);
    // this = conjugate(q0) * q1 (out of line in hkSimulation.o).
    void setInverseMul(const hkQuaternion& q0, const hkQuaternion& q1);
    // Scales to unit length; defined in src/havok/hkFootPlacementIkSolver.cpp.
    void normalize();
    // Quaternion product this = q0 * q1: imaginary part q0 x q1 + q0.w * q1 + q1.w * q0, real part
    // q0.w * q1.w - q0 . q1. HYPOTHESIS: the exact statement form; the lane order and the dead w lane of the
    // temporary come from hkTransformAgent::processCollision.
    void setMul(const hkQuaternion& q0, const hkQuaternion& q1) {
        hkVector4 vec;
        vec.setCross(q0.m_vec, q1.m_vec);
        vec.addMul4(q0.m_vec.w, q1.m_vec);
        vec.addMul4(q1.m_vec.w, q0.m_vec);
        hkSimdReal w = q0.m_vec.w * q1.m_vec.w - q0.m_vec.dot3(q1.m_vec);
        m_vec.set(vec.x, vec.y, vec.z, w);
    }
};

// Helper class from the Havok TU map (hkQuadReal__quaternionFromRotatation).
struct hkQuadReal {
    static void quaternionFromRotatation(hkQuaternion* out, const hkRotation& r);
};
