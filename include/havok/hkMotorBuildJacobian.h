#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>

// Write cursor used while the motor Jacobian is committed: element pointer and schema pointer.
struct hkMotorJacobianCursor {
    u8* m_elem;   // 0x00 HYPOTHESIS: name
    u32 unk04;    // 0x04
    u8* m_schema; // 0x08 HYPOTHESIS: name
};

// Motor Jacobian builder (the motor data is read through this pointer).
struct hkMotorBuildJacobian {
    float unk00; // 0x00
    float unk04; // 0x04
    float unk08; // 0x08
    float unk0C; // 0x0C
    float unk10; // 0x10
    float unk14; // 0x14

    void hk1dLinearVelocityMotorCommitJacobian(const hkVector4* v, hkMotorJacobianCursor* c);
};
