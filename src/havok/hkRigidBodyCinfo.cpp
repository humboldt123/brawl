// Havok translation unit hkRigidBodyCinfo.o (main.dol 0x802E3604-0x802E3724).
//   0x802E3604   288  hkRigidBodyCinfo::hkRigidBodyCinfo
#include <havok/hkRigidBodyCinfo.h>

// A dynamic body with unit mass and inertia at the origin; the deactivator and solver deactivation types are 2.
hkRigidBodyCinfo::hkRigidBodyCinfo() {
    m_position.setZero4();
    m_rotation.setIdentity();
    m_linearVelocity.setZero4();
    m_angularVelocity.setZero4();
    m_inertiaTensor.setDiagonal(1.0f, 1.0f, 1.0f);
    m_centerOfMass.setZero4();
    m_mass = 1.0f;
    m_linearDamping = 0.0f;
    m_angularDamping = 0.05f;
    m_friction = 0.5f;
    m_restitution = 0.4f;
    m_maxLinearVelocity = 200.0f;
    m_maxAngularVelocity = 200.0f;
    m_allowedPenetrationDepth = -1.0f;
    m_motionType = hkMotion::MOTION_DYNAMIC;
    m_rigidBodyDeactivatorType = 2;
    m_solverDeactivation = SOLVER_DEACTIVATION_LOW;
    m_qualityType = QUALITY_INVALID;
    m_collisionResponse = hkMaterial::RESPONSE_SIMPLE_CONTACT;
    m_processContactCallbackDelay = 0xFFFF;
    m_collisionFilterInfo = 0;
    m_shape = NULL;
    m_autoRemoveLevel = 0;
}
