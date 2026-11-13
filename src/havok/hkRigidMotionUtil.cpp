// Havok translation unit hkRigidMotionUtil.o (main.dol 0x802E61F0-0x802E7878).
// Partly decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E61F0  1496  hkRigidMotionUtilStep   [map: hkRigidMotionUtil__hkRigidMotionUtilStep]
//   0x802E67C8  1344  _stepMotionState   [map: hkSweptTransformUtil___stepMotionState]
//   0x802E6D08     8  hkAddByteOffset<8hkMotion>   [map: hkMotion__hkAddByteOffset_8hkMotion_]
//   0x802E6D10   552  hkRigidMotionUtilApplyForcesAndStep   [map: hkRigidMotionUtil__hkRigidMotionUtilApplyForcesAndStep]
//   0x802E6F38     8  __opQ28hkMotion10MotionType   [map: hkEnum_Q28hkMotion10MotionType_Uc_____opQ28hkMotion10MotionType]
//   0x802E6F40    76  setReciprocal4   [map: hkVector4__setReciprocal4]
//   0x802E6F8C    68  mul4   [map: hkVector4__mul41]
//   0x802E6FD0  1412  hkRigidMotionUtilApplyForcesAndBuildAccumulators   [map: hkRigidMotionUtil__hkRigidMotionUtilApplyForcesAndBuildAccumulators]
//   0x802E7554   788  hkRigidMotionUtilApplyAccumulators   [map: hkRigidMotionUtil__hkRigidMotionUtilApplyAccumulators]
//   0x802E7868     8  getSumAngularVel   [map: hkVelocityAccumulator__getSumAngularVel]
//   0x802E7870     8  setDeactivationCounter   [map: hkMotion__setDeactivationCounter]
#include <havok/hkMotion.h>
#include <havok/hkRigidMotionUtil.h>
#include <havok/hkVelocityAccumulator.h>
#include <havok/hkMath.h>
#include <havok/hkStepInfo.h>
#include <havok/hkSweptTransformUtil.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkTransform.h>

// BYTES: the two step loops are compiled without inlining; every accessor is a call to its out-of-line copy.
#pragma push
#pragma dont_inline on
// Integrates the motions of the entities without a solver: gravity and damping on the velocities, then the motion
// state step. A thin box keeps its angular momentum (not its angular velocity) over the step.
void hkRigidMotionUtil::hkRigidMotionUtilApplyForcesAndStep(const hkStepInfo& info, const hkVector4& deltaVel,
                                                            hkEntity** entities, int numEntities, int motionOffset) {
    for (int i = numEntities - 1; i >= 0; entities++, i--) {
        hkMotion* motion = hkAddByteOffset((hkMotion*)*entities, motionOffset);
        switch (motion->m_type) {
        case hkMotion::MOTION_THIN_BOX_INERTIA: {
            motion->m_linearVelocity.add4(deltaVel);
            {
                hkReal deltaTime = info.m_deltaTime;
                motion->m_linearVelocity.mul4(
                    hkMath::max2f(0.0f, 1.0f - deltaTime * motion->m_motionState.m_linearDamping));
                motion->m_angularVelocity.mul4(
                    hkMath::max2f(0.0f, 1.0f - deltaTime * motion->m_motionState.m_angularDamping));
            }
            hkVector4 angularMomentum;
            hkVector4 localVelocity;
            localVelocity.setRotatedInverseDir(motion->getTransform().getRotation(), motion->m_angularVelocity);
            hkVector4 inertia;
            inertia.setReciprocal4(motion->m_inertiaAndMassInv);
            localVelocity.mul4(inertia);
            angularMomentum.setRotatedDir(motion->getTransform().getRotation(), localVelocity);
            hkSweptTransformUtil::_stepMotionState(info, motion->m_linearVelocity, motion->m_angularVelocity,
                                                   motion->m_motionState);
            hkVector4 localMomentum;
            localMomentum.setRotatedInverseDir(motion->getTransform().getRotation(), angularMomentum);
            localMomentum.mul4(motion->m_inertiaAndMassInv);
            motion->m_angularVelocity.setRotatedDir(motion->getTransform().getRotation(), localMomentum);
            break;
        }
        case hkMotion::MOTION_FIXED:
            break;
        default:
            motion->m_linearVelocity.add4(deltaVel);
            {
                hkReal deltaTime = info.m_deltaTime;
                motion->m_linearVelocity.mul4(
                    hkMath::max2f(0.0f, 1.0f - deltaTime * motion->m_motionState.m_linearDamping));
                motion->m_angularVelocity.mul4(
                    hkMath::max2f(0.0f, 1.0f - deltaTime * motion->m_motionState.m_angularDamping));
            }
            // fall through
        case hkMotion::MOTION_KEYFRAMED:
            hkSweptTransformUtil::_stepMotionState(info, motion->m_linearVelocity, motion->m_angularVelocity,
                                                   motion->m_motionState);
            break;
        }
    }
}

// Writes the solver's velocities back into the motions and steps them. The accumulators follow the order of the
// entities (0x80 bytes each); how the summed velocities turn into the motion's velocities depends on the motion
// type (box inertias keep the angular velocity in the solver's local frame).
void hkRigidMotionUtil::hkRigidMotionUtilApplyAccumulators(const hkStepInfo& info,
                                                           hkVelocityAccumulator* accumulators, hkEntity** entities,
                                                           int numEntities, int motionOffset, void* unk5) {
    hkVelocityAccumulator* accumulator = accumulators;
    for (int i = numEntities - 1; i >= 0; entities++, accumulator++, i--) {
        hkMotion* motion = hkAddByteOffset((hkMotion*)*entities, motionOffset);
        switch (motion->m_type) {
        case hkMotion::MOTION_INVALID:
        case hkMotion::MOTION_THIN_BOX_INERTIA: {
            motion->m_linearVelocity = *accumulator->getSumLinearVel();
            motion->m_angularVelocity._setRotatedDir(motion->getTransform().getRotation(),
                                                     *accumulator->getSumAngularVel());
            motion->setDeactivationCounter(accumulator->unk4);
            hkVector4 angularMomentum;
            hkVector4 localVelocity;
            localVelocity.setRotatedInverseDir(motion->getTransform().getRotation(), motion->m_angularVelocity);
            hkVector4 inertia;
            inertia.setReciprocal4(motion->m_inertiaAndMassInv);
            localVelocity.mul4(inertia);
            angularMomentum.setRotatedDir(motion->getTransform().getRotation(), localVelocity);
            hkSweptTransformUtil::_stepMotionState(info, motion->m_linearVelocity, motion->m_angularVelocity,
                                                   motion->m_motionState);
            motion->m_linearVelocity = accumulator->m_linearVel;
            hkVector4 localMomentum;
            localMomentum.setRotatedInverseDir(motion->getTransform().getRotation(), angularMomentum);
            localMomentum.mul4(motion->m_inertiaAndMassInv);
            motion->m_angularVelocity.setRotatedDir(motion->getTransform().getRotation(), localMomentum);
            break;
        }
        case hkMotion::MOTION_STABILIZED_BOX_INERTIA:
            accumulator->getSumAngularVel()->setRotatedDir(motion->getTransform().getRotation(),
                                                           *accumulator->getSumAngularVel());
            accumulator->m_angularVel = *accumulator->getSumAngularVel();
            accumulator->m_linearVel = *accumulator->getSumLinearVel();
            goto SPHERE;
        case hkMotion::MOTION_STABILIZED_SPHERE_INERTIA:
            accumulator->m_angularVel = *accumulator->getSumAngularVel();
            accumulator->m_linearVel = *accumulator->getSumLinearVel();
            goto SPHERE;
        case hkMotion::MOTION_KEYFRAMED:
            accumulator->m_linearVel = motion->m_linearVelocity;
            accumulator->m_angularVel = motion->m_angularVelocity;
            *accumulator->getSumLinearVel() = motion->m_linearVelocity;
            *accumulator->getSumAngularVel() = motion->m_angularVelocity;
            goto STEP;
        case hkMotion::MOTION_BOX_INERTIA:
            accumulator->getSumAngularVel()->setRotatedDir(motion->getTransform().getRotation(),
                                                           *accumulator->getSumAngularVel());
            accumulator->m_angularVel._setRotatedDir(motion->getTransform().getRotation(), accumulator->m_angularVel);
            // fall through
        case hkMotion::MOTION_SPHERE_INERTIA:
        SPHERE:
            motion->setDeactivationCounter(accumulator->unk4);
        STEP:
            hkSweptTransformUtil::_stepMotionState(info, *accumulator->getSumLinearVel(),
                                                   *accumulator->getSumAngularVel(), motion->m_motionState);
            motion->m_linearVelocity = accumulator->m_linearVel;
            motion->m_angularVelocity = accumulator->m_angularVel;
            break;
        }
    }
}
#pragma pop

// Lane-wise reciprocal. A zero w lane gives 1.0f.
void hkVector4::setReciprocal4(const hkVector4& a) {
    const hkReal one = 1.0f;
    x = one / a.x;
    y = one / a.y;
    z = one / a.z;
    if (a.w != 0.0f) {
        w = one / a.w;
    } else {
        w = one;
    }
}

// Lane-wise product in place: this *= a.
void hkVector4::mul4(const hkVector4& a) {
    x *= a.x;
    y *= a.y;
    z *= a.z;
    w *= a.w;
}

hkVector4* hkVelocityAccumulator::getSumAngularVel() {
    return reinterpret_cast<hkVector4*>(reinterpret_cast<char*>(this) + 0x50);
}

void hkMotion::setDeactivationCounter(int deactivationCounter) {
    m_motionState.m_deactivationCounter = deactivationCounter;
}
