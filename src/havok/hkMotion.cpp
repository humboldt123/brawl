// Havok translation unit hkMotion.o (main.dol 0x802E4338-0x802E46B0).
#pragma fp_contract on
#include <havok/hkMotion.h>
#include <havok/hkTransform.h>

// Warps and transforms of the swept transform (hkSweptTransformUtil, another TU).
extern "C" void fn_802870F8(const hkVector4& v, hkMotionState& state);
extern "C" void fn_80286F98(const hkVector4& v, hkMotionState& state);
extern "C" void fn_802870E4(const hkQuaternion& q, hkMotionState& state);
extern "C" void fn_80286C20(const hkVector4& v, const hkQuaternion& q, hkMotionState& state);
extern "C" void fn_80286D74(const hkTransform& t, hkMotionState& state);

hkMotion::hkMotion(const hkVector4& position, const hkQuaternion& rotation) {
    // MATCH-ONLY: the zero stores run from w back to x.
    m_linearVelocity.w = 0.0f;
    m_linearVelocity.z = 0.0f;
    m_linearVelocity.y = 0.0f;
    m_linearVelocity.x = 0.0f;
    m_angularVelocity.w = 0.0f;
    m_angularVelocity.z = 0.0f;
    m_angularVelocity.y = 0.0f;
    m_angularVelocity.x = 0.0f;
    m_motionState.initMotionState(position, rotation);
    m_type = MOTION_INVALID;
    m_motionState.m_linearDamping = 0.0f;
    m_motionState.m_angularDamping = 0.0f;
}

// Mass is stored as its inverse; zero mass gives zero inverse mass.
void hkMotion::setMass(hkReal mass) {
    hkReal massInv;
    if (0.0f == mass) {
        massInv = 0.0f;
    } else {
        massInv = 1.0f / mass;
    }
    setMassInv(massInv);
}

hkReal hkMotion::getMass() const {
    hkReal massInv = m_inertiaAndMassInv.w;
    if (massInv == 0.0f) {
        return 0.0f;
    }
    return 1.0f / massInv;
}

void hkMotion::setMassInv(hkReal massInv) {
    m_inertiaAndMassInv.w = massInv;
}

void hkMotion::setCenterOfMassInLocal(const hkVector4& com) {
    fn_802870F8(com, m_motionState);
}

void hkMotion::setPosition(const hkVector4& position) {
    fn_80286F98(position, m_motionState);
}

void hkMotion::setRotation(const hkQuaternion& rotation) {
    fn_802870E4(rotation, m_motionState);
}

void hkMotion::setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation) {
    fn_80286C20(position, rotation, m_motionState);
}

void hkMotion::setTransform(const hkTransform& transform) {
    fn_80286D74(transform, m_motionState);
}

void hkMotion::setLinearVelocity(const hkVector4& v) {
    m_linearVelocity = v;
}

void hkMotion::setAngularVelocity(const hkVector4& v) {
    m_angularVelocity = v;
}

// Velocity change: v += inverse mass * impulse (all four lanes, as the original).
void hkMotion::applyLinearImpulse(const hkVector4& impulse) {
    hkReal massInv = m_inertiaAndMassInv.w;
    m_linearVelocity.x = massInv * impulse.x + m_linearVelocity.x;
    m_linearVelocity.y = massInv * impulse.y + m_linearVelocity.y;
    m_linearVelocity.z = massInv * impulse.z + m_linearVelocity.z;
    m_linearVelocity.w = massInv * impulse.w + m_linearVelocity.w;
}

// HYPOTHESIS: out type. MATCH-ONLY: the motion state is copied as 45 individual float words
// (0x00-0xB0 of hkMotionState), then the two halfword counters, then both velocities.
void hkMotion::getMotionStateAndVelocities(hkMotion* out) const {
    // MATCH-ONLY: volatile keeps each word copy as a load/store pair in order.
    const volatile hkReal* s = (const volatile hkReal*)&m_motionState;
    volatile hkReal* d = (volatile hkReal*)&out->m_motionState;
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
    d[4] = s[4];
    d[5] = s[5];
    d[6] = s[6];
    d[7] = s[7];
    d[8] = s[8];
    d[9] = s[9];
    d[10] = s[10];
    d[11] = s[11];
    d[12] = s[12];
    d[13] = s[13];
    d[14] = s[14];
    d[15] = s[15];
    d[16] = s[16];
    d[17] = s[17];
    d[18] = s[18];
    d[19] = s[19];
    d[20] = s[20];
    d[21] = s[21];
    d[22] = s[22];
    d[23] = s[23];
    d[24] = s[24];
    d[25] = s[25];
    d[26] = s[26];
    d[27] = s[27];
    d[28] = s[28];
    d[29] = s[29];
    d[30] = s[30];
    d[31] = s[31];
    d[32] = s[32];
    d[33] = s[33];
    d[34] = s[34];
    d[35] = s[35];
    d[36] = s[36];
    d[37] = s[37];
    d[38] = s[38];
    d[39] = s[39];
    d[40] = s[40];
    d[41] = s[41];
    d[42] = s[42];
    d[43] = s[43];
    d[44] = s[44];
    out->m_motionState.m_deactivationClass = m_motionState.m_deactivationClass;
    out->m_motionState.m_deactivationCounter = m_motionState.m_deactivationCounter;
    out->m_linearVelocity.x = m_linearVelocity.x;
    out->m_linearVelocity.y = m_linearVelocity.y;
    out->m_linearVelocity.z = m_linearVelocity.z;
    out->m_linearVelocity.w = m_linearVelocity.w;
    out->m_angularVelocity.x = m_angularVelocity.x;
    out->m_angularVelocity.y = m_angularVelocity.y;
    out->m_angularVelocity.z = m_angularVelocity.z;
    out->m_angularVelocity.w = m_angularVelocity.w;
}

void hkMotion::setDeactivationClass(u16 deactivationClass) {
    m_motionState.m_deactivationClass = deactivationClass;
}
