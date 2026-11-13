#pragma once

struct hkEntity;

#include <havok/hkEntityDeactivator.h>
#include <havok/hkRegistry.h>

// Static type registration record for deactivators (same layout as hkTypeInfo; see hkMotion.h).
struct hkRigidBodyDeactivatorTypeInfo : hkTypeInfo {
    hkRigidBodyDeactivatorTypeInfo(const char* name, void (*finish)(void*), void (*cleanup)(void*), const void* vtable) {
        m_name = name;
        m_finish = finish;
        m_cleanup = cleanup;
        m_vtable = vtable;
    }
};

// Rigid-body deactivator (0x08 bytes, no fields). Layout from hkRigidBodyDeactivatorClass.cpp.
struct hkRigidBodyDeactivator : hkEntityDeactivator {
    enum DeactivatorType {
        DEACTIVATOR_INVALID = 0,
        DEACTIVATOR_NEVER = 1,
        DEACTIVATOR_SPATIAL = 2,
        DEACTIVATOR_MAX_ID = 3,
    };

    hkRigidBodyDeactivator() {}
    hkRigidBodyDeactivator(hkFinishLoadedObjectFlag flag) : hkEntityDeactivator(flag) {}
    // BYTES: inline; ~hkSpatialRigidBodyDeactivator has no call to it (weak copy kept in hkFakeRigidBodyDeactivator.o)
    virtual ~hkRigidBodyDeactivator() {} // 0x08

    virtual int getRigidBodyDeactivatorType() const = 0;         // 0x0C
    // The spatial deactivator reads the entity's motion state and stores new reference samples (not const).
    // HYPOTHESIS: a non-const entity (the sample copies keep their load/store pairs, so the source may alias).
    virtual hkBool shouldDeactivateHighFrequency(hkEntity* entity) = 0; // 0x10
    virtual hkBool shouldDeactivateLowFrequency(hkEntity* entity) = 0;  // 0x14
};
