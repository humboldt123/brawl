#pragma once

#include <havok/hkWorldObject.h>
#include <havok/hkMotion.h>
#include <havok/hkArray.h>

struct hkSimulationIsland;
struct hkEntityDeactivator;
struct hkEntityListener;
struct hkCollisionListener;
struct hkConstraintInstance; // HYPOTHESIS: pointer stored in the constraint arrays

// Simulated body with a motion (0x1F0 bytes). Derived from hkWorldObject.
// Layout from hkEntityClass.cpp.
struct hkEntity : hkWorldObject {
    hkSimulationIsland* m_simulationIsland; // 0x58
    u8 m_material[0xC];                     // 0x5C (hkMaterial, layout not recovered yet)
    hkEntityDeactivator* m_deactivator;     // 0x68
    // Master constraints: 0x24-byte elements whose first word is the constraint pointer.
    hkArrayBase m_constraintsMaster;        // 0x6C
    // Slave constraints: plain pointers.
    hkArrayBase m_constraintsSlave;         // 0x78
    hkArrayBase m_constraintRuntime;        // 0x84 (u8 entries)
    u16 m_storageIndex;                     // 0x90
    u16 m_processContactCallbackDelay;      // 0x92
    s8 m_autoRemoveLevel;                   // 0x94
    u8 m_padA5[0xB];                        // 0x95
    // hkMaxSizeMotion (0x110 bytes, a hkMotion subobject). Kept as raw storage: hkMotion is abstract.
    u8 m_motion[0x110];                     // 0xA0
    u32 m_solverData;                       // 0x1B0
    hkArrayBase m_collisionListeners;       // 0x1B4 (pointers)
    hkArrayBase m_activationListeners;      // 0x1C0
    hkArrayBase m_entityListeners;          // 0x1CC (pointers)
    hkArrayBase m_actions;                  // 0x1D8
    u32 m_uid;                              // 0x1E4
    u8 m_padE8[0x8];                        // 0x1E8

    hkEntity(hkFinishLoadedObjectFlag flag);

    // Loaded-object support (static init registration).
    static void finishLoadedObjecthkEntity(void* p);
    static void cleanupLoadedObjecthkEntity(void* p);
    static const void* getVtablehkEntity();

    hkMotion* getMotion() { return (hkMotion*)m_motion; }

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
    u32 isActive() const;
    void activate();
    void deactivate();
    int getNumConstraints() const;
    hkConstraintInstance* getConstraint(int index) const;
};
