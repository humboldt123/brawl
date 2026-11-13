#pragma once

#include <havok/hkMotionState.h>

struct hkStepInfo;

// helpers that move a body's hkMotionState around (the swept transform lives inside it).
// signatures are from the asm. most match havok 5.1 but setTimeInformation takes the motion state FIRST here,
// 5.1 has it last. so this build is a bit older, don't assume the rest line up either
struct hkSweptTransformUtil {
    // pose at time t, blended between the start and end of the step. result goes in out
    static void lerp2(const hkSweptTransform& st, hkReal t, hkTransform& out);
    // same but t gets tAddOn nudged on for extra precision
    static void lerp2Ha(const hkSweptTransform& st, hkReal t, hkReal tAddOn, hkTransform& out);

    // rewind the end of the step back to time (only touches the end pose + the transform)
    static void backStepMotionState(hkReal time, hkMotionState& ms);
    // Moves the motion state over the step with the velocities (map name hkSweptTransformUtil::_stepMotionState;
    // its out-of-line copy in hkRigidMotionUtil.o is not written yet). HYPOTHESIS: the parameter list.
    static void _stepMotionState(const hkStepInfo& info, hkVector4& linearVelocity, hkVector4& angularVelocity,
                                 hkMotionState& motionState);
    // pin the body at time: start and end pose become the same and invDeltaTime goes to 0
    static void freezeMotionState(hkReal time, hkMotionState& ms);

    static void setTimeInformation(hkMotionState& ms, hkReal t0, hkReal t1);

    // teleport stuff. these all kill the delta angle and make the swept transform a single point
    static void warpTo(const hkVector4& position, const hkQuaternion& rotation, hkMotionState& ms);
    static void warpTo(const hkTransform& transform, hkMotionState& ms);
    static void warpToPosition(const hkVector4& position, hkMotionState& ms);
    static void warpToRotation(const hkQuaternion& rotation, hkMotionState& ms);

    // move the center of mass offset (in local space) without moving the body, so the swept
    // centers have to shift by the rotated difference
    static void setCentreOfRotationLocal(const hkVector4& newCenter, hkMotionState& ms);

    // Time of impact information for a pair over the step: xyz is the relative linear motion of the two centers of mass
    // over deltaTime (A's backwards, B's forwards), w the bound on the distance the angular motion can add (each body's
    // delta angle times its object radius). Inline: hkListAgent::processCollision expands it; the out-of-line copy in
    // hkBvTreeAgent.o comes from its caller there being compiled without inlining.
    // HYPOTHESIS: parameter types (the map has none).
    static void calcTimInfo(const hkMotionState& ms0, const hkMotionState& ms1, hkReal deltaTime, hkVector4& timInfo);
};

// The two center of mass differences are written lane by lane: the target stores them x first (set() would evaluate
// w first).
inline void hkSweptTransformUtil::calcTimInfo(const hkMotionState& ms0, const hkMotionState& ms1, hkReal deltaTime,
                                              hkVector4& timInfo) {
    hkVector4 diff0;
    diff0.x = ms0.m_sweptTransform.m_centerOfMass0.x - ms0.m_sweptTransform.m_centerOfMass1.x;
    diff0.y = ms0.m_sweptTransform.m_centerOfMass0.y - ms0.m_sweptTransform.m_centerOfMass1.y;
    diff0.z = ms0.m_sweptTransform.m_centerOfMass0.z - ms0.m_sweptTransform.m_centerOfMass1.z;
    diff0.w = ms0.m_sweptTransform.m_centerOfMass0.w - ms0.m_sweptTransform.m_centerOfMass1.w;
    hkVector4 diff1;
    diff1.x = ms1.m_sweptTransform.m_centerOfMass1.x - ms1.m_sweptTransform.m_centerOfMass0.x;
    diff1.y = ms1.m_sweptTransform.m_centerOfMass1.y - ms1.m_sweptTransform.m_centerOfMass0.y;
    diff1.z = ms1.m_sweptTransform.m_centerOfMass1.z - ms1.m_sweptTransform.m_centerOfMass0.z;
    diff1.w = ms1.m_sweptTransform.m_centerOfMass1.w - ms1.m_sweptTransform.m_centerOfMass0.w;
    // HYPOTHESIS: the center of mass 1 w lane is the inverse step length (Havok's swept transform keeps it there).
    hkReal f0 = deltaTime * ms0.m_sweptTransform.m_centerOfMass1.w;
    hkReal f1 = deltaTime * ms1.m_sweptTransform.m_centerOfMass1.w;
    timInfo.setMul4(f0, diff0);
    timInfo.addMul4(f1, diff1);
    timInfo.w = f0 * ms0.m_deltaAngle.w * ms0.m_objectRadius + f1 * ms1.m_deltaAngle.w * ms1.m_objectRadius;
}

