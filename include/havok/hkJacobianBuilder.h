#pragma once

#include <havok/hkVector4.h>

struct hkVelocityAccumulator;

// One linear and two angular Jacobian rows as the contact Jacobian builder fills them (0x30 bytes). The builder
// starts with a 4-float vector (the last position, its w at 0x0C is a scale factor) and has a second vector at
// 0x10 (w at 0x1C). The static members are the builder steps hkSimpleContactConstraintDataBuildJacobian calls;
// the map names all of them hkJacobianBuilder. HYPOTHESIS: parameter names and the meaning of the float arguments.
struct hkJacobianBuilder {
    hkVector4 m_lastPosition; // 0x00 HYPOTHESIS: name
    hkVector4 unk10;          // 0x10
    hkVector4 unk20;          // 0x20

    static void initBuilder(const hkVelocityAccumulator* bodyA, const hkVelocityAccumulator* bodyB, hkReal invDeltaTime);
    static void exitBuilder();
    static void copyJacRegToJac1Reg();
    // Out-of-line copies in hkSimpleContactConstraintInfo.o, not written yet (0x802943E4, 0x80294924, 0x80294DD4).
    static void buildLinearBegin(const hkVelocityAccumulator* bodyA, const hkVelocityAccumulator* bodyB,
                                 const hkVector4& position, const hkVector4& direction, hkJacobianBuilder* jac,
                                 hkReal tau);
    static hkReal getInvJac01Optimized(const hkVelocityAccumulator* bodyA, const hkVelocityAccumulator* bodyB,
                                       const hkJacobianBuilder* jacs, hkReal tau, hkReal coupling);
    static void buildAngularBegin(const hkVelocityAccumulator* bodyA, const hkVelocityAccumulator* bodyB,
                                  const hkVector4& direction, hkJacobianBuilder* jac, hkReal tau);
    void mulInvJacDiag(float s);
    void buildLinearEnd(float a, float b);
    void buildAngularEnd(float a, float b);
    // posSum += position, normalSum += normal.
    static void addLastPosition(const hkVector4* position, const hkVector4* normal, hkVector4* posSum,
                                hkVector4* normalSum);
};
