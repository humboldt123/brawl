// Havok translation unit hkConstraintSolverSetup.o (main.dol 0x802DF6E4-0x802E0E40).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802DF6E4    28  integrate   [map: hkConstraintSolverSetup__integrate]
//   0x802DF700   192  oneStepIntegrate   [map: hkConstraintSolverSetup__oneStepIntegrate]
//   0x802DF7C0  2528  solve   [map: hkConstraintSolverSetup__solve]
//   0x802E01A0   356  initializeSolverState   [map: hkConstraintSolverSetup__initializeSolverState]
//   0x802E0304     4  shutdownSolver   [map: hkConstraintSolverSetup__shutdownSolver]
//   0x802E0308   916  internalAddAccumulators   [map: hkConstraintSolverSetup__internalAddAccumulators]
//   0x802E069C   828  internalAddJacobianElements   [map: hkConstraintSolverSetup__internalAddJacobianElements]
//   0x802E09D8    60  internalIsMemoryOkForNewAccumulators   [map: hkConstraintSolverSetup__internalIsMemoryOkForNewAccumulators]
//   0x802E0A14   880  internalIsMemoryOkForNewJacobianElements   [map: hkConstraintSolverSetup__internalIsMemoryOkForNewJacobianElements]
//   0x802E0D84   188  subSolve   [map: hkConstraintSolverSetup__subSolve]

#include <havok/hkConstraintSolverSetup.h>
#include <havok/hkRigidMotionUtil.h>
#include <havok/hkEntity.h>

void hkConstraintSolverSetup::integrate(void* a, void* b, void* c) {
    hkRigidMotionUtil::hkRigidMotionUtilApplyAccumulators(this, (u8*)c + 0x80, a, (u32)b, 0xa0, a);
}

void hkConstraintSolverSetup::oneStepIntegrate(hkEntity** entities, int count, u8* base) {
    for (int i = 0; i < count; i++) {
        hkEntity* entity = entities[i];
        u8* state = base + entity->m_solverData;
        float* src = (float*)(state + 0x10);
        float* dst = (float*)(state + 0x40);
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        dst[6] = src[6];
        dst[7] = src[7];
        void* record[2];
        record[0] = (u8*)entity + 0xa0;
        hkRigidMotionUtil::hkRigidMotionUtilApplyAccumulators(this, entity, record, 1, 0, (u8*)entity + 0xa0);
    }
}

void hkConstraintSolverSetup::shutdownSolver() {}
