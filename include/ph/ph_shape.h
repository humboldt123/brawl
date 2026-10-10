#pragma once

#include <havok/hkBase.h>

struct hkRigidBody;
struct hkMotionState;
struct phPostureSource;

// HYPOTHESIS: non-polymorphic base at offset 0x00 so that the vptr lands at 0x04 as in the target.
struct phShapeHolder {
    hkRigidBody* m_rigidBody; // 0x00
};

// Brawl physics shape wrapper. Owns a hkRigidBody (0x1F0 bytes) that is removed from its world on destruction.
// Layout: 0x00 m_rigidBody, 0x04 vptr.
struct phShape : phShapeHolder {
    phShape(int flag); // HYPOTHESIS: word forwarded to the rigid body constructor (target passes it in r4 unchanged)
    virtual void draw(); // HYPOTHESIS: slot 0x08
    virtual ~phShape();
    virtual void getMatrixPosture(phPostureSource* const* src); // HYPOTHESIS: slot 0x0C
};
