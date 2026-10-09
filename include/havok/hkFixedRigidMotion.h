#pragma once

#include <havok/hkKeyframedRigidMotion.h>

// Fixed rigid motion (type 7, 0x110 bytes). Same layout as the keyframed motion it derives from.
struct hkFixedRigidMotion : hkKeyframedRigidMotion {
    hkFixedRigidMotion(const hkVector4& position, const hkQuaternion& rotation);
    hkFixedRigidMotion(hkFinishLoadedObjectFlag f) : hkKeyframedRigidMotion(f) {}
    virtual ~hkFixedRigidMotion(); // 0x08 (out of line in src/havok/hkFixedRigidMotion.cpp)

    // Loaded-object support (static init registration; see src/havok/hkFixedRigidMotion.cpp).
    static void finishLoadedObjecthkFixedRigidMotion(void* p);
    static void cleanupLoadedObjecthkFixedRigidMotion(void* p);
    static const void* getVtablehkFixedRigidMotion() __attribute__((never_inline));

    virtual void setStepPosition(hkReal stepPosition);                  // HYPOTHESIS: parameter unused
    // HYPOTHESIS: the base motion's slot 0x64 (getMotionStateAndVelocities) may be this function.
    virtual void getPositionAndVelocities(hkMotion* out) const;
};
