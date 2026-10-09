#pragma once

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
    virtual ~hkRigidBodyDeactivator(); // 0x08 (out of line in src/havok/hkFakeRigidBodyDeactivator.cpp)

    virtual int getRigidBodyDeactivatorType() const = 0;         // 0x0C
    // HYPOTHESIS: the real parameters are not visible in the implementations (they ignore them).
    virtual hkBool shouldDeactivateHighFrequency() const = 0;    // 0x10
    virtual hkBool shouldDeactivateLowFrequency() const = 0;     // 0x14
};
