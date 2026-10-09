#pragma once

#include <types.h>

// Two-joint IK solver (map class phIkTwoJointSolver). Offsets come from the asm of
// src/sora/ph/ph_ik_two_joint_solver.cpp. Names are HYPOTHESIS; unkNN marks fields not yet seen in the asm.
class phIkTwoJointSolver {
public:
    // Setup block (map class phIkTwoJointSolver::SetupInfo), filled by init().
    class SetupInfo {
    public:
        void init();

        s32 unk00;      // 0x00, set to -1
        s16 unk04[4];   // 0x04, set to -1
        f32 unk0C;      // 0x0C, not written by init
        f32 unk10[4];   // 0x10
        f32 unk20[4];   // 0x20
        f32 unk30[4];   // 0x30, table values copied from a static 4-float table
        f32 unk40[4];   // 0x40, same table values
        f32 unk50[2];   // 0x50, pi
        f32 unk58[2];   // 0x58, 5.0
    };

    void init();

    void setSolverParamData(const f32* param);
    bool isHaveNodeFromhkIndex(int hkIndex) const;
    void savedParam();
    void applyParam();

    u8 unk00;          // 0x00
    u8 m_enable;       // 0x01, written by phIkController::setEnable
    u8 unk02[0x12];    // 0x02
    s16 m_hkIndex[4];  // 0x14, Havok node indices (HYPOTHESIS: the four nodes this solver drives)
    u8 unk1C[0x14];    // 0x1C
    f32 m_cosAngle0;   // 0x30, cos of param[0] in degrees
    f32 m_cosAngle1;   // 0x34, cos of param[1] in degrees
    f32 unk38;         // 0x38, set to 1.0 by setSolverParamData
    f32 unk3C;         // 0x3C, set to 1.0 by setSolverParamData
    u8 unk40[0x20];    // 0x40
    f32 m_param[2];    // 0x60, param[6..7] scaled from degrees to radians
    f32 m_param68[2];  // 0x68, param[4..5]
    u8 unk70[0x50];    // 0x70
    f32 m_savedParam[2]; // 0xC0, copy of m_param (see savedParam/applyParam)
    u8 unkC8[0x18];    // 0xC8
    f32 m_stressRef[3]; // 0xE0, stress reference point written by phIkController::setStressReferencePoint
    u8 unkEC[0x14];    // 0xEC
    f32 m_vec100[4];   // 0x100, written by updateInsideStepDir (not yet decompiled)
};
