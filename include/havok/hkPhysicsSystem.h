#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkMemory.h>

struct hkWorldObject;

// Container of rigid bodies, constraints, actions and phantoms (0x44 bytes).
// Layout from hkPhysicsSystemClass.cpp. The four arrays are hkArray<T*> (the destructor runs their
// destructors in reverse order); the finish-loaded constructor leaves them as loaded.
struct hkPhysicsSystem : hkReferencedObject {
    hkArray<hkWorldObject*> m_rigidBodies;        // 0x08 (hkRigidBody*)
    hkArray<hkReferencedObject*> m_constraints;   // 0x14 (hkConstraintInstance*)
    hkArray<hkReferencedObject*> m_actions;       // 0x20 (hkAction*)
    hkArray<hkWorldObject*> m_phantoms;           // 0x2C (hkPhantom*)
    const char* m_name;          // 0x38
    void* m_userData;            // 0x3C
    hkBool m_active;             // 0x40

    HK_DECLARE_REF_ALLOCATOR(0x2D)

    hkPhysicsSystem()
        : m_rigidBodies(hkFinishLoadedObjectFlag()), m_constraints(hkFinishLoadedObjectFlag()),
          m_actions(hkFinishLoadedObjectFlag()), m_phantoms(hkFinishLoadedObjectFlag()) {}

    static void finishLoadedObjecthkPhysicsSystem(void* p);
    static void cleanupLoadedObjecthkPhysicsSystem(void* p);
    static const void* getVtablehkPhysicsSystem();

    virtual ~hkPhysicsSystem();
};
