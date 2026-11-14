#pragma once

#include <havok/hkWorldObject.h>
#include <havok/hkMemory.h>
#include <havok/hkMotion.h>
#include <havok/hkArray.h>
#include <havok/hkMaterial.h>
#include <havok/hkKeyframedRigidMotion.h>
#include <havok/hkConstraintInternal.h>

struct hkSimulationIsland;
struct hkEntityDeactivator;
struct hkEntityListener;
struct hkCollisionListener;
struct hkConstraintInstance; // HYPOTHESIS: pointer stored in the constraint arrays
struct hkEntityActivationListener;
struct hkAction;

// Simulated body with a motion (0x1F0 bytes). Derived from hkWorldObject.
// Layout from hkEntityClass.cpp.
struct hkEntity : hkWorldObject {
    hkSimulationIsland* m_simulationIsland; // 0x58
    hkMaterial m_material;                  // 0x5C
    hkEntityDeactivator* m_deactivator;     // 0x68
    // Master constraints: 0x24-byte elements whose first word is the constraint pointer.
    hkArray<hkConstraintInternal> m_constraintsMaster; // 0x6C
    // Slave constraints: plain pointers.
    hkArray<hkConstraintInstance*> m_constraintsSlave; // 0x78
    hkArray<u8> m_constraintRuntime;        // 0x84
    u16 m_storageIndex;                     // 0x90
    u16 m_processContactCallbackDelay;      // 0x92
    s8 m_autoRemoveLevel;                   // 0x94
    u8 m_padA5[0xB];                        // 0x95
    // Built as an hkMaxSizeMotion and replaced in place by the motion of the loaded type (the loaded-object
    // constructor), so it is reached through getMotion().
    hkMaxSizeMotion m_motion;               // 0xA0
    u32 m_solverData;                       // 0x1B0
    hkArray<hkCollisionListener*> m_collisionListeners; // 0x1B4
    hkArray<hkEntityActivationListener*> m_activationListeners; // 0x1C0
    hkArray<hkEntityListener*> m_entityListeners; // 0x1CC
    hkArray<hkAction*> m_actions;           // 0x1D8
    u32 m_uid;                              // 0x1E4
    u8 m_padE8[0x8];                        // 0x1E8

    hkEntity(hkShape* shape);
    hkEntity(hkFinishLoadedObjectFlag flag);
    // Entities (and hkRigidBody) are freed as class 0x2B (the deleting destructors of both).
    HK_DECLARE_REF_ALLOCATOR(0x2B)
    static void* operator new(unsigned long, void* p) { return p; } // the loaded-object constructors
    // Out of line in hkEntity.o (hkRigidBody's destructor calls it).
    virtual ~hkEntity();

    // Loaded-object support (static init registration).
    static void finishLoadedObjecthkEntity(void* p);
    static void cleanupLoadedObjecthkEntity(void* p);
    static const void* getVtablehkEntity();

    hkMotion* getMotion() { return &m_motion; }
    hkBool isFixed() { return getMotion()->m_type == hkMotion::MOTION_FIXED; }
    hkLinkedCollidable* getLinkedCollidable(); // out of line in src/havok/hkSimulation.cpp (fn_802F2E3C)

    virtual hkMotionState* getMotionState();

    // Top two bits of the island's byte at 0x27 (activity state), 0 without an island. HYPOTHESIS: meaning.
    u8 getActivationState() const {
        if (m_simulationIsland == 0) {
            return 0;
        }
        u32 bits = ((u8*)m_simulationIsland)[0x27];
        return (bits >> 6) & 3;
    }

    void setDeactivator(hkEntityDeactivator* deactivator);
    void addEntityListener(hkEntityListener* listener);
    void addCollisionListener(hkCollisionListener* listener);
    hkBool isActive() const;
    void activate();
    void deactivate();
    int getNumConstraints() const;
    hkConstraintInstance* getConstraint(int index) const;
};
