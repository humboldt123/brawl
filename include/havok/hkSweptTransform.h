#pragma once

#include <havok/hkQuaternion.h>

// Start and end pose of a moving body over one step (0x50 bytes). Layout from hkSweptTransformClass.cpp.
struct hkSweptTransform {
    hkVector4 m_centerOfMass0;      // 0x00
    hkVector4 m_centerOfMass1;      // 0x10
    hkQuaternion m_rotation0;       // 0x20
    hkQuaternion m_rotation1;       // 0x30
    hkVector4 m_centerOfMassLocal;  // 0x40

    // Both ends at the given pose; the w components (base time and inverse delta time) and the local center of
    // mass are zeroed.
    void initSweptTransform(const hkVector4& position, const hkQuaternion& rotation);
};
