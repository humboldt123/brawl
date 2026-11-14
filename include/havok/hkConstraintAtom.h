#pragma once

#include <havok/hkBase.h>
#include <havok/hkTransform.h>
#include <havok/hkVector4.h>
#include <havok/hkContactPoint.h>

struct hkConstraintData;
struct hkConstraintInfo;
struct hkConstraintMotor;

// Constraint atoms. Layouts and member names come from main.dol's reflection (hkConstraintAtomClasses.cpp;
// tests/havok_layout checks them). Atoms are plain structs (no vtable); the type selects the atom behaviour.

// Per-body mass input of a simple collision response (0x50 bytes; the modifier callbacks scale the inverse inertia
// at 0x10 and the inverse mass at 0x40).
struct hkSimpleConstraintInfoInitInput {
    hkVector4 m_massRelPos;  // 0x00
    hkMatrix3 m_invInertia;  // 0x10
    hkReal m_invMass;        // 0x40
};

// Linear and angular velocity of one body during a simple collision response (0x20 bytes).
struct hkBodyVelocity {
    hkVector4 m_linear;  // 0x00
    hkVector4 m_angular; // 0x10

    hkBodyVelocity() {}
};

// Atom header shared by all constraint atoms (0x02 bytes).
struct hkConstraintAtom {
    enum AtomType {
        TYPE_INVALID = 0,
        TYPE_BRIDGE = 1,
        TYPE_SET_LOCAL_TRANSFORMS = 2,
        TYPE_SET_LOCAL_TRANSLATIONS = 3,
        TYPE_SET_LOCAL_ROTATIONS = 4,
        TYPE_BALL_SOCKET = 5,
        TYPE_STIFF_SPRING = 6,
        TYPE_LIN = 7,
        TYPE_LIN_SOFT = 8,
        TYPE_LIN_LIMIT = 9,
        TYPE_LIN_FRICTION = 10,
        TYPE_LIN_MOTOR = 11,
        TYPE_2D_ANG = 12,
        TYPE_ANG = 13,
        TYPE_ANG_LIMIT = 14,
        TYPE_TWIST_LIMIT = 15,
        TYPE_CONE_LIMIT = 16,
        TYPE_ANG_FRICTION = 17,
        TYPE_ANG_MOTOR = 18,
        TYPE_RAGDOLL_MOTOR = 19,
        TYPE_PULLEY = 20,
        TYPE_OVERWRITE_PIVOT = 21,
        TYPE_CONTACT = 22,
        TYPE_MODIFIER_SOFT_CONTACT = 23,
        TYPE_MODIFIER_MASS_CHANGER = 24,
        TYPE_MODIFIER_VISCOUS_SURFACE = 25,
        TYPE_MODIFIER_MOVING_SURFACE = 26,
        TYPE_MAX = 27
    };
    enum CallbackRequest {
        CALLBACK_REQUEST_NONE = 0,
        CALLBACK_REQUEST_CONTACT_POINT = 1,
        CALLBACK_REQUEST_SETUP_PPU_ONLY = 2
    };

    u16 m_type; // 0x00 (AtomType)

    AtomType getType() const {
        return (AtomType)m_type;
    }

    // HYPOTHESIS: name (getConstraintInfoUtil tests the type through an hkBool, cntlzw + extsb.).
    hkBool isContact() const {
        return m_type == TYPE_CONTACT;
    }
};

// Hook called while a bridge atom is being solved; the bridge stores a function pointer to its thunk.
typedef void (*hkBridgeBuildJacobianFunc)(hkConstraintData* data);

// Size 0x0C.
struct hkBridgeConstraintAtom : hkConstraintAtom {
    hkBridgeBuildJacobianFunc m_buildJacobianFunc; // 0x04
    hkConstraintData* m_constraintData;            // 0x08

    void init(hkConstraintData* data);
};

// The atoms of a constraint that is solved through its constraint data (0x0C bytes).
struct hkBridgeAtoms {
    hkBridgeConstraintAtom m_bridgeAtom; // 0x00
};

// Size 0x02.
struct hkBallSocketConstraintAtom : hkConstraintAtom {
};

// Size 0x08.
struct hkStiffSpringConstraintAtom : hkConstraintAtom {
    hkReal m_length; // 0x04
};

// Size 0x04.
struct hkOverwritePivotConstraintAtom : hkConstraintAtom {
    u8 m_copyToPivotBFromPivotA; // 0x02
};

// Size 0x04.
struct hkLinConstraintAtom : hkConstraintAtom {
    u8 m_axisIndex; // 0x02
};

// Size 0x0C.
struct hkLinLimitConstraintAtom : hkConstraintAtom {
    u8 m_axisIndex; // 0x02
    hkReal m_min;   // 0x04
    hkReal m_max;   // 0x08

    int numSolverResults() const;
};

// Size 0x08.
struct hkLinFrictionConstraintAtom : hkConstraintAtom {
    u8 m_isEnabled;            // 0x02
    u8 m_frictionAxis;         // 0x03
    hkReal m_maxFrictionForce; // 0x04

    int numSolverResults() const;
};

// Size 0x10.
struct hkLinMotorConstraintAtom : hkConstraintAtom {
    hkBool m_isEnabled;                 // 0x02
    u8 m_motorAxis;                     // 0x03
    s16 m_initializedOffset;            // 0x04
    s16 m_previousTargetPositionOffset; // 0x06
    hkReal m_targetPosition;            // 0x08
    hkConstraintMotor* m_motor;         // 0x0C

    int numSolverResults() const;
};

// Size 0x04.
struct hk2dAngConstraintAtom : hkConstraintAtom {
    u8 m_freeRotationAxis; // 0x02

    int numSolverResults() const;
};

// Size 0x04.
struct hkAngConstraintAtom : hkConstraintAtom {
    u8 m_firstConstrainedAxis; // 0x02
    u8 m_numConstrainedAxes;   // 0x03

    int numSolverResults() const;
};

// Size 0x10.
struct hkAngLimitConstraintAtom : hkConstraintAtom {
    u8 m_isEnabled;                  // 0x02
    u8 m_limitAxis;                  // 0x03
    hkReal m_minAngle;               // 0x04
    hkReal m_maxAngle;               // 0x08
    hkReal m_angularLimitsTauFactor; // 0x0C

    int numSolverResults() const;
};

// Size 0x14.
struct hkTwistLimitConstraintAtom : hkConstraintAtom {
    u8 m_isEnabled;                  // 0x02
    u8 m_twistAxis;                  // 0x03
    u8 m_refAxis;                    // 0x04
    hkReal m_minAngle;               // 0x08
    hkReal m_maxAngle;               // 0x0C
    hkReal m_angularLimitsTauFactor; // 0x10

    int numSolverResults() const;
};

// Size 0x14.
struct hkConeLimitConstraintAtom : hkConstraintAtom {
    enum MeasurementMode {
        ZERO_WHEN_VECTORS_ALIGNED = 0,
        ZERO_WHEN_VECTORS_PERPENDICULAR = 1
    };

    u8 m_isEnabled;                  // 0x02
    u8 m_twistAxisInA;               // 0x03
    u8 m_refAxisInB;                 // 0x04
    s8 m_angleMeasurementMode;       // 0x05 (MeasurementMode)
    hkReal m_minAngle;               // 0x08
    hkReal m_maxAngle;               // 0x0C
    hkReal m_angularLimitsTauFactor; // 0x10

    int numSolverResults() const;
};

// Size 0x0C.
struct hkAngFrictionConstraintAtom : hkConstraintAtom {
    u8 m_isEnabled;             // 0x02
    u8 m_firstFrictionAxis;     // 0x03
    u8 m_numFrictionAxes;       // 0x04
    hkReal m_maxFrictionTorque; // 0x08

    int numSolverResults() const;
};

// Size 0x14.
struct hkAngMotorConstraintAtom : hkConstraintAtom {
    hkBool m_isEnabled;                            // 0x02
    u8 m_motorAxis;                                // 0x03
    s16 m_initializedOffset;                       // 0x04
    s16 m_previousTargetAngleOffset;               // 0x06
    s16 m_correspondingAngLimitSolverResultOffset; // 0x08
    hkReal m_targetAngle;                          // 0x0C
    hkConstraintMotor* m_motor;                    // 0x10

    int numSolverResults() const;
};

// Size 0x50.
struct hkRagdollMotorConstraintAtom : hkConstraintAtom {
    hkBool m_isEnabled;               // 0x02
    s16 m_initializedOffset;          // 0x04
    s16 m_previousTargetAnglesOffset; // 0x06
    hkMatrix3 m_targetFrameAinB;      // 0x10
    hkConstraintMotor* m_motors[3];   // 0x40

    int numSolverResults() const;
};

// Size 0x40.
struct hkPulleyConstraintAtom : hkConstraintAtom {
    hkVector4 m_fixedPivotAinWorld; // 0x10
    hkVector4 m_fixedPivotBinWorld; // 0x20
    hkReal m_ropeLength;            // 0x30
    hkReal m_leverageOnBodyB;       // 0x34

    int numSolverResults() const;
};

// Modifier atom header, size 0x0C.
struct hkModifierConstraintAtom : hkConstraintAtom {
    u16 m_modifierAtomSize;    // 0x02
    u16 m_childSize;           // 0x04
    hkConstraintAtom* m_child; // 0x08

    u32 addModifierDataToConstraintInfo(hkConstraintInfo* info);
    u32 addAllModifierDataToConstraintInfo(hkConstraintInfo* info);
};

// Size 0x20: the reflected size needs 16-byte alignment (the members end at 0x14).
struct hkMassChangerModifierConstraintAtom : hkModifierConstraintAtom {
    hkReal m_factorA; // 0x0C
    hkReal m_factorB; // 0x10

    int numSolverResults() const;

    void collisionResponseBeginCallback(const hkContactPoint& cp, hkSimpleConstraintInfoInitInput& inA,
                                        hkBodyVelocity& velA, hkSimpleConstraintInfoInitInput& inB,
                                        hkBodyVelocity& velB);
    void collisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied,
                                      hkSimpleConstraintInfoInitInput& inA, hkBodyVelocity& velA,
                                      hkSimpleConstraintInfoInitInput& inB, hkBodyVelocity& velB);
} __attribute__((aligned(16)));

// Size 0x20: the reflected size needs 16-byte alignment (the members end at 0x14).
struct hkSoftContactModifierConstraintAtom : hkModifierConstraintAtom {
    hkReal m_tau;             // 0x0C
    hkReal m_maxAcceleration; // 0x10

    int numSolverResults() const;

    void collisionResponseBeginCallback(const hkContactPoint& cp, hkSimpleConstraintInfoInitInput& inA,
                                        hkBodyVelocity& velA, hkSimpleConstraintInfoInitInput& inB,
                                        hkBodyVelocity& velB);
    void collisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied,
                                      hkSimpleConstraintInfoInitInput& inA, hkBodyVelocity& velA,
                                      hkSimpleConstraintInfoInitInput& inB, hkBodyVelocity& velB);
} __attribute__((aligned(16)));

// Size 0x10: no members of its own; the reflected size needs 16-byte alignment.
struct hkViscousSurfaceModifierConstraintAtom : hkModifierConstraintAtom {
} __attribute__((aligned(16)));

// Size 0x20.
struct hkMovingSurfaceModifierConstraintAtom : hkModifierConstraintAtom {
    hkVector4 m_velocity; // 0x10

    void collisionResponseBeginCallback(const hkContactPoint& cp, hkSimpleConstraintInfoInitInput& inA,
                                        hkBodyVelocity& velA, hkSimpleConstraintInfoInitInput& inB,
                                        hkBodyVelocity& velB);
    void collisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied,
                                      hkSimpleConstraintInfoInitInput& inA, hkBodyVelocity& velA,
                                      hkSimpleConstraintInfoInitInput& inB, hkBodyVelocity& velB);
};
