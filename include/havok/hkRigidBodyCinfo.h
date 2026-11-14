#pragma once

#include <havok/hkVector4.h>
#include <havok/hkQuaternion.h>
#include <havok/hkMatrix3.h>
#include <havok/hkMotion.h>
#include <havok/hkMaterial.h>

struct hkShape;

// Construction info of a rigid body (0xC0 bytes). HYPOTHESIS: the names (the image has no reflection for it; the
// offsets come from its constructor and hkRigidBody's).
struct hkRigidBodyCinfo {
    // HYPOTHESIS: the names of the values (only 2 is used here, as the default).
    enum SolverDeactivation {
        SOLVER_DEACTIVATION_INVALID = 0,
        SOLVER_DEACTIVATION_OFF = 1,
        SOLVER_DEACTIVATION_LOW = 2,
    };
    // Collision quality of the body; INVALID picks it from the motion type. HYPOTHESIS: the names.
    enum QualityType {
        QUALITY_INVALID = 0,
        QUALITY_FIXED = 1,
        QUALITY_KEYFRAMED = 2,
        QUALITY_DEBRIS = 3,
    };

    u32 m_collisionFilterInfo;         // 0x00
    hkShape* m_shape;                  // 0x04
    hkEnum<hkMaterial::ResponseType, s8> m_collisionResponse; // 0x08
    u16 m_processContactCallbackDelay; // 0x0A
    hkVector4 m_position;              // 0x10
    hkQuaternion m_rotation;           // 0x20
    hkVector4 m_linearVelocity;        // 0x30
    hkVector4 m_angularVelocity;       // 0x40
    hkMatrix3 m_inertiaTensor;         // 0x50
    hkVector4 m_centerOfMass;          // 0x80
    hkReal m_mass;                     // 0x90
    hkReal m_linearDamping;            // 0x94
    hkReal m_angularDamping;           // 0x98
    hkReal m_friction;                 // 0x9C
    hkReal m_restitution;              // 0xA0
    hkReal m_maxLinearVelocity;        // 0xA4
    hkReal m_maxAngularVelocity;       // 0xA8
    hkReal m_allowedPenetrationDepth;  // 0xAC (<= 0: estimated from the shape)
    hkEnum<hkMotion::MotionType, s8> m_motionType; // 0xB0
    s8 m_rigidBodyDeactivatorType;     // 0xB1
    hkEnum<SolverDeactivation, s8> m_solverDeactivation; // 0xB2
    hkEnum<QualityType, s8> m_qualityType; // 0xB3
    s8 m_autoRemoveLevel;              // 0xB4

    hkRigidBodyCinfo();
};
