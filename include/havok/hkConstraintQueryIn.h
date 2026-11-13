#pragma once

#include <havok/hkPadSpu.h>

struct hkVelocityAccumulator;
struct hkJacobianElement;
struct hkJacobianSchema;

// Solver step data handed to the constraint Jacobian builders (offsets from hkSimpleContactConstraintDataBuildJacobian).
// HYPOTHESIS: all names; only the members that function reads are named after their use.
struct hkConstraintQueryIn {
    hkPadSpu<hkReal> unk00;            // 0x00
    hkPadSpu<hkReal> unk04;            // 0x04
    hkPadSpu<hkReal> m_builderParam;   // 0x08 passed to hkJacobianBuilder::initBuilder
    hkPadSpu<hkReal> m_rhsParam;       // 0x0C passed to hkContactPoint::calculateRhs
    hkPadSpu<hkReal> unk10;            // 0x10
    hkPadSpu<hkReal> unk14;            // 0x14
    hkPadSpu<hkReal> m_frictionScale;  // 0x18 multiplies the friction limit
    hkPadSpu<hkReal> m_contactRhsScale; // 0x1C second argument of the contact rows' buildLinearEnd
    hkPadSpu<hkReal> m_tau;            // 0x20 passed to every build*Begin
    hkPadSpu<hkReal> m_frictionRhsScale; // 0x24 second argument of the friction rows' build*End
    hkPadSpu<const hkVelocityAccumulator*> m_bodyA; // 0x28
    hkPadSpu<const hkVelocityAccumulator*> m_bodyB; // 0x2C
    u8 unk30[0x48 - 0x30];
    hkPadSpu<u32> m_accumulatorAOffset; // 0x48 first offset written into the Jacobian header
    hkPadSpu<u32> m_accumulatorBOffset; // 0x4C
};

// Output cursors of the Jacobian builders.
struct hkConstraintQueryOut {
    hkPadSpu<hkJacobianElement*> m_jacobians;     // 0x00 next free Jacobian row
    hkPadSpu<hkJacobianElement*> m_jacobiansBase; // 0x04 HYPOTHESIS: the base the header's offset is taken from
    hkPadSpu<hkJacobianSchema*> m_jacobianSchemas; // 0x08 next free schema
};
