#pragma once

#include <havok/hkVector4.h>

// Jacobian builder state used while the constraint atoms are expanded. The builder starts with a 4-float
// vector (the last position, its w at 0x0C is a scale factor) and has a second vector at 0x10 (w at 0x1C).
struct hkJacobianBuilder {
    hkVector4 m_lastPosition; // 0x00 HYPOTHESIS: name
    hkVector4 unk10;          // 0x10

    void initBuilder();
    void exitBuilder();
    void copyJacRegToJac1Reg();
    void mulInvJacDiag(float s);
    void buildLinearEnd(float a, float b);
    void buildAngularEnd(float a, float b);
    void addLastPosition(const hkVector4* b, hkVector4* c, hkVector4* d);
};
