#pragma once

#include <havok/hkRigidBodyDeactivator.h>

// Deactivator that never lets a body sleep (DEACTIVATOR_NEVER). No fields.
struct hkFakeRigidBodyDeactivator : hkRigidBodyDeactivator {
    hkFakeRigidBodyDeactivator() {}
    hkFakeRigidBodyDeactivator(hkFinishLoadedObjectFlag flag) : hkRigidBodyDeactivator(flag) {}
    virtual ~hkFakeRigidBodyDeactivator(); // 0x08

    // Loaded-object support (static init registration; see src/havok/hkFakeRigidBodyDeactivator.cpp).
    static void finishLoadedObjecthkFakeRigidBodyDeactivator(void* p);
    static void cleanupLoadedObjecthkFakeRigidBodyDeactivator(void* p);
    static const void* getVtablehkFakeRigidBodyDeactivator() __attribute__((never_inline));

    virtual int getRigidBodyDeactivatorType() const;      // 0x0C
    virtual hkBool shouldDeactivateHighFrequency() const; // 0x10
    virtual hkBool shouldDeactivateLowFrequency() const;  // 0x14
};
