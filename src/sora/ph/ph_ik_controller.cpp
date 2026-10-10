// Brawl physics wrapper translation unit ph_ik_controller.o (main.dol 0x800966EC-0x800985D8).
// Not yet decompiled. Functions in address order with their map names:
//   0x800966EC   136  __ct   [map: phIkController____ct]
//   0x80096774   600  __dt   [map: phIkController____dt]
//   0x800969CC    20  setEnable   [map: phIkController__setEnable]
//   0x800969E0  1488  calacCallBackTimingCForIKCharacter   [map: phIkController__calacCallBackTimingCForIKCharacter]
//   0x80096FB0  2112  createIKSolverInit   [map: phIkController__createIKSolverInit]
//   0x800977F0  1188  updatePose   [map: phIkController__updatePose]
//   0x80097C94   512  setHeelPoint   [map: phIkController__setHeelPoint]
//   0x80097E94   512  setToePoint   [map: phIkController__setToePoint]
//   0x80098094   204  setNormal   [map: phIkController__setNormal]
//   0x80098160    40  setStressReferencePoint   [map: phIkController__setStressReferencePoint]
//   0x80098188   512  setGain   [map: phIkController__setGain]
//   0x80098388   496  setToeAngle   [map: phIkController__setToeAngle]
//   0x80098578    16  resetToeAngle   [map: phIkController__resetToeAngle]
//   0x80098588    80  setSolverParamData   [map: phIkController__setSolverParamData]
#include <ph/phIkController.h>
#include <ph/phIkTwoJointSolver.h>

void phIkController::setEnable(int idx, bool enable) {
    m_solvers[idx]->m_enable = enable;
}

void phIkController::setStressReferencePoint(int idx, const f32* pos) {
    phIkTwoJointSolver* solver = m_solvers[idx];
    solver->m_stressRef[0] = pos[0];
    solver->m_stressRef[1] = pos[1];
    solver->m_stressRef[2] = pos[2];
}

void phIkController::resetToeAngle(int idx) {
    m_solvers[idx]->applyParam();
}

void phIkController::setSolverParamData(int idx, const f32* data) {
    m_solvers[idx]->setSolverParamData(data);
    m_solvers[idx]->savedParam();
}
