// Brawl physics wrapper translation unit ph_ik_two_joint_solver.o (main.dol 0x800985D8-0x80099978).
// Not yet decompiled. Functions in address order with their map names:
//   0x800985D8   208  init   [map: phIkTwoJointSolver9SetupInfoFv__init]
//   0x800986A8     4  init   [map: phIkTwoJointSolver__init]
//   0x800986AC   576  update   [map: phIkTwoJointSolver__update]
//   0x800988EC  1320  updateToeInsideCurve   [map: phIkTwoJointSolver__updateToeInsideCurve]
//   0x80098E14  2336  updateToeCurve   [map: phIkTwoJointSolver__updateToeCurve]
//   0x80099734   156  setSolverParamData   [map: phIkTwoJointSolver__setSolverParamData]
//   0x800997D0    64  isHaveNodeFromhkIndex   [map: phIkTwoJointSolver__isHaveNodeFromhkIndex]
//   0x80099810    20  savedParam   [map: phIkTwoJointSolver__savedParam]
//   0x80099824    20  applyParam   [map: phIkTwoJointSolver__applyParam]
//   0x80099838   320  updateInsideStepDir   [map: phIkTwoJointSolver__updateInsideStepDir]
#include <math.h>
#include <ph/phIkTwoJointSolver.h>

void phIkTwoJointSolver::init() {}

void phIkTwoJointSolver::setSolverParamData(const f32* param) {
    f64 cosVal = cos(0.017453292f * param[0]);
    f32 p1 = param[1];
    m_cosAngle0 = cosVal;
    cosVal = cos(0.017453292f * p1);
    m_cosAngle1 = cosVal;
    unk38 = 1.0f;
    unk3C = 1.0f;
    m_param68[0] = param[4];
    m_param68[1] = param[5];
    m_param[0] = 0.017453292f * param[6];
    m_param[1] = 0.017453292f * param[7];
}

bool phIkTwoJointSolver::isHaveNodeFromhkIndex(int hkIndex) const {
    if (hkIndex == m_hkIndex[0] || hkIndex == m_hkIndex[1] || hkIndex == m_hkIndex[2] || hkIndex == m_hkIndex[3]) {
        return true;
    }
    return false;
}

void phIkTwoJointSolver::savedParam() {
    m_savedParam[0] = m_param[0];
    m_savedParam[1] = m_param[1];
}

void phIkTwoJointSolver::applyParam() {
    m_param[0] = m_savedParam[0];
    m_param[1] = m_savedParam[1];
}
