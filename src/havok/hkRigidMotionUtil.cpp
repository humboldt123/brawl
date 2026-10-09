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

// Lane-wise reciprocal. A zero w lane gives 1.0f.
void hkVector4::setReciprocal4(const hkVector4& a) {
    const hkReal one = 1.0f;
    x = one / a.x;
    y = one / a.y;
    z = one / a.z;
    if (a.w == 0.0f) {
        w = one;
    } else {
        w = one / a.w;
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

void hkMotion::setDeactivationCounter(u16 deactivationCounter) {
    m_motionState.m_deactivationCounter = deactivationCounter;
}
