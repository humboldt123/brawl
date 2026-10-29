#pragma once

#include <StaticAssert.h>
#include <gr/collision/gr_collision_shape.h>
#include <types.h>

class grCollStatus;
class grCollisionLineWork;

class grCollisionTest {
public:
    // 0
    char _0[0x8];
    // 8
    float m_unk8; // HYPOTHESIS: the movement rate that testMoveCircleToSeg shortens on a hit
    // 12
    char _c[0x2];
    // 14
    u8 m_unkE; // HYPOTHESIS: direction bits of the current-to-target movement (1: -x, 2: +x, 4: +y, 8: -y)
    char _f[0x9];
    // 24
    grCollStatus* m_collStatus;
    char _1c[0x2b8 - 0x1c];
    // 696
    grCollShape* m_unk2b8;
    grCollShape* m_unk2bc;
    grCollShape* m_unk2c0;
    grCollShape* m_unk2c4;
    char _2c8[0x32c - 0x2c8];
    // 812 (vtable)

    grCollisionTest(grCollStatus* status);

    virtual void resetWork();
    virtual void updateCurrent(float rate) = 0;
    virtual void updateTarget(float rate) = 0;
    virtual void ajustShape(); // HYPOTHESIS: parameters unknown
    virtual int testStaticCorrection(grCollisionLineWork* lineWork) = 0; // HYPOTHESIS: parameter type
    virtual void testCollisionSetup() = 0;
    virtual int testCollisionNotMove(grCollisionLineWork* lineWork) = 0; // HYPOTHESIS: parameter type
    virtual int testCollisionCompound(grCollisionLineWork* lineWork) = 0; // HYPOTHESIS: parameter type
    virtual int testStaticCorrectionLast(grCollisionLineWork* lineWork) = 0; // HYPOTHESIS: parameter type
    virtual void testStaticCorrectionSetup() = 0;
    virtual void updateIgnoreLine() = 0;
    virtual int testCollisionNotMoveSlatingLine(grCollisionLineWork* lineWork, void* unk) = 0; // HYPOTHESIS: parameters
};
static_assert(sizeof(grCollisionTest) == 0x330, "Class is wrong size!");
