#pragma once

#include <types.h>

class phIkTwoJointSolver;

// IK controller (map class phIkController). Offsets from src/sora/ph/ph_ik_controller.cpp.
class phIkController {
public:
    void setEnable(int idx, bool enable);
    void setStressReferencePoint(int idx, const f32* pos);
    void resetToeAngle(int idx);
    void setSolverParamData(int idx, const f32* data);

    u8 unk00;                    // 0x00, set to 1 by the constructor (HYPOTHESIS: enable flag)
    void* unk04;                 // 0x04, allocated by the constructor
    phIkTwoJointSolver** m_solvers; // 0x08, array of solver pointers
    s32 m_solverCount;           // 0x0C
    u32 m_solverFlags;           // 0x10, capacity with the 0x80000000 ownership bit
    void* unk14;                 // 0x14, array of 8-byte entries
    s32 unk18;                   // 0x18
    u32 unk1C;                   // 0x1C, flags with 0x80000000 set by the constructor
};
