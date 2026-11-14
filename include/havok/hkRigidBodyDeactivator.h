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

    // shouldDeactivateHighFrequency / LowFrequency                       0x10 / 0x14 (hkEntityDeactivator)
    virtual int getRigidBodyDeactivatorType() const = 0;                // 0x18
};
