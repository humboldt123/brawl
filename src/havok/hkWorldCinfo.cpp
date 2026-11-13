// Havok translation unit hkWorldCinfo.o (main.dol 0x802F004C-0x802F03A8).
// Functions in address order:
//   0x802F004C    52  finishLoadedObjecthkWorldCinfo   [map: hkWorldCinfo__finishLoadedObjecthkWorldCinfo]
//   0x802F0080    20  cleanupLoadedObjecthkWorldCinfo   [map: hkWorldCinfo__cleanupLoadedObjecthkWorldCinfo]
//   0x802F0094    72  getVtablehkWorldCinfo   [map: hkWorldCinfo__getVtablehkWorldCinfo]
//   0x802F00DC   240  __ct   [map: hkWorldCinfo____ct]
//   0x802F01CC    52  setBroadPhaseWorldSize   [map: hkWorldCinfo__setBroadPhaseWorldSize]
//   0x802F0200   284  setupSolverInfo   [map: hkWorldCinfo__setupSolverInfo]
//   0x802F031C    60  __ct   [map: hkWorldCinfo____ct1]
//   0x802F0358    80  __sinit_\hkWorldCinfo_cpp   [map: hkWorldCinfocpp____sinit_]

#include <havok/hkWorldCinfo.h>
#include <havok/hkRegistry.h>
#include <havok/hkCdPoint.h> // HK_REAL_MAX

void hkWorldCinfo::finishLoadedObjecthkWorldCinfo(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkWorldCinfo(flag);
}

void hkWorldCinfo::cleanupLoadedObjecthkWorldCinfo(void* p) {
    ((hkWorldCinfo*)p)->~hkWorldCinfo();
}

#pragma auto_inline off
const void* hkWorldCinfo::getVtablehkWorldCinfo() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    // Temporary object on the stack (16-byte aligned like hkWorldCinfo); only its vtable pointer is read back.
    __attribute__((aligned(16))) char buf[0xA0];
    ::new (buf) hkWorldCinfo(flag);
    return *(const void**)buf;
}

#pragma auto_inline reset

hkWorldCinfo::hkWorldCinfo() {
    // Default values from the literals the target loads (objdiff does not compare literal values; checked with a
    // literal comparison of the relocation targets).
    m_gravity.set(0.0f, -9.8f, 0.0f, 0.0f);
    m_enableSimulationIslands = true;
    m_broadPhaseQuerySize = 1024;
    m_broadPhaseWorldAabb.m_min.set(-500.0f, -500.0f, -500.0f, 0.0f);
    m_broadPhaseWorldAabb.m_max.set(500.0f, 500.0f, 500.0f, 0.0f);
    m_collisionFilter = 0;
    m_broadPhaseNumMarkers = 0;
    m_solverTau = 0.6f;
    m_solverDamp = 1.0f;
    m_contactRestingVelocity = HK_REAL_MAX;
    m_solverIterations = 4;
    m_solverMicrosteps = 1;
    m_collisionTolerance = 0.1f;
    m_broadPhaseBorderBehaviour = BROADPHASE_BORDER_ASSERT;
    m_toiCollisionResponseRotateNormal = 0.2f;
    m_expectedMaxLinearVelocity = 200.0f;
    m_expectedMinPsiDeltaTime = 1.0f / 30.0f;
    m_iterativeLinearCastEarlyOutDistance = 0.01f;
    m_iterativeLinearCastMaxIterations = 20;
    m_enableDeactivation = true;
    m_shouldActivateOnRigidBodyTransformChange = true;
    m_highFrequencyDeactivationPeriod = 0.1f;
    m_lowFrequencyDeactivationPeriod = 1.0f;
    m_contactPointGeneration = CONTACT_POINT_REJECT_MANY;
    m_simulationType = SIMULATION_TYPE_CONTINUOUS;
    m_frameMarkerPsiSnap = 0.0001f;
    m_memoryWatchDog = 0;
}

void hkWorldCinfo::setBroadPhaseWorldSize(hkReal size) {
    // HYPOTHESIS: the aabb is +/- size/2 (scale constants not verified).
    m_broadPhaseWorldAabb.m_min.set(-0.5f * size, -0.5f * size, -0.5f * size, -0.5f * size);
    m_broadPhaseWorldAabb.m_max.set(0.5f * size, 0.5f * size, 0.5f * size, 0.5f * size);
}

void hkWorldCinfo::setupSolverInfo(int solverType) {
    // tau/damp per stiffness from the target's literals (soft 0.3/0.9, medium 0.6/1.0, hard 0.9/1.1)
    switch (solverType) {
    case SOLVER_TYPE_2ITERS_SOFT:
        m_solverTau = 0.3f;
        m_solverDamp = 0.9f;
        m_solverIterations = 2;
        break;
    case SOLVER_TYPE_2ITERS_MEDIUM:
        m_solverTau = 0.6f;
        m_solverDamp = 1.0f;
        m_solverIterations = 2;
        break;
    case SOLVER_TYPE_2ITERS_HARD:
        m_solverTau = 0.9f;
        m_solverDamp = 1.1f;
        m_solverIterations = 2;
        break;
    case SOLVER_TYPE_4ITERS_SOFT:
        m_solverTau = 0.3f;
        m_solverDamp = 0.9f;
        m_solverIterations = 4;
        break;
    case SOLVER_TYPE_4ITERS_MEDIUM:
        m_solverTau = 0.6f;
        m_solverDamp = 1.0f;
        m_solverIterations = 4;
        break;
    case SOLVER_TYPE_4ITERS_HARD:
        m_solverTau = 0.9f;
        m_solverDamp = 1.1f;
        m_solverIterations = 4;
        break;
    case SOLVER_TYPE_8ITERS_SOFT:
        m_solverTau = 0.3f;
        m_solverDamp = 0.9f;
        m_solverIterations = 8;
        break;
    case SOLVER_TYPE_8ITERS_MEDIUM:
        m_solverTau = 0.6f;
        m_solverDamp = 1.0f;
        m_solverIterations = 8;
        break;
    case SOLVER_TYPE_8ITERS_HARD:
        m_solverTau = 0.9f;
        m_solverDamp = 1.1f;
        m_solverIterations = 8;
        break;
    }
}

#pragma auto_inline off
hkWorldCinfo::hkWorldCinfo(hkFinishLoadedObjectFlag flag) {
    if (flag.m_finishing != 0) {
        if (m_contactRestingVelocity == 0.0f) {
            m_contactRestingVelocity = HK_REAL_MAX;
        }
    }
}

#pragma auto_inline reset

static hkStaticTypeInfo hkWorldCinfoTypeInfo("hkWorldCinfo", hkWorldCinfo::finishLoadedObjecthkWorldCinfo, hkWorldCinfo::cleanupLoadedObjecthkWorldCinfo,
                                      hkWorldCinfo::getVtablehkWorldCinfo());
