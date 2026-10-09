#pragma once

#include <havok/hkBase.h>
#include <havok/hkMemory.h>

// Base entity deactivator (0x08 bytes: vtable only). Layout from hkEntityDeactivatorClass.cpp.
// The class-specific allocator (id 0x29) is inherited by the deactivator subclasses.
struct hkEntityDeactivator : hkReferencedObject {
    HK_DECLARE_REF_ALLOCATOR(0x29)
    static void* operator new(unsigned long, void* where) { return where; } // placement new for loaded objects

    hkEntityDeactivator() {}
    hkEntityDeactivator(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkEntityDeactivator(); // 0x08 (out of line in src/havok/hkFakeRigidBodyDeactivator.cpp)
};
