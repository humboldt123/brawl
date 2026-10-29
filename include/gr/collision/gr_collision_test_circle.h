#pragma once

#include <StaticAssert.h>
#include <gr/collision/gr_collision_shape_circle.h>
#include <gr/collision/gr_collision_test.h>
#include <types.h>

class grCollisionTestCircle : public grCollisionTest {
public:
    // 816
    grCollShapeCircle m_shape0;
    grCollShapeCircle m_shape1; // HYPOTHESIS: the current shape, moved toward m_shape2 by updateCurrent
    grCollShapeCircle m_shape2;
    grCollShapeCircle m_shape3; // HYPOTHESIS: the target shape
    // 880
    Vec2f m_unk370; // HYPOTHESIS: the touched segment end
    Vec2f m_unk378; // HYPOTHESIS: the circle center at the touch

    grCollisionTestCircle(grCollStatus* status);

    virtual void resetWork();
    virtual void updateCurrent(float rate);
    virtual void updateTarget(float rate);
    virtual int testStaticCorrection(grCollisionLineWork* lineWork);
    virtual void testCollisionSetup();
    virtual int testCollisionNotMove(grCollisionLineWork* lineWork);
    virtual int testCollisionCompound(grCollisionLineWork* lineWork);
    virtual int testStaticCorrectionLast(grCollisionLineWork* lineWork);
    virtual void testStaticCorrectionSetup();
    virtual void updateIgnoreLine();
};
static_assert(sizeof(grCollisionTestCircle) == 0x380, "Class is wrong size!");
