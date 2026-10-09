#pragma once

#include <havok/hkTransform.h>
#include <havok/hkSweptTransform.h>

// Per-body motion state (0xC0 bytes). Layout from hkMotionStateClass.cpp.
struct hkMotionState {
    hkTransform m_transform;           // 0x00
    hkSweptTransform m_sweptTransform; // 0x40
    hkVector4 m_deltaAngle;            // 0x90
    hkReal m_objectRadius;             // 0xA0
    hkReal m_maxLinearVelocity;        // 0xA4
    hkReal m_maxAngularVelocity;       // 0xA8
    hkReal m_linearDamping;            // 0xAC
    hkReal m_angularDamping;           // 0xB0
    u16 m_deactivationClass;           // 0xB4
    u16 m_deactivationCounter;         // 0xB6

    void initMotionState(const hkVector4& position, const hkQuaternion& rotation);
};
