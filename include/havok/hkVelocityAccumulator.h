#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Velocity accumulator of the solver (0x80 bytes: hkRigidMotionUtilApplyAccumulators walks them with that stride).
// The 0x10 and 0x20 vectors are the velocities the motions get back after the step, 0x30 the angular multiplier
// used by the angular impulse code, 0x40 / 0x50 the summed velocities the step uses.
// HYPOTHESIS: the names of m_linearVel / m_angularVel (from the write-back into hkMotion's velocities).
struct hkVelocityAccumulator {
    u8 unk00[4];           // 0x00
    u32 unk4;              // 0x04 handed to hkMotion::setDeactivationCounter
    u8 unk08[8];           // 0x08
    hkVector4 m_linearVel;  // 0x10
    hkVector4 m_angularVel; // 0x20
    hkVector4 unk30;        // 0x30 HYPOTHESIS: name
    hkVector4 unk40;        // 0x40
    hkVector4 unk50;        // 0x50
    u8 unk60[0x20];         // 0x60

    hkVector4* getSumLinearVel();
    hkVector4* getSumAngularVel(); // defined in src/havok/hkRigidMotionUtil.cpp (returns this + 0x50)
};
