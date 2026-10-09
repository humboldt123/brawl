#pragma once

#include <havok/hkBase.h>
#include <havok/hkAabb.h>
#include <havok/hkVector4.h>

struct hkCollisionFilter;
struct hkWorldMemoryWatchDog;

// World construction info (0xA0 bytes). Layout from hkWorldCinfoClass.cpp.
struct hkWorldCinfo : hkReferencedObject {
    enum SolverType {
        SOLVER_TYPE_INVALID,
        SOLVER_TYPE_2ITERS_SOFT,
        SOLVER_TYPE_2ITERS_MEDIUM,
        SOLVER_TYPE_2ITERS_HARD,
        SOLVER_TYPE_4ITERS_SOFT,
        SOLVER_TYPE_4ITERS_MEDIUM,
        SOLVER_TYPE_4ITERS_HARD,
        SOLVER_TYPE_8ITERS_SOFT,
        SOLVER_TYPE_8ITERS_MEDIUM,
        SOLVER_TYPE_8ITERS_HARD,
        SOLVER_TYPE_MAX_ID,
    };

    enum SimulationType {
        SIMULATION_TYPE_INVALID,
        SIMULATION_TYPE_DISCRETE,
        SIMULATION_TYPE_CONTINUOUS,
        SIMULATION_TYPE_MULTITHREADED,
    };

    enum ContactPointGeneration {
        CONTACT_POINT_ACCEPT_ALWAYS,
        CONTACT_POINT_REJECT_DUBIOUS,
        CONTACT_POINT_REJECT_MANY,
    };

    enum BroadPhaseBorderBehaviour {
        BROADPHASE_BORDER_ASSERT,
        BROADPHASE_BORDER_FIX_ENTITY,
        BROADPHASE_BORDER_REMOVE_ENTITY,
        BROADPHASE_BORDER_DO_NOTHING,
    };

    hkVector4 m_gravity;                              // 0x10
    int m_broadPhaseQuerySize;                        // 0x20
    hkReal m_contactRestingVelocity;                  // 0x24
    u8 m_broadPhaseBorderBehaviour;                   // 0x28
    hkAabb m_broadPhaseWorldAabb;                     // 0x30
    hkReal m_collisionTolerance;                      // 0x50
    hkCollisionFilter* m_collisionFilter;             // 0x54
    hkReal m_expectedMaxLinearVelocity;               // 0x58
    hkReal m_expectedMinPsiDeltaTime;                 // 0x5C
    hkWorldMemoryWatchDog* m_memoryWatchDog;          // 0x60
    int m_broadPhaseNumMarkers;                       // 0x64
    u8 m_contactPointGeneration;                      // 0x68
    hkReal m_solverTau;                               // 0x6C
    hkReal m_solverDamp;                              // 0x70
    int m_solverIterations;                           // 0x74
    int m_solverMicrosteps;                           // 0x78
    hkReal m_iterativeLinearCastEarlyOutDistance;     // 0x7C
    int m_iterativeLinearCastMaxIterations;           // 0x80
    hkReal m_highFrequencyDeactivationPeriod;         // 0x84
    hkReal m_lowFrequencyDeactivationPeriod;          // 0x88
    hkBool m_shouldActivateOnRigidBodyTransformChange; // 0x8C
    hkReal m_toiCollisionResponseRotateNormal;        // 0x90
    hkBool m_enableDeactivation;                      // 0x94
    u8 m_simulationType;                              // 0x95
    hkBool m_enableSimulationIslands;                 // 0x96
    hkBool m_processActionsInSingleThread;            // 0x97
    hkReal m_frameMarkerPsiSnap;                      // 0x98

    hkWorldCinfo();
    hkWorldCinfo(hkFinishLoadedObjectFlag flag);

    static void finishLoadedObjecthkWorldCinfo(void* p);
    static void cleanupLoadedObjecthkWorldCinfo(void* p);
    static const void* getVtablehkWorldCinfo();

    void setBroadPhaseWorldSize(hkReal size);
    void setupSolverInfo(int solverType);
};
