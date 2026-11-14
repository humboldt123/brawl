// Havok translation unit hkConstraintAtom.o (main.dol 0x802D95D0-0x802D9AD8).
// Functions in address order (method names from the Havok TU map):
//   0x802D95D0    16  hkBridgeConstraintAtom_callData   [map: hkConstraintData__hkBridgeConstraintAtom_callData]
//   0x802D95E0     4  buildJacobian   [map: hkConstraintData__buildJacobian]
//   0x802D95E4    20  init   [map: hkBridgeConstraintAtom__init]
//   0x802D95F8   108  collisionResponseBeginCallback   [map: hkMassChangerModifierConstraintAtom__collisionResponseBeginCallback]
//   0x802D9664     4  collisionResponseEndCallback   [map: hkMassChangerModifierConstraintAtom__collisionResponseEndCallback]
//   0x802D9668   140  collisionResponseBeginCallback   [map: hkSoftContactModifierConstraintAtom__collisionResponseBeginCallback]
//   0x802D96F4   344  collisionResponseEndCallback   [map: hkSoftContactModifierConstraintAtom__collisionResponseEndCallback]
//   0x802D984C   180  collisionResponseBeginCallback   [map: hkMovingSurfaceModifierConstraintAtom__collisionResponseBeginCallback]
//   0x802D9900   180  collisionResponseEndCallback   [map: hkMovingSurfaceModifierConstraintAtom__collisionResponseEndCallback]
//   0x802D99B4   120  addModifierDataToConstraintInfo   [map: hkModifierConstraintAtom__addModifierDataToConstraintInfo]
//   0x802D9A2C   104  addAllModifierDataToConstraintInfo   [map: hkModifierConstraintAtom__addAllModifierDataToConstraintInfo]
//   0x802D9A94    68  __sinit_\hkConstraintAtom_cpp   [map: hkConstraintAtomcpp____sinit_]
//   0x802D9AD8     4  hkBodyVelocity::hkBodyVelocity (weak)
#include <havok/hkConstraintAtom.h>
#include <havok/hkConstraintData.h>

// Thunk stored in a bridge atom: forwards to the constraint's virtual buildJacobian (slot 9).
static void hkBridgeConstraintAtom_callData(hkConstraintData* data) {
    data->buildJacobian();
}

void hkConstraintData::buildJacobian() {
}

void hkBridgeConstraintAtom::init(hkConstraintData* data) {
    m_constraintData = data;
    m_buildJacobianFunc = hkBridgeConstraintAtom_callData;
}

void hkMassChangerModifierConstraintAtom::collisionResponseBeginCallback(const hkContactPoint& cp,
                                                                         hkSimpleConstraintInfoInitInput& inA,
                                                                         hkBodyVelocity& velA,
                                                                         hkSimpleConstraintInfoInitInput& inB,
                                                                         hkBodyVelocity& velB) {
    float factorA = m_factorA;
    float factorB = m_factorB;
    inA.m_invMass = inA.m_invMass * factorA;
    inA.m_invInertia.mul(factorA);
    inB.m_invMass = inB.m_invMass * factorB;
    inB.m_invInertia.mul(factorB);
}

void hkMassChangerModifierConstraintAtom::collisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied,
                                                                       hkSimpleConstraintInfoInitInput& inA,
                                                                       hkBodyVelocity& velA,
                                                                       hkSimpleConstraintInfoInitInput& inB,
                                                                       hkBodyVelocity& velB) {
}

// Body velocities saved by the soft-contact begin callback and blended back by its end callback (TOI events are
// processed one after another).
static hkBodyVelocity s_bodyVelocities[2];
static hkBool s_bodyVelocitiesInitialized(false);

void hkSoftContactModifierConstraintAtom::collisionResponseBeginCallback(const hkContactPoint& cp,
                                                                         hkSimpleConstraintInfoInitInput& inA,
                                                                         hkBodyVelocity& velA,
                                                                         hkSimpleConstraintInfoInitInput& inB,
                                                                         hkBodyVelocity& velB) {
    s_bodyVelocities[0] = velA;
    s_bodyVelocities[1] = velB;
}

// dst = u * saved + t * dst (t = tau, u = 1 - t), per float.
static inline void hkSoftContactBlend(float* dst, const float* saved, float t, float u) {
    dst[0] = u * saved[0] + t * dst[0];
    dst[1] = u * saved[1] + t * dst[1];
    dst[2] = u * saved[2] + t * dst[2];
    dst[3] = u * saved[3] + t * dst[3];
    dst[4] = u * saved[4] + t * dst[4];
    dst[5] = u * saved[5] + t * dst[5];
    dst[6] = u * saved[6] + t * dst[6];
    dst[7] = u * saved[7] + t * dst[7];
}

void hkSoftContactModifierConstraintAtom::collisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied,
                                                                       hkSimpleConstraintInfoInitInput& inA,
                                                                       hkBodyVelocity& velA,
                                                                       hkSimpleConstraintInfoInitInput& inB,
                                                                       hkBodyVelocity& velB) {
    hkReal usedImpulseFraction = m_tau;
    hkBodyVelocity& oldA = s_bodyVelocities[0];
    hkBodyVelocity& oldB = s_bodyVelocities[1];
    velA.m_linear.setInterpolate4(oldA.m_linear, velA.m_linear, usedImpulseFraction);
    velA.m_angular.setInterpolate4(oldA.m_angular, velA.m_angular, usedImpulseFraction);
    velB.m_linear.setInterpolate4(oldB.m_linear, velB.m_linear, usedImpulseFraction);
    velB.m_angular.setInterpolate4(oldB.m_angular, velB.m_angular, usedImpulseFraction);
}

void hkMovingSurfaceModifierConstraintAtom::collisionResponseBeginCallback(const hkContactPoint& cp,
                                                                           hkSimpleConstraintInfoInitInput& inA,
                                                                           hkBodyVelocity& velA,
                                                                           hkSimpleConstraintInfoInitInput& inB,
                                                                           hkBodyVelocity& velB) {
    hkVector4 vel = m_velocity;
    // Project the surface velocity into the contact plane, so the bodies do not sink in.
    hkSimdReal dot = vel.dot3(cp.m_normal);
    hkVector4 perp;
    perp.setMul4(dot, vel);
    vel.sub4(perp);
    velB.m_linear.add4(vel);
}

void hkMovingSurfaceModifierConstraintAtom::collisionResponseEndCallback(const hkContactPoint& cp, hkReal impulseApplied,
                                                                         hkSimpleConstraintInfoInitInput& inA,
                                                                         hkBodyVelocity& velA,
                                                                         hkSimpleConstraintInfoInitInput& inB,
                                                                         hkBodyVelocity& velB) {
    hkVector4 vel = m_velocity;
    // Project the surface velocity into the contact plane, so the bodies do not sink in.
    hkSimdReal dot = vel.dot3(cp.m_normal);
    hkVector4 perp;
    perp.setMul4(dot, vel);
    vel.sub4(perp);
    velB.m_linear.sub4(vel);
}

#pragma dont_inline on
// Returns a flag mask: 2 for soft-contact and viscous-surface atoms, 0 otherwise. Mass-changer and
// moving-surface atoms advance the accumulator offsets.
u32 hkModifierConstraintAtom::addModifierDataToConstraintInfo(hkConstraintInfo* info) {
    if (m_type == 0x19) { // VISCOUS_SURFACE
        return 2;
    }
    if (m_type > 0x19) {
        if (m_type < 0x1B) { // MOVING_SURFACE
            info->m_sizeOfSchemas += 0x20;
            info->m_sizeOfJacobians += 0x20;
        }
        return 0;
    }
    if (m_type == 0x17) { // SOFT_CONTACT
        return 2;
    }
    if (m_type > 0x17) { // MASS_CHANGER
        info->m_sizeOfSchemas += 0x20;
        info->m_sizeOfJacobians += 0x40;
    }
    return 0;
}

#pragma dont_inline reset
u32 hkModifierConstraintAtom::addAllModifierDataToConstraintInfo(hkConstraintInfo* info) {
    u32 flags = 0;
    hkModifierConstraintAtom* atom = this;
    while (true) {
        int type = atom->m_type;
        if (type < 0x17) {
            break;
        }
        flags |= atom->addModifierDataToConstraintInfo(info);
        atom = (hkModifierConstraintAtom*)atom->m_child;
    }
    return flags;
}
