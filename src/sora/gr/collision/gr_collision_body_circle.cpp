#include <gr/collision/gr_collision_linework.h>
#include <gr/collision/gr_collision_status.h>
#include <gr/collision/gr_collision_test_circle.h>
#include <mt/mt_coll2d.h>
#include <types.h>

// HYPOTHESIS: grCollision::getSegment (gr_collision.o) fills the segment of a line.
extern "C" void fn_80111968(grCollision* collision, void* out, grCollisionLine* line, int unk);
extern "C" int fn_80132468(grCollisionTestCircle* test, const clCircle2D* start, const clCircle2D* end, void* segment, float* rate);

grCollisionTestCircle::grCollisionTestCircle(grCollStatus* status) : grCollisionTest(status) {
    m_unk2bc = &m_shape1;
    m_unk2c0 = &m_shape2;
    m_unk2c4 = &m_shape3;
    m_unk2b8 = &m_shape0;
}

void grCollisionTestCircle::resetWork() {
    grCollisionTest::resetWork();
    m_unk2b8->set(*m_collStatus->m_prevCollShape);
    m_unk2bc->set(*m_collStatus->m_prevCollShape);
    m_unk2c0->set(*m_collStatus->m_currentCollShape);
    m_unk2c4->set(*m_unk2c0);
    Vec2f start = m_shape1.m_circle.m_centerPos;
    Vec2f end = m_shape3.m_circle.m_centerPos;
    Vec2f diff = end - start;
    m_unkE = 0;
    if (diff.m_x > 0.00001f) {
        m_unkE |= 2;
    }
    if (diff.m_x < -0.00001f) {
        m_unkE |= 1;
    }
    if (diff.m_y > 0.00001f) {
        m_unkE |= 4;
    }
    if (diff.m_y < -0.00001f) {
        m_unkE |= 8;
    }
}

void grCollisionTestCircle::updateCurrent(float rate) {
    m_shape1.m_circle.interpolate(m_shape2.m_circle, m_shape1.m_circle, rate);
}

void grCollisionTestCircle::updateTarget(float rate) {
    m_shape1.m_circle.interpolate(m_shape3.m_circle, m_shape2.m_circle, rate);
}

void grCollisionTestCircle::testStaticCorrectionSetup() { }

void grCollisionTestCircle::testCollisionSetup() { }

void grCollisionTestCircle::updateIgnoreLine() { }

int grCollisionTestCircle::testStaticCorrection(grCollisionLineWork* lineWork) {
    return 0;
}

int grCollisionTestCircle::testStaticCorrectionLast(grCollisionLineWork* lineWork) {
    return 0;
}

int grCollisionTestCircle::testCollisionNotMove(grCollisionLineWork* lineWork) {
    clCircle2D start = m_shape1.m_circle;
    clCircle2D end = m_shape2.m_circle;
    clSegment2D segment;
    float rate = m_unk8;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    int result = fn_80132468(this, &start, &end, &segment, &rate);
    if (result) {
        Vec2f normal = m_unk378 - m_unk370;
        m_unk8 = rate;
        if (__fabs(normal.m_y) < 0.00001f) {
            if (normal.m_x < 0.0f) {
                updateTouchLine(2, lineWork, rate);
            } else {
                updateTouchLine(1, lineWork, rate);
            }
        } else if (__fabs(normal.m_x) < 0.00001f) {
            if (normal.m_y < 0.0f) {
                updateTouchLine(0, lineWork, rate);
            } else {
                updateTouchLine(3, lineWork, rate);
            }
        } else if (__fabs(normal.m_y / normal.m_x) < 1.0f) {
            if (normal.m_x > 0.0f) {
                updateTouchLine(1, lineWork, rate);
            } else {
                updateTouchLine(2, lineWork, rate);
            }
        } else {
            if (normal.m_y > 0.0f) {
                updateTouchLine(3, lineWork, rate);
            } else {
                updateTouchLine(0, lineWork, rate);
            }
        }
    }
    return result;
}

int grCollisionTestCircle::testCollisionCompound(grCollisionLineWork* lineWork) {
    return grCollisionTestCircle::testCollisionNotMove(lineWork);
}
