#pragma once

#include <havok/hkBase.h>
#include <havok/hkMemory.h>

struct hkEntity;

// Base entity deactivator (0x08 bytes: vtable only). Layout from hkEntityDeactivatorClass.cpp.
// The class-specific allocator (id 0x29) is inherited by the deactivator subclasses.
struct hkEntityDeactivator : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x29)
    static void* operator new(unsigned long, void* where) { return where; } // placement new for loaded objects

    hkEntityDeactivator() {}
    hkEntityDeactivator(hkFinishLoadedObjectFlag flag) {}
    // BYTES: inline; ~hkSpatialRigidBodyDeactivator has no call to it (weak copy kept in hkFakeRigidBodyDeactivator.o)
    virtual ~hkEntityDeactivator() {} // 0x08
    // calcStatistics                    0x0C (inherited)
    // The spatial deactivator reads the entity's motion state and stores new reference samples (not const).
    // HYPOTHESIS: a non-const entity (the sample copies keep their load/store pairs, so the source may alias).
    virtual hkBool shouldDeactivateHighFrequency(hkEntity* entity) = 0; // 0x10
    virtual hkBool shouldDeactivateLowFrequency(hkEntity* entity) = 0;  // 0x14
};
