#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkVector4.h>
#include <havok/hkStepInfo.h>
#include <havok/hkSolverInfo.h>
#include <havok/hkWorldMaintenanceMgr.h>

struct hkWorldOperationQueue;
struct hkWorld;
// Listener told when its world is deleted (virtual destructor first, then the callback: hkBroadPhaseBorder's
// secondary vtable).
struct hkWorldDeletionListener {
    virtual ~hkWorldDeletionListener() {}
    virtual void worldDeletedCallback(hkWorld* world) = 0;
};
struct hkWorldMemoryWatchDog;
struct hkPhantom;
struct hkAction;
struct hkEntity;
struct hkConstraintInstance;
struct hkPhysicsSystem;
struct hkCollisionFilter;
struct hkCollisionDispatcher;
struct hkStatisticsCollector;
struct hkAabb;
struct hkProcessCollisionInput;
struct hkBroadPhase;
struct hkSimulation;
struct hkTypedBroadPhaseDispatcher;

// The simulation world (layout partly recovered from hkWorld.cpp asm; the tail after 0x114 is not
// recovered). Fields marked unk are written by the constructor and not read by the code matched so far.
// The world's current step and its solver settings (0x150 bytes). Evidence for the pair: integrateInternal hands
// hkConstraintSolverSetup::solve the address of the step and the address 0x10 past it.
struct hkWorldDynamicsStepInfo {
    hkStepInfo m_stepInfo;     // 0x00
    hkSolverInfo m_solverInfo; // 0x10
};

struct hkWorld : hkReferencedObject {
    hkSimulation* m_simulation;                     // 0x08 (hkWorld::stepDeltaTime calls its slot 0x10, stepDeltaTime)
    hkVector4 m_gravity;                            // 0x10
    u8 m_unk20[0x08];                               // 0x20 .. 0x27 not recovered
    hkArrayBase m_unk28;                            // 0x28 array of hkWorldMemoryEntry* (HYPOTHESIS: islands), see getMemUsageForIntegration
    hkArrayBase m_unk34;                            // 0x34 not recovered
    hkArrayBase m_unk40;                            // 0x40 not recovered
    hkWorldMaintenanceMgr* m_maintenanceMgr;        // 0x4C (hkSimulation::integrate calls performMaintenance)
    u8 m_unk50[0x04];                               // 0x50 not recovered
    hkBroadPhase* m_broadPhase;                     // 0x54 (hkSimulation::collideEntitiesBroadPhaseDiscrete updates it)
    hkTypedBroadPhaseDispatcher* m_broadPhaseDispatcher; // 0x58 gets the pairs the broad phase reports
    u8 m_unk5C[0x0C];                               // 0x5C .. 0x67 not recovered
    hkProcessCollisionInput* m_collisionInput;      // 0x68 HYPOTHESIS: name; integrate copies the step info into it
    hkCollisionFilter* m_collisionFilter;           // 0x6C handed to the dispatcher with the new pairs
    hkCollisionDispatcher* m_collisionDispatcher;   // 0x70 HYPOTHESIS: name; hkWorldAgentUtil::removeAgent hands it to
                                                    // hkAgentNnMachine_DestroyAgent
    hkWorldOperationQueue* m_operationQueue;        // 0x74 (allocated 0x20 bytes in the constructor)
    int m_pendingOperationsCount;                                    // 0x78 how many world operations are sitting in the queue
    int m_lockCount;                                // 0x7C incremented around add/remove calls
    int m_unk80;                                    // 0x80 summed with m_lockCount in removePhantom
    hkBool m_unk84;                                 // 0x84 flag: operations are being executed
    u8 m_unk85;                                     // 0x85 (initialised to 1)
    int m_unk88;                                    // 0x88
    int m_pendingOperationQueueCount;               // 0x8C how many executeAllPending calls are on the stack (starts at 1)
    int m_unk90;                                    // 0x90
    u8 m_unk94[0x08];                               // 0x94 .. 0x9B not recovered
    int m_unk9C;                                    // 0x9C
    int m_unkA0;                                    // 0xA0
    int m_unkA4;                                    // 0xA4
    u8 m_unkA8[0x0D];                               // 0xA8 .. 0xB4 not recovered
    // hkDefaultWorldMaintenanceMgr::performMaintenance only looks for islands to deactivate when it is set
    hkBool m_unkB5;                                 // 0xB5 HYPOTHESIS: a deactivation switch
    u8 m_unkB6[0x12];                               // 0xB6 .. 0xC7 not recovered
    int m_unkC8;                                    // 0xC8 (initialised to -1)
    hkArray<hkPhantom*> m_phantoms;                 // 0xCC
    u8 m_unkD8[0x30];                               // 0xD8 .. 0x107 not recovered
    hkArray<hkWorldDeletionListener*> m_worldDeletionListeners; // 0x108
    u8 m_unk114[0x0C];                              // 0x114 .. 0x11F not recovered
    // listener arrays (hkWorldCallbackUtil reads them at these offsets; element types there)
    hkArrayBase m_postSimulationListeners;          // 0x120
    // tested before hkWorldCallbackUtil::firePostIntegrateCallback (hkSimulation::integrateInternal)
    hkArrayBase m_postIntegrateListeners;           // 0x12C HYPOTHESIS: name, from the callback it guards
    hkArrayBase m_postCollideListeners;             // 0x138
    // tested before hkWorldCallbackUtil::fireIslandPostIntegrateCallback (hkSimulation::integrateInternal)
    hkArrayBase m_islandPostIntegrateListeners;     // 0x144 HYPOTHESIS: name, from the callback it guards
    // tested before hkWorldCallbackUtil::fireIslandPostCollideCallback (hkSimulation::collideInternal)
    hkArrayBase m_islandPostCollideListeners;       // 0x150 HYPOTHESIS: name, from the callback it guards
    u8 m_unk15C[0x14];                              // 0x15C .. 0x16F not recovered
    // the current step and the solver settings (hkSimulation::integrate copies the step in; actions get it in
    // applyActions)
    hkWorldDynamicsStepInfo m_dynamicsStepInfo;     // 0x170 HYPOTHESIS: name
    // 0x2C0 the broadphase extents (min, max) as two vector4s (hkBroadPhaseBorder places its phantoms on them).
    // HYPOTHESIS: Havok's hkVector4 m_broadPhaseExtents[2]; kept as floats so the class layout stays as it is.
    hkReal m_broadPhaseExtents[2][4];               // 0x2C0
    u8 m_unk2E0[0x2E4 - 0x2E0];                     // 0x2E0 not recovered
    int m_unk2E4;                                   // 0x2E4 capacity of hkWorldOperationUtil's local pair arrays
    int m_unk2E8;                                   // 0x2E8 capacity of collideEntitiesBroadPhaseDiscrete's pair arrays

    int getMemUsageForIntegration();
    void setGravity(const hkVector4& gravity);
    void queueOperation(const void* operation);
    void addWorldDeletionListener(hkWorldDeletionListener* listener);
    void removeWorldDeletionListener(hkWorldDeletionListener* listener);
    hkPhantom* addPhantom(hkPhantom* phantom);
    void removePhantom(hkPhantom* phantom);
    void removeAction(hkAction* action);
    void removeActionImmediately(hkAction* action);
    void addEntityBatch(hkEntity** entities, int count, bool active);
    void removeEntity(hkEntity* entity);
    void removeEntityBatch(hkEntity** entities, int count);
    hkEntity* addEntity(hkEntity* entity, int activation);
    void activateRegion(hkAabb* aabb);
    int stepDeltaTime(hkReal physicsDeltaTime);
    void updateCollisionFilterOnEntity(hkEntity* entity, u8 updateMode, u8 updateShapeCollections);
    void updateCollisionFilterOnPhantom(hkPhantom* phantom, u8 updateShapeCollections);
    void updateCollisionFilterOnWorld(u8 updateMode, u8 updateShapeCollections);
    void addAction(hkAction* action);
    hkConstraintInstance* addConstraint(hkConstraintInstance* constraint);
    void removeConstraint(hkConstraintInstance* constraint);
    void addPhantomBatch(hkPhantom** phantoms, int count);
    void removePhantomBatch(hkPhantom** phantoms, int count);
    void addPhysicsSystem(hkPhysicsSystem* system);
    void removePhysicsSystem(hkPhysicsSystem* system);
    void lock();
    void unlock();
    void lockIslandForConstraintUpdate();
    void unlockIslandForConstraintUpdate();
    // Runs the queued world operations (map name hkWorld__internal_executePendingOperations, fn_802EFE70).
    void internal_executePendingOperations();
    // drop one lock and, if that was the last one and something got queued meanwhile, run the queue
    // BYTES: the flag goes through hkBool's bool conversion before the negation (hkBool::operator! builds a second
    // hkBool); in the propagation-off hkContinuousSimulation::handleAllToisTill that gives extsb; neg; or; srwi.
    void unlockAndAttemptToExecutePendingOperations() {
        if (--m_lockCount == 0 && m_pendingOperationsCount != 0 && !(bool)m_unk84) {
            internal_executePendingOperations();
        }
    }
};
