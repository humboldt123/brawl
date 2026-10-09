#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Velocity accumulator (only the members used so far). The 0x20 and 0x30 vectors are the velocity and the
// angular multiplier used by the angular impulse code; 0x40 is the summed linear velocity.
struct hkVelocityAccumulator {
    u8 unk00[0x20];
    hkVector4 unk20; // 0x20 HYPOTHESIS: name
    hkVector4 unk30; // 0x30 HYPOTHESIS: name
    hkVector4 unk40; // 0x40

    hkVector4* getSumLinearVel();
    hkVector4* getSumAngularVel(); // defined in src/havok/hkRigidMotionUtil.cpp (returns this + 0x50)
};
