#pragma once

#include <havok/hkSphereMotion.h>

// Stabilized sphere-inertia motion (type 3). hkEntity's loaded-object constructor builds it in place; its
// vtable and destructor are emitted in hkEntity.o. HYPOTHESIS: no slot of its own beyond the destructor (as
// hkStabilizedBoxMotion).
struct hkStabilizedSphereMotion : hkSphereMotion {
    hkStabilizedSphereMotion(const hkVector4& position, const hkQuaternion& rotation)
        : hkSphereMotion(position, rotation) {
        m_type = MOTION_STABILIZED_SPHERE_INERTIA;
    }
    hkStabilizedSphereMotion(hkFinishLoadedObjectFlag f) : hkSphereMotion(f) {}
};
