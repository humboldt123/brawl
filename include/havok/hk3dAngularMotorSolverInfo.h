#pragma once

#include <havok/hkBase.h>

// 3d angular motor solver info: a byte of packed 2-bit states.
struct hk3dAngularMotorSolverInfo {
    u8 m_states; // 0x00 HYPOTHESIS: name

    int getState(int index) const;
};
