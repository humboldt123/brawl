// Havok translation unit hkMotion.o (main.dol 0x802E4338-0x802E46B0).
#pragma fp_contract on
#include <havok/hkMotion.h>
#include <havok/hkTransform.h>
#include <havok/hkSweptTransformUtil.h>

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
    hkSweptTransformUtil::setCentreOfRotationLocal(com, m_motionState);
}

void hkMotion::setPosition(const hkVector4& position) {
    hkSweptTransformUtil::warpToPosition(position, m_motionState);
}

void hkMotion::setRotation(const hkQuaternion& rotation) {
    hkSweptTransformUtil::warpToRotation(rotation, m_motionState);
}

void hkMotion::setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation) {
    hkSweptTransformUtil::warpTo(position, rotation, m_motionState);
}

void hkMotion::setTransform(const hkTransform& transform) {
    hkSweptTransformUtil::warpTo(transform, m_motionState);
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

// HYPOTHESIS: output type; preserve the observed motion-state and velocity range.
// MATCH-ONLY: native scalar loads/stores alternate; disable scheduling locally
// instead of making ordinary motion-state storage volatile.
#pragma scheduling off
void hkMotion::getMotionStateAndVelocities(hkMotion* out) const {
    out->m_motionState.m_transform.m_rotation.m_col0.x = m_motionState.m_transform.m_rotation.m_col0.x;
    out->m_motionState.m_transform.m_rotation.m_col0.y = m_motionState.m_transform.m_rotation.m_col0.y;
    out->m_motionState.m_transform.m_rotation.m_col0.z = m_motionState.m_transform.m_rotation.m_col0.z;
    out->m_motionState.m_transform.m_rotation.m_col0.w = m_motionState.m_transform.m_rotation.m_col0.w;
    out->m_motionState.m_transform.m_rotation.m_col1.x = m_motionState.m_transform.m_rotation.m_col1.x;
    out->m_motionState.m_transform.m_rotation.m_col1.y = m_motionState.m_transform.m_rotation.m_col1.y;
    out->m_motionState.m_transform.m_rotation.m_col1.z = m_motionState.m_transform.m_rotation.m_col1.z;
    out->m_motionState.m_transform.m_rotation.m_col1.w = m_motionState.m_transform.m_rotation.m_col1.w;
    out->m_motionState.m_transform.m_rotation.m_col2.x = m_motionState.m_transform.m_rotation.m_col2.x;
    out->m_motionState.m_transform.m_rotation.m_col2.y = m_motionState.m_transform.m_rotation.m_col2.y;
    out->m_motionState.m_transform.m_rotation.m_col2.z = m_motionState.m_transform.m_rotation.m_col2.z;
    out->m_motionState.m_transform.m_rotation.m_col2.w = m_motionState.m_transform.m_rotation.m_col2.w;
    out->m_motionState.m_transform.m_translation.x = m_motionState.m_transform.m_translation.x;
    out->m_motionState.m_transform.m_translation.y = m_motionState.m_transform.m_translation.y;
    out->m_motionState.m_transform.m_translation.z = m_motionState.m_transform.m_translation.z;
    out->m_motionState.m_transform.m_translation.w = m_motionState.m_transform.m_translation.w;
    out->m_motionState.m_sweptTransform.m_centerOfMass0.x = m_motionState.m_sweptTransform.m_centerOfMass0.x;
    out->m_motionState.m_sweptTransform.m_centerOfMass0.y = m_motionState.m_sweptTransform.m_centerOfMass0.y;
    out->m_motionState.m_sweptTransform.m_centerOfMass0.z = m_motionState.m_sweptTransform.m_centerOfMass0.z;
    out->m_motionState.m_sweptTransform.m_centerOfMass0.w = m_motionState.m_sweptTransform.m_centerOfMass0.w;
    out->m_motionState.m_sweptTransform.m_centerOfMass1.x = m_motionState.m_sweptTransform.m_centerOfMass1.x;
    out->m_motionState.m_sweptTransform.m_centerOfMass1.y = m_motionState.m_sweptTransform.m_centerOfMass1.y;
    out->m_motionState.m_sweptTransform.m_centerOfMass1.z = m_motionState.m_sweptTransform.m_centerOfMass1.z;
    out->m_motionState.m_sweptTransform.m_centerOfMass1.w = m_motionState.m_sweptTransform.m_centerOfMass1.w;
    out->m_motionState.m_sweptTransform.m_rotation0.m_vec.x = m_motionState.m_sweptTransform.m_rotation0.m_vec.x;
    out->m_motionState.m_sweptTransform.m_rotation0.m_vec.y = m_motionState.m_sweptTransform.m_rotation0.m_vec.y;
    out->m_motionState.m_sweptTransform.m_rotation0.m_vec.z = m_motionState.m_sweptTransform.m_rotation0.m_vec.z;
    out->m_motionState.m_sweptTransform.m_rotation0.m_vec.w = m_motionState.m_sweptTransform.m_rotation0.m_vec.w;
    out->m_motionState.m_sweptTransform.m_rotation1.m_vec.x = m_motionState.m_sweptTransform.m_rotation1.m_vec.x;
    out->m_motionState.m_sweptTransform.m_rotation1.m_vec.y = m_motionState.m_sweptTransform.m_rotation1.m_vec.y;
    out->m_motionState.m_sweptTransform.m_rotation1.m_vec.z = m_motionState.m_sweptTransform.m_rotation1.m_vec.z;
    out->m_motionState.m_sweptTransform.m_rotation1.m_vec.w = m_motionState.m_sweptTransform.m_rotation1.m_vec.w;
    out->m_motionState.m_sweptTransform.m_centerOfMassLocal.x = m_motionState.m_sweptTransform.m_centerOfMassLocal.x;
    out->m_motionState.m_sweptTransform.m_centerOfMassLocal.y = m_motionState.m_sweptTransform.m_centerOfMassLocal.y;
    out->m_motionState.m_sweptTransform.m_centerOfMassLocal.z = m_motionState.m_sweptTransform.m_centerOfMassLocal.z;
    out->m_motionState.m_sweptTransform.m_centerOfMassLocal.w = m_motionState.m_sweptTransform.m_centerOfMassLocal.w;
    out->m_motionState.m_deltaAngle.x = m_motionState.m_deltaAngle.x;
    out->m_motionState.m_deltaAngle.y = m_motionState.m_deltaAngle.y;
    out->m_motionState.m_deltaAngle.z = m_motionState.m_deltaAngle.z;
    out->m_motionState.m_deltaAngle.w = m_motionState.m_deltaAngle.w;
    out->m_motionState.m_objectRadius = m_motionState.m_objectRadius;
    out->m_motionState.m_maxLinearVelocity = m_motionState.m_maxLinearVelocity;
    out->m_motionState.m_maxAngularVelocity = m_motionState.m_maxAngularVelocity;
    out->m_motionState.m_linearDamping = m_motionState.m_linearDamping;
    out->m_motionState.m_angularDamping = m_motionState.m_angularDamping;
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

#pragma scheduling reset

void hkMotion::setDeactivationClass(u16 deactivationClass) {
    m_motionState.m_deactivationClass = deactivationClass;
}
