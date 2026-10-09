// Havok translation unit hkMotorBuildJacobian.o (main.dol 0x802951A0-0x80295788).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802951A0   468  hkCalcMotorData   [map: hkMotorBuildJacobian__hkCalcMotorData]
//   0x80295374   928  hk1dLinearVelocityMotorBeginJacobian   [map: hkMotorBuildJacobian__hk1dLinearVelocityMotorBeginJacobian]
//   0x80295714   116  hk1dLinearVelocityMotorCommitJacobian   [map: hkMotorBuildJacobian__hk1dLinearVelocityMotorCommitJacobian]

#include <havok/hkMotorBuildJacobian.h>

void hkMotorBuildJacobian::hk1dLinearVelocityMotorCommitJacobian(const hkVector4* v, hkMotorJacobianCursor* c) {
    u8* elem = c->m_elem;
    float* sch = (float*)c->m_schema;
    float vy = v->y;
    sch[2] = unk08 * vy;
    sch[3] = unk0C * vy;
    sch[4] = unk04;
    sch[1] = 0.0f; // HYPOTHESIS: constant value
    sch[5] = unk10;
    sch[6] = unk14;
    *(u32*)sch = 0x0609001C;
    float vz = v->z;
    ((float*)elem)[3] = unk00 * vz;
    c->m_elem = elem + 0x30;
    c->m_schema = (u8*)sch + 0x1c;
}
