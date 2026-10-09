#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkMemory.h>

// Container of rigid bodies, constraints, actions and phantoms (0x44 bytes).
// Layout from hkPhysicsSystemClass.cpp. The four arrays are hkArray<T*> in the original; they are
// untyped here (hkArrayBase) so the finish-loaded constructor does not default-construct them.
struct hkPhysicsSystem : hkReferencedObject {
    hkArrayBase m_rigidBodies;   // 0x08 (hkRigidBody*)
    hkArrayBase m_constraints;   // 0x14 (hkConstraintInstance*)
    hkArrayBase m_actions;       // 0x20 (hkAction*)
    hkArrayBase m_phantoms;      // 0x2C (hkPhantom*)
    const char* m_name;          // 0x38
    void* m_userData;            // 0x3C
    hkBool m_active;             // 0x40

    HK_DECLARE_REF_ALLOCATOR(0x2D)

    hkPhysicsSystem() {}

    static void finishLoadedObjecthkPhysicsSystem(void* p);
    static void cleanupLoadedObjecthkPhysicsSystem(void* p);
    static const void* getVtablehkPhysicsSystem();

    virtual ~hkPhysicsSystem();
};
