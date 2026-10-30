// MATCH-ONLY: this unit copies whole vectors with the aggregate word copy of a plain base (see mt_vector.h).
#define MT_VEC2F_PLAIN_BASE
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_collection.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_status.h>
#include <gr/collision/gr_collision_test_rhombus.h>
#include <types.h>
#include <nw4r/math/math_arithmetic.h>

// ===== cluster R1 =====

// MATCH-ONLY: the original takes the componentwise minimum and maximum of the two segment end points with paired singles.
static inline void segmentBounds(register const clSegment2D* segment, register Vec2f* lo, register Vec2f* hi) {
    register float p0, p1, diffMin, diffMax, minPair, maxPair;
    // clang-format off
    asm {
        psq_l p0, 0(segment), 0, 0
        psq_l p1, 8(segment), 0, 0
        ps_sub diffMin, p0, p1
        ps_sub diffMax, p0, p1
        ps_sel minPair, diffMin, p1, p0
        ps_sel maxPair, diffMax, p0, p1
        psq_st minPair, 0(lo), 0, 0
        psq_st maxPair, 0(hi), 0, 0
    }
    // clang-format on
}

static inline void setEdge(clSegment2D* edge, const Vec2f& from, const Vec2f& to) {
    edge->m_p0Pos.m_x = from.m_x;
    edge->m_p0Pos.m_y = from.m_y;
    edge->m_p1Pos.m_x = to.m_x;
    edge->m_p1Pos.m_y = to.m_y;
}

// 0 when the segment misses the rhombus; else the edges of the rhombus the segment crosses, or 0x10 when the whole segment is inside.
u8 grCollisionTestRhombus::checkIntersect(clRhombus2D* rhombus, clSegment2D* segment) {
    u32 result = 0;
    Vec2f lo;
    Vec2f hi;
    segmentBounds(segment, &lo, &hi);
    if (!(lo.m_y > rhombus->m_upPos.m_y || lo.m_x > rhombus->m_rightPos.m_x || hi.m_y < rhombus->m_downPos.m_y ||
          hi.m_x < rhombus->m_leftPos.m_x)) {
        result = rhombus->getIntersectInfo(*segment);
        if (result == 0) {
            // no edge is crossed: the segment is inside when both end points are on the inner side of all four edges
            clSegment2D edge(rhombus->m_upPos, rhombus->m_rightPos);
            if (!edge.isLeftArea(segment->m_p0Pos) && !edge.isLeftArea(segment->m_p1Pos)) {
                setEdge(&edge, rhombus->m_rightPos, rhombus->m_downPos);
                if (!edge.isLeftArea(segment->m_p0Pos) && !edge.isLeftArea(segment->m_p1Pos)) {
                    setEdge(&edge, rhombus->m_downPos, rhombus->m_leftPos);
                    if (!edge.isLeftArea(segment->m_p0Pos) && !edge.isLeftArea(segment->m_p1Pos)) {
                        setEdge(&edge, rhombus->m_leftPos, rhombus->m_upPos);
                        if (!edge.isLeftArea(segment->m_p0Pos) && !edge.isLeftArea(segment->m_p1Pos)) {
                            result = 0x10;
                        }
                    }
                }
            }
        }
    }
    return result;
}

grCollisionTestRhombus::grCollisionTestRhombus(grCollStatus* status) : grCollisionTest(status) {
    m_unk2bc = &m_shape1;
    m_unk2c0 = &m_shape2;
    m_unk2c4 = &m_shape3;
    m_unk2b8 = &m_shape0;
}

// A line work whose line, joint and collision are all set (checked in two steps, as the original inline does).
static inline bool hasLine(const grCollisionLineWork* lineWork) {
    bool valid = false;
    bool hasJoint = false;
    if (lineWork->m_collisionLine != NULL && lineWork->m_collisionJoint != NULL) {
        hasJoint = true;
    }
    if (hasJoint && lineWork->m_collision != NULL) {
        valid = true;
    }
    return valid;
}

void grCollisionTestRhombus::resetWork() {
    grCollisionTest::resetWork();
    m_unk2b8->set(*m_collStatus->m_prevCollShape);
    m_unk2bc->set(*m_collStatus->m_prevCollShape);
    m_unk2c0->set(*m_collStatus->m_currentCollShape);
    m_unk2c4->set(*m_unk2c0);
    m_unk2E4 = m_unk2bc->getCenterPos();
    Vec2f center = m_shape3.m_rhombus.m_centerPos;
    Vec2f diff = center - m_shape1.m_rhombus.m_centerPos;
    m_cornerOffsetLeft = m_shape3.m_rhombus.m_leftPos - center;
    m_cornerOffsetRight = m_shape3.m_rhombus.m_rightPos - center;
    m_cornerOffsetUp = m_shape3.m_rhombus.m_upPos - center;
    m_cornerOffsetDown = m_shape3.m_rhombus.m_downPos - center;
    m_unkE = 0;
    m_moveVec = diff;
    if (diff.m_x > 0.00001f) {
        m_unkE |= DIR_RIGHT;
    }
    if (diff.m_x < -0.00001f) {
        m_unkE |= DIR_LEFT;
    }
    if (diff.m_y > 0.00001f) {
        m_unkE |= DIR_UP;
    }
    if (diff.m_y < -0.00001f) {
        m_unkE |= DIR_DOWN;
    }
    bool mostlyHorizontal = false;
    m_unk47C = 0;
    Vec2f step = (m_shape2.m_rhombus.m_centerPos - m_shape1.m_rhombus.m_centerPos) - m_collStatus->m_touchLineSpeed;
    if (m_collStatus->m_tracerType) {
        if ((float)__fabs(step.m_y) <= 0.00001f || (float)__fabs(step.m_x / step.m_y) > 1.0f) {
            mostlyHorizontal = true;
        }
    } else if (m_collStatus->m_setting != grCollStatus::Setting_Ground) {
        bool done = false;
        if (m_collStatus->m_setting == grCollStatus::Setting_Cliff) {
            Vec2f down(m_shape1.m_rhombus.m_downPos.m_x, m_shape1.m_rhombus.m_downPos.m_y);
            grCollisionLineWork& cliff = m_collStatus->m_collLineWorkArray[8];
            if (hasLine(&cliff)) {
                clSegment2D segment;
                cliff.m_collision->getSegment(&segment, cliff.m_collisionLine);
                Vec2f end;
                if (m_unk20 == 1) {
                    end = Vec2f(segment.m_p0Pos.m_x, segment.m_p0Pos.m_y);
                } else {
                    end = Vec2f(segment.m_p1Pos.m_x, segment.m_p1Pos.m_y);
                }
                if ((float)__fabs(down.m_x - end.m_x) < 2.0f && (float)__fabs(down.m_y - end.m_y) < 1.0f) {
                    mostlyHorizontal = true;
                    done = true;
                }
            }
        }
        if (!done) {
            if ((float)__fabs(step.m_y) < 0.05f && (float)__fabs(step.m_x) > 0.4f) {
                mostlyHorizontal = true;
            } else if ((float)__fabs(step.m_y) > 0.00001f && (float)__fabs(step.m_x) > 0.5f && (float)__fabs(step.m_x / step.m_y) > 1.0f) {
                mostlyHorizontal = true;
            }
        }
    }
    if (mostlyHorizontal) {
        m_unk47C = 1;
    }
}

void grCollisionTestRhombus::updateCurrent(float rate) {
    m_shape1.m_rhombus.interpolate(m_shape2.m_rhombus, m_shape1.m_rhombus, rate);
}

void grCollisionTestRhombus::updateTarget(float rate) {
    m_shape1.m_rhombus.interpolate(m_shape3.m_rhombus, m_shape2.m_rhombus, rate);
}

// A line work whose line, joint and collision are all set, whose joint is active and whose collision is enabled.
static inline bool isActiveLineWork(grCollisionLineWork* lineWork) {
    bool valid = false;
    if (lineWork->m_collisionLine != NULL && lineWork->m_collisionJoint != NULL && lineWork->m_collision != NULL) {
        valid = true;
    }
    bool active;
    if (valid && lineWork->m_collisionJoint->m_0x54_4 && lineWork->m_collision->m_isEnabled) {
        active = true;
    } else {
        active = false;
    }
    return active;
}

// Drops the ignore-list entries the rhombus has left behind and adds the floors it is about to cross from below.
// For every collected floor: if the rhombus' down corner x is within the floor's x range, then the corner either crossed
// the floor (then the floor is remembered as ignored, unless a wall-type neighbour makes it a dead end) or moved clear of it.
void grCollisionTestRhombus::updateIgnoreLine() {
    grCollisionLineWork lineWork;
    clRhombus2D* rhombus = &m_shape1.m_rhombus;
    float downX = rhombus->m_downPos.m_x;
    Vec2f projected;
    u16 num = m_collection->m_lineWorks.m_num;
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* work = &m_collection->m_lineWorks.m_works[i];
        lineWork = *work;
        if (isActiveLineWork(&lineWork) && lineWork.m_collisionLine->m_isUnder) {
            grCollSegmentEx segment;
            fn_80111968(work->m_collision, &segment, work->m_collisionLine, 0);
            if (rhombus->getIntersectInfo(segment.m_segment) != 0) {
                if (segment.m_segment.m_p0Pos.m_x < downX && downX < segment.m_segment.m_p1Pos.m_x &&
                    segment.m_segment.projectionX(rhombus->m_downPos, projected) && rhombus->m_downPos.m_y < projected.m_y &&
                    projected.m_y < rhombus->m_upPos.m_y) {
                    if ((!work->getPrevNoVLink(&lineWork) || !lineWork.m_collisionLine->m_isRight) &&
                        (!work->getNextNoVLink(&lineWork) || !lineWork.m_collisionLine->m_isLeft) &&
                        (!work->getPrev(&lineWork) || !lineWork.m_collisionLine->m_isRight) &&
                        (!work->getNext(&lineWork) || !lineWork.m_collisionLine->m_isLeft)) {
                        fn_80137074(m_collStatus, work);
                    }
                }
            } else {
                if (segment.m_segment.m_p0Pos.m_x < downX && downX < segment.m_segment.m_p1Pos.m_x &&
                    segment.m_segment.projectionX(rhombus->m_downPos, projected) &&
                    (projected.m_y < rhombus->m_downPos.m_y || rhombus->m_upPos.m_y < projected.m_y)) {
                    fn_80137298(m_collStatus, work);
                }
            }
        }
    }
}

// The free space around a point: how far the lines of the collection leave room to the left / right / up / down of it.
// The box is first the collected area (shrunk by 0.01); every ceiling, floor and wall line that reaches into it moves the matching
// edge in. The edges are stored in m_unk2EC (up) / m_unk2F4 (down) / m_unk2FC (left) / m_unk304 (right); outSpace = (width, height).
// The floor that closes the box from below becomes the ground line m_unk31C.
void grCollisionTestRhombus::calcSpace(const Vec2f* pos, Vec2f* outSpace) {
    u16 num = m_collection->m_lineWorks.m_num;
    u16 mask = createCheckMaskStatic();
    clAABBox2D* area = (clAABBox2D*)m_collection;
    float top = area->m_unk4 - 0.01f;
    float left = area->m_unk1 + 0.01f;
    float right = area->m_unk3 - 0.01f;
    float bottom;
    if (m_collStatus->m_setting == grCollStatus::Setting_Ground) {
        Vec2f groundPos = m_unk2bc->getDownPos();
        bottom = groundPos.m_y;
    } else {
        bottom = area->m_unk2 + 0.01f;
    }
    float centerX = m_unk2E4.m_x;
    Vec2f centerPos = m_unk2bc->getCenterPos();
    float centerY = centerPos.m_y;
    Vec2f downPos = m_unk2bc->getDownPos();
    float downY = downPos.m_y;
    Vec2f topPos = m_unk2bc->getTopPos();
    float topY = topPos.m_y;
    Vec2f leftPos = m_unk2bc->getLeftPos();
    Vec2f rightPos = m_unk2bc->getRightPos();
    grCollisionLineWork neighbor;
    grCollisionLineWork lineWork;
    grCollisionLine* groundLine = m_touch[3].m_lineWork.m_collisionLine;
    bool onGround = m_collStatus->m_setting == grCollStatus::Setting_Ground;
    if (onGround) {
        onGround = isActiveLineWork(&m_touch[3].m_lineWork);
    }
    for (u32 i = 0; i < num; i++) {
        lineWork = m_collection->m_lineWorks.m_works[i];
        if ((mask & lineWork.m_collisionLine->m_unkE) != 0 && lineWork.m_unkC == 0 &&
            (!lineWork.m_collisionLine->m_isUnder || fn_80137114(m_collStatus, &lineWork) == 0)) {
            clSegment2D segment;
            lineWork.m_collision->getSegment(&segment, lineWork.m_collisionLine);
            grCollisionLine* line = lineWork.m_collisionLine;
            Vec2f projected;
            if (line->m_isUpper) {
                if (segment.m_p1Pos.m_x <= centerX && centerX <= segment.m_p0Pos.m_x && segment.projectionX(*pos, projected) &&
                    centerY < projected.m_y && projected.m_y < top) {
                    top = projected.m_y;
                }
            } else if (line->m_isUnder) {
                if (segment.m_p0Pos.m_x <= centerX && centerX <= segment.m_p1Pos.m_x && segment.projectionX(*pos, projected) &&
                    bottom < projected.m_y && projected.m_y < centerY) {
                    m_unk31C = lineWork;
                    bottom = projected.m_y;
                }
            } else if (line->m_isLeft) {
                if (onGround) {
                    if (lineWork.getPrev(&neighbor) && isActiveLineWork(&neighbor) && neighbor.m_collisionLine == groundLine) {
                        continue;
                    }
                    if (lineWork.getPrevNoVLink(&neighbor) && isActiveLineWork(&neighbor) && neighbor.m_collisionLine == groundLine) {
                        continue;
                    }
                }
                if (centerY < segment.m_p1Pos.m_y || segment.m_p0Pos.m_y < centerY) {
                    if (centerX <= segment.m_p0Pos.m_x || segment.m_p0Pos.m_y < downY || centerY < segment.m_p0Pos.m_y) {
                        if (segment.m_p1Pos.m_x < centerX && centerY <= segment.m_p1Pos.m_y && segment.m_p1Pos.m_y <= topY) {
                            clSegment2D edge(m_shape1.m_rhombus.m_leftPos, m_shape1.m_rhombus.m_upPos);
                            if (edge.projectionY(segment.m_p1Pos, projected)) {
                                float x = m_shape1.m_rhombus.m_leftPos.m_x + (segment.m_p1Pos.m_x - projected.m_x);
                                if (left < x && x < centerX) {
                                    left = x;
                                }
                            }
                        }
                    } else {
                        clSegment2D edge(m_shape1.m_rhombus.m_leftPos, m_shape1.m_rhombus.m_downPos);
                        if (edge.projectionY(segment.m_p0Pos, projected)) {
                            float x = m_shape1.m_rhombus.m_leftPos.m_x + (segment.m_p0Pos.m_x - projected.m_x);
                            if (left < x && x < centerX) {
                                left = x;
                            }
                        }
                    }
                } else if (segment.projectionY(*pos, projected) && left < projected.m_x && projected.m_x < centerX) {
                    left = projected.m_x;
                }
            } else if (line->m_isRight) {
                if (onGround) {
                    if (lineWork.getNext(&neighbor) && isActiveLineWork(&neighbor) && neighbor.m_collisionLine == groundLine) {
                        continue;
                    }
                    if (lineWork.getNextNoVLink(&neighbor) && isActiveLineWork(&neighbor) && neighbor.m_collisionLine == groundLine) {
                        continue;
                    }
                }
                if (centerY < segment.m_p0Pos.m_y || segment.m_p1Pos.m_y < centerY) {
                    if (segment.m_p1Pos.m_x <= centerX || segment.m_p1Pos.m_y < downY || centerY < segment.m_p1Pos.m_y) {
                        if (centerX < segment.m_p0Pos.m_x && centerY <= segment.m_p0Pos.m_y && segment.m_p0Pos.m_y <= topY) {
                            clSegment2D edge(m_shape1.m_rhombus.m_rightPos, m_shape1.m_rhombus.m_upPos);
                            if (edge.projectionY(segment.m_p0Pos, projected)) {
                                float x = m_shape1.m_rhombus.m_rightPos.m_x + (segment.m_p0Pos.m_x - projected.m_x);
                                if (centerX < x && x < right) {
                                    right = x;
                                }
                            }
                        }
                    } else {
                        clSegment2D edge(m_shape1.m_rhombus.m_rightPos, m_shape1.m_rhombus.m_downPos);
                        if (edge.projectionY(segment.m_p1Pos, projected)) {
                            float x = m_shape1.m_rhombus.m_rightPos.m_x + (segment.m_p1Pos.m_x - projected.m_x);
                            if (centerX < x && x < right) {
                                right = x;
                            }
                        }
                    }
                } else if (segment.projectionY(*pos, projected) && centerX < projected.m_x && projected.m_x < right) {
                    right = projected.m_x;
                }
            }
        }
    }
    m_unk2EC.m_x = centerX;
    m_unk2EC.m_y = top;
    m_unk2F4.m_x = centerX;
    m_unk2F4.m_y = bottom;
    m_unk304.m_x = right;
    m_unk304.m_y = centerY;
    m_unk2FC.m_x = left;
    m_unk2FC.m_y = centerY;
    outSpace->m_x = right - left;
    outSpace->m_y = top - bottom;
}

static inline int isFinite(float f) {
    int res = 1;
    u32 i;
    __memcpy(&i, &f, 4);
    if ((i >> 23 & 0xFF) == 0xFF)
        res = 0;
    return res;
}

// Squeezes the rhombus into the free box left by calcSpace (m_unk2FC.x .. m_unk304.x): the left and right corners go to the box
// edges (less a margin) and the centre column to the position that keeps the original left/width proportion.
int grCollisionTestRhombus::calcShapeMin(clRhombus2D* rhombus) {
    float left = m_cornerOffsetLeft.m_x;
    float width = m_cornerOffsetRight.m_x - left;
    if (width < 0.21f) {
        width = 0.2f;
        left = 0.5f;
    }
    float minX = 0.01f + m_unk2FC.m_x;
    float maxX = m_unk304.m_x - 0.01f;
    rhombus->m_leftPos.m_x = minX;
    rhombus->m_rightPos.m_x = maxX;
    rhombus->m_aabBox.m_unk1 = minX;
    rhombus->m_aabBox.m_unk2 = rhombus->m_downPos.m_y;
    float centerX = minX + (maxX - minX) * ((float)__fabs(left) / width);
    rhombus->m_aabBox.m_unk3 = maxX;
    rhombus->m_aabBox.m_unk4 = rhombus->m_upPos.m_y;
    rhombus->m_centerPos.m_x = centerX;
    rhombus->m_upPos.m_x = centerX;
    rhombus->m_downPos.m_x = centerX;
    return isFinite(rhombus->m_aabBox.m_unk3);
}

// Fits the (current) rhombus back to its full size around the centre after the corrections: the centre is first pulled inside
// the left/right and down/up corners, then the free space around it decides between the full size (offsets from the target
// shape) and the squeezed one (calcShapeMin) horizontally, and between the full height anchored at the down corner or at the
// centre vertically.
void grCollisionTestRhombus::ajustShape() {
    float width = m_cornerOffsetRight.m_x - m_cornerOffsetLeft.m_x;
    float height = m_cornerOffsetUp.m_y - m_cornerOffsetDown.m_y;
    float curWidth = m_unk2bc->getWidth();
    float curHeight = m_unk2bc->getHeight();
    if (m_unk2E4.m_x <= m_shape1.m_rhombus.m_leftPos.m_x) {
        m_unk2E4.m_x = m_shape1.m_rhombus.m_leftPos.m_x + 0.1f;
    } else if (m_shape1.m_rhombus.m_rightPos.m_x <= m_unk2E4.m_x) {
        m_unk2E4.m_x = m_shape1.m_rhombus.m_rightPos.m_x - 0.1f;
    }
    if (m_unk2E4.m_y <= m_shape1.m_rhombus.m_downPos.m_y) {
        m_unk2E4.m_y = m_shape1.m_rhombus.m_downPos.m_y + 0.1f;
    } else if (m_shape1.m_rhombus.m_upPos.m_y <= m_unk2E4.m_y) {
        m_unk2E4.m_y = m_shape1.m_rhombus.m_upPos.m_y - 0.1f;
    }
    clRhombus2D* rhombus = &m_shape1.m_rhombus;
    Vec2f center = m_unk2bc->getCenterPos();
    Vec2f space;
    calcSpace(&center, &space);
    // MATCH-ONLY: both branches of this comparison are identical in the original.
    if (curWidth < width) {
        if (width <= space.m_x - 0.02f) {
            rhombus->m_rightPos = rhombus->m_centerPos + m_cornerOffsetRight;
            rhombus->m_leftPos = rhombus->m_centerPos + m_cornerOffsetLeft;
        } else {
            calcShapeMin(rhombus);
        }
    } else {
        if (width <= space.m_x - 0.02f) {
            rhombus->m_rightPos = rhombus->m_centerPos + m_cornerOffsetRight;
            rhombus->m_leftPos = rhombus->m_centerPos + m_cornerOffsetLeft;
        } else {
            calcShapeMin(rhombus);
        }
    }
    if (curHeight < height) {
        if (height <= space.m_y - 0.02f) {
            if (m_collStatus->m_setting == grCollStatus::Setting_Ground) {
                float y = rhombus->m_downPos.m_y - m_cornerOffsetDown.m_y;
                rhombus->m_centerPos.m_y = y;
                rhombus->m_rightPos.m_y = y;
                rhombus->m_upPos.m_y = y + m_cornerOffsetUp.m_y;
                rhombus->m_leftPos.m_y = y;
            } else {
                float y = rhombus->m_centerPos.m_y;
                rhombus->m_rightPos.m_y = y;
                rhombus->m_leftPos.m_y = y;
                rhombus->m_downPos.m_y = y + m_cornerOffsetDown.m_y;
                rhombus->m_upPos.m_y = y + m_cornerOffsetUp.m_y;
            }
        }
    } else {
        float y = rhombus->m_downPos.m_y - m_cornerOffsetDown.m_y;
        rhombus->m_centerPos.m_y = y;
        rhombus->m_rightPos.m_y = y;
        rhombus->m_upPos.m_y = y + m_cornerOffsetUp.m_y;
        rhombus->m_leftPos.m_y = y;
    }
}

struct RhombusWords {
    u32 m_words[sizeof(clRhombus2D) / 4];
};

// Remembers the rhombus before the static correction (m_savedRhombus = its position before any correction moved it).
void grCollisionTestRhombus::testStaticCorrectionSetup() {
    // MATCH-ONLY: the original copies the rhombus as plain words.
    *(RhombusWords*)&m_savedRhombus = *(RhombusWords*)&m_shape1.m_rhombus;
}

// The static (no sweep) correction of the rhombus against one line: finds out which edges of the rhombus the line crosses
// (checkIntersect) and lets the matching direction function push the rhombus out of the line. Returns 1 when it was moved.
int grCollisionTestRhombus::testStaticCorrection(grCollisionLineWork* lineWork) {
    grCollSegmentEx segment;
    grCollSegmentEx preSegment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    fn_80111AF4(lineWork->m_collision, &preSegment, lineWork->m_collisionLine, 0);
    grCollisionLineWork prev;
    grCollisionLineWork next;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    u8 hitInfo = 0;
    int moved = 0;
    bool ignored = true;
    if (isIgnoreLine(lineWork) != 1) {
        ignored = false;
        hitInfo = checkIntersect(&m_shape1.m_rhombus, &segment.m_segment);
        if (hitInfo != 0) {
            grCollisionLine* line = lineWork->m_collisionLine;
            if (line->m_isRight && !(lineWork->m_unkC & 2)) {
                moved = testStaticRight(lineWork, hitInfo);
            } else if (line->m_isLeft && !(lineWork->m_unkC & 1)) {
                moved = testStaticLeft(lineWork, hitInfo);
            } else if (line->m_isUpper && !(lineWork->m_unkC & 4)) {
                moved = testStaticUp(lineWork, hitInfo);
            } else if (line->m_isUnder && !(lineWork->m_unkC & 8)) {
                moved = testStaticDown(lineWork, hitInfo);
            }
        }
    }
    // A floor the rhombus touched without being pushed: it is ignored from now on unless it is the line directly below the
    // down corner and not a dead end between neighbours.
    if (hitInfo != 0 && moved == 0 && lineWork->m_collisionLine->m_isUnder) {
        grCollisionLineWork below;
        grCollisionLineWork neighbor;
        bool ignore = true;
        if (!lineWork->getPosXDownLine(m_shape1.m_rhombus.m_downPos.m_x, &below)) {
            ignore = false;
        } else if (lineWork->m_collisionLine == below.m_collisionLine) {
            clSegment2D belowSegment;
            below.m_collision->getSegment(&belowSegment, below.m_collisionLine);
            Vec2f projected;
            if (belowSegment.projectionX(m_shape1.m_rhombus.m_downPos, projected) && projected.m_y < m_shape1.m_rhombus.m_downPos.m_y) {
                ignore = false;
            }
            if (lineWork->getLeftDownLine(&below) && below.getPrev(&neighbor)) {
                ignore = false;
            } else if (lineWork->getRightDownLine(&below) && below.getNext(&neighbor)) {
                ignore = false;
            }
        } else {
            ignore = false;
        }
        if (ignore) {
            fn_80137074(m_collStatus, lineWork);
        }
    }
    // A ceiling the rhombus did not overlap: if the up corner has just passed through it (the previous-vertex trace shows it
    // was below), push the rhombus down by the missing amount.
    if (hitInfo == 0 && lineWork->m_collisionLine->m_isUpper) {
        clSegment2D line;
        clSegment2D trace;
        fn_80111A38(lineWork->m_collision, &line, &trace, lineWork->m_collisionLine);
        Vec2f up = m_shape1.m_rhombus.m_upPos;
        if (line.m_p1Pos.m_x <= up.m_x && up.m_x <= line.m_p0Pos.m_x) {
            Vec2f projected;
            if (line.projectionX(up, projected) && projected.m_y <= up.m_y) {
                bool crossed = false;
                if (up.m_x < trace.m_p1Pos.m_x || trace.m_p0Pos.m_x < up.m_x) {
                    if (trace.m_p1Pos.m_x <= up.m_x) {
                        if (up.m_y < trace.m_p0Pos.m_y) {
                            crossed = true;
                        }
                    } else if (up.m_y < trace.m_p1Pos.m_y) {
                        crossed = true;
                    }
                } else {
                    Vec2f traceProjected;
                    if (trace.projectionX(up, traceProjected) && up.m_y <= traceProjected.m_y) {
                        crossed = true;
                    }
                }
                if (crossed) {
                    Vec2f push;
                    push = (projected - m_shape1.m_rhombus.m_upPos) + Vec2f(0.0f, -0.01f);
                    if (push.m_y < 0.0f) {
                        m_shape1.m_rhombus.move(push);
                        bool record = false;
                        if (!(m_unk2CA & SIDE_UP)) {
                            record = true;
                        } else if (m_shape1.m_rhombus.m_upPos.m_y < m_unk2EC.m_y) {
                            record = true;
                        }
                        if (record) {
                            moved = 1;
                            m_unk2CA |= SIDE_UP;
                            m_unk2EC = m_shape1.m_rhombus.m_upPos;
                            m_unk30C = *lineWork;
                        }
                    }
                }
            }
        }
    }
    if (moved != 1 && lineWork->m_collisionLine->m_isUnder && !(lineWork->m_unkC & 8)) {
        grCollStatus::Setting setting = m_collStatus->m_setting;
        if (setting == grCollStatus::Setting_Ground && lineWork->m_collisionLine == m_touch[3].m_lineWork.m_collisionLine) {
            fn_801371D8(m_collStatus, lineWork);
        } else if (!ignored && (setting != grCollStatus::Setting_Ground || lineWork->m_collisionLine != m_unk224.m_collisionLine) &&
                   testStaticChangeDownLineCheck(lineWork) != 0) {
            moved = 1;
        }
    }
    return moved;
}

// MATCH-ONLY: the original takes the maximum of two floats with a paired-single subtract and select.
static inline float psMax(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, a, b
    }
    // clang-format on
    return result;
}

static inline float psMin(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, b, a
    }
    // clang-format on
    return result;
}

// Pushes the right corner (and with it the rhombus) out of a wall line on its right. flags = checkIntersect's result.
// The wall is tested at the right corner first, then (for an inner neighbour floor / ceiling) at the down / up corners.
// Returns 1 when the rhombus was moved.
int grCollisionTestRhombus::testStaticRight(grCollisionLineWork* lineWork, u8 flags) {
    grCollisionLineWork prev;
    grCollisionLineWork next;
    int moved = 0;
    clRhombus2D* rhombus = &m_shape1.m_rhombus;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    grCollSegmentEx wall;
    fn_80111968(lineWork->m_collision, &wall, lineWork->m_collisionLine, 0);
    Vec2f projected;
    Vec2f push;
    if (wall.m_segment.m_p0Pos.m_y <= rhombus->m_rightPos.m_y && rhombus->m_rightPos.m_y <= wall.m_segment.m_p1Pos.m_y &&
        wall.m_segment.projectionY(rhombus->m_rightPos, projected)) {
        bool hit = false;
        if (m_unk2E4.m_x < projected.m_x) {
            hit = true;
        } else {
            clSegment2D pre;
            fn_801119F8(lineWork->m_collision, &pre, lineWork->m_collisionLine);
            if (rhombus->m_rightPos.m_y < pre.m_p0Pos.m_y || pre.m_p1Pos.m_y < rhombus->m_rightPos.m_y) {
                if (pre.m_p0Pos.m_y <= rhombus->m_rightPos.m_y) {
                    if (rhombus->m_rightPos.m_x < pre.m_p1Pos.m_x) {
                        hit = true;
                    }
                } else if (rhombus->m_rightPos.m_x < pre.m_p0Pos.m_x) {
                    hit = true;
                }
            } else {
                Vec2f preProjected;
                if (pre.projectionY(rhombus->m_rightPos, preProjected) && rhombus->m_rightPos.m_x < preProjected.m_x) {
                    hit = true;
                }
            }
        }
        if (hit) {
            push = (projected - rhombus->m_rightPos) + Vec2f(-0.01f, 0.0f);
            if (push.m_x < 0.0f) {
                rhombus->move(push);
                moved = 1;
                if (m_unk2CA & SIDE_RIGHT) {
                    if (rhombus->m_rightPos.m_x < m_unk304.m_x) {
                        m_unk304 = rhombus->m_rightPos;
                        updateTouchLine(2, lineWork, 1.0f);
                    }
                } else {
                    m_unk2CA |= SIDE_RIGHT;
                    m_unk304 = rhombus->m_rightPos;
                    updateTouchLine(2, lineWork, 1.0f);
                }
            }
        }
    }
    if (wall.m_segment.projectionY(rhombus->m_centerPos, projected) && projected.m_x <= psMin(rhombus->m_centerPos.m_x, m_unk2E4.m_x)) {
        return moved;
    }
    // the wall's lower end continues into a floor on the next line: slide the right corner along the edge down corner -> right corner
    if (isActiveLineWork(&next) && next.m_collisionLine->m_isUnder && rhombus->m_downPos.m_y <= wall.m_segment.m_p1Pos.m_y &&
        wall.m_segment.m_p1Pos.m_y <= rhombus->m_rightPos.m_y) {
        clSegment2D edge(rhombus->m_downPos, rhombus->m_rightPos);
        bool hit = false;
        if (rhombus->m_rightPos.m_y < wall.m_segment.m_p1Pos.m_y || !edge.projectionY(wall.m_segment.m_p1Pos, projected)) {
            if (wall.m_segment.projectionY(rhombus->m_rightPos, projected)) {
                hit = true;
                push = (projected - rhombus->m_rightPos) + Vec2f(-0.01f, 0.0f);
            }
        } else {
            hit = true;
            push = (wall.m_segment.m_p1Pos - projected) + Vec2f(-0.01f, 0.0f);
        }
        if (hit && push.m_x < 0.0f) {
            rhombus->move(push);
            moved = 1;
            if (m_unk2CA & SIDE_RIGHT) {
                if (rhombus->m_rightPos.m_x < m_unk304.m_x) {
                    m_unk304 = rhombus->m_rightPos;
                }
            } else {
                m_unk2CA |= SIDE_RIGHT;
                m_unk304 = rhombus->m_rightPos;
            }
        }
    }
    // the wall's upper end continues into a ceiling on the previous line: same along the edge right corner -> up corner
    if (isActiveLineWork(&prev) && prev.m_collisionLine->m_isUpper) {
        clSegment2D edge(rhombus->m_rightPos, rhombus->m_upPos);
        if (rhombus->m_rightPos.m_y <= wall.m_segment.m_p0Pos.m_y && wall.m_segment.m_p0Pos.m_y <= rhombus->m_upPos.m_y) {
            bool hit = false;
            if (wall.m_segment.m_p0Pos.m_y < rhombus->m_rightPos.m_y || !edge.projectionY(wall.m_segment.m_p0Pos, projected)) {
                if (wall.m_segment.projectionY(rhombus->m_rightPos, projected)) {
                    hit = true;
                    push = (projected - rhombus->m_rightPos) + Vec2f(-0.01f, 0.0f);
                }
            } else {
                hit = true;
                push = (wall.m_segment.m_p0Pos - projected) + Vec2f(-0.01f, 0.0f);
            }
            if (hit && push.m_x < 0.0f) {
                rhombus->move(push);
                moved = 1;
                if (m_unk2CA & SIDE_RIGHT) {
                    if (rhombus->m_rightPos.m_x < m_unk304.m_x) {
                        m_unk304 = rhombus->m_rightPos;
                    }
                } else {
                    m_unk2CA |= SIDE_RIGHT;
                    m_unk304 = rhombus->m_rightPos;
                }
            }
        }
    }
    if (flags & 1) {
        // the next line is a ceiling: the up corner must not end up beyond the wall, nor above the ceiling
        if (isActiveLineWork(&next) && next.m_collisionLine->m_isUpper) {
            if (wall.m_segment.projectionY(rhombus->m_upPos, projected)) {
                push = (projected - rhombus->m_upPos) + Vec2f(-0.01f, 0.0f);
                if (push.m_x < 0.0f) {
                    rhombus->move(push);
                    moved = 1;
                    if (m_unk2CA & SIDE_RIGHT) {
                        if (rhombus->m_rightPos.m_x < m_unk304.m_x) {
                            m_unk304 = rhombus->m_rightPos;
                        }
                    } else {
                        m_unk304 = rhombus->m_rightPos;
                    }
                    m_unk2CA |= SIDE_RIGHT;
                }
            }
            grCollSegmentEx ceiling;
            fn_80111968(next.m_collision, &ceiling, next.m_collisionLine, 0);
            if (ceiling.m_segment.m_p1Pos.m_x <= rhombus->m_upPos.m_x && ceiling.m_segment.projectionX(rhombus->m_upPos, projected)) {
                push = (projected - rhombus->m_upPos) + Vec2f(0.0f, -0.01f);
                if (push.m_y < 0.0f) {
                    rhombus->move(push);
                    moved = 1;
                    if (!(m_unk2CA & SIDE_UP)) {
                        m_unk2EC = rhombus->m_upPos;
                        m_unk30C = *lineWork;
                    } else if (rhombus->m_upPos.m_y < m_unk2EC.m_y) {
                        m_unk2EC = rhombus->m_upPos;
                        m_unk30C = *lineWork;
                    }
                    m_unk2CA |= SIDE_UP;
                }
            }
        }
    }
    // the wall's previous line is a floor the rhombus can stand on: keep the down corner above it
    if (isActiveLineWork(&prev) && prev.m_collisionLine->m_isUnder && (prev.m_collisionLine->m_unkE & m_exclusiveLineAttr) == 0 &&
        fn_80137114(m_collStatus, &prev) == 0) {
        grCollSegmentEx floor;
        fn_80111968(prev.m_collision, &floor, prev.m_collisionLine, 0);
        if (floor.m_segment.projectionX(rhombus->m_centerPos, projected) && projected.m_y < rhombus->m_centerPos.m_y) {
            if (wall.m_segment.projectionY(rhombus->m_downPos, projected) && rhombus->m_leftPos.m_x < projected.m_x) {
                push = (projected - rhombus->m_downPos) + Vec2f(-0.01f, 0.0f);
                if (push.m_x < 0.0f) {
                    rhombus->move(push);
                    moved = 1;
                    if (m_unk2CA & SIDE_RIGHT) {
                        if (rhombus->m_rightPos.m_x < m_unk304.m_x) {
                            m_unk304 = rhombus->m_rightPos;
                        }
                    } else {
                        m_unk2CA |= SIDE_RIGHT;
                        m_unk304 = rhombus->m_rightPos;
                    }
                }
            }
            if (floor.m_segment.m_p0Pos.m_x <= rhombus->m_downPos.m_x && floor.m_segment.projectionX(rhombus->m_downPos, projected)) {
                push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
                if (0.0f < push.m_y) {
                    push.m_x = (floor.m_segment.m_p1Pos.m_x - rhombus->m_downPos.m_x) - 0.01f;
                    if (0.0f < push.m_x) {
                        push.m_x = 0.0f;
                    }
                    rhombus->move(push);
                    moved = 1;
                    if (!(m_unk2CA & SIDE_DOWN)) {
                        m_unk2CA |= SIDE_DOWN;
                        m_unk2F4 = rhombus->m_downPos;
                        m_unk31C = prev;
                    } else if (m_unk2F4.m_y < rhombus->m_downPos.m_y) {
                        m_unk2F4 = rhombus->m_downPos;
                        m_unk31C = prev;
                    }
                }
            }
        }
    }
    return moved;
}

// Mirror image of testStaticRight for a wall line on the left of the rhombus: pushes the left corner out of it (the prev / next
// roles of the neighbours are swapped). flags = checkIntersect's result (bit 3 is tested). Returns 1 when the rhombus was moved.
int grCollisionTestRhombus::testStaticLeft(grCollisionLineWork* lineWork, u8 flags) {
    grCollisionLineWork prev;
    grCollisionLineWork next;
    int moved = 0;
    clRhombus2D* rhombus = &m_shape1.m_rhombus;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    grCollSegmentEx wall;
    fn_80111968(lineWork->m_collision, &wall, lineWork->m_collisionLine, 0);
    Vec2f projected;
    Vec2f push;
    if (wall.m_segment.m_p1Pos.m_y <= rhombus->m_leftPos.m_y && rhombus->m_leftPos.m_y <= wall.m_segment.m_p0Pos.m_y &&
        wall.m_segment.projectionY(rhombus->m_leftPos, projected)) {
        bool hit = false;
        if (m_unk2E4.m_x <= projected.m_x) {
            grCollSegmentEx pre;
            fn_80111AF4(lineWork->m_collision, &pre, lineWork->m_collisionLine, 0);
            if (pre.m_segment.m_p0Pos.m_y < rhombus->m_leftPos.m_y || rhombus->m_leftPos.m_y < pre.m_segment.m_p1Pos.m_y) {
                if (rhombus->m_leftPos.m_y <= pre.m_segment.m_p0Pos.m_y) {
                    if (pre.m_segment.m_p1Pos.m_x < rhombus->m_leftPos.m_x) {
                        hit = true;
                    }
                } else if (pre.m_segment.m_p0Pos.m_x < rhombus->m_leftPos.m_x) {
                    hit = true;
                }
            } else {
                Vec2f preProjected;
                if (pre.m_segment.projectionY(rhombus->m_leftPos, preProjected) && preProjected.m_x < rhombus->m_leftPos.m_x) {
                    hit = true;
                }
            }
        } else {
            hit = true;
        }
        if (hit) {
            push = (projected - rhombus->m_leftPos) + Vec2f(0.01f, 0.0f);
            if (0.0f < push.m_x) {
                rhombus->move(push);
                moved = 1;
                if (!(m_unk2CA & SIDE_LEFT)) {
                    m_unk2CA |= SIDE_LEFT;
                    m_unk2FC = rhombus->m_leftPos;
                    updateTouchLine(1, lineWork, 1.0f);
                } else if (m_unk2FC.m_x < rhombus->m_leftPos.m_x) {
                    m_unk2FC = rhombus->m_leftPos;
                    updateTouchLine(1, lineWork, 1.0f);
                }
            }
        }
    }
    if (wall.m_segment.projectionY(rhombus->m_centerPos, projected) && psMax(rhombus->m_centerPos.m_x, m_unk2E4.m_x) <= projected.m_x) {
        return moved;
    }
    // the wall's upper end continues into a floor on the previous line: slide the left corner along the edge down -> left corner
    if (isActiveLineWork(&prev) && prev.m_collisionLine->m_isUnder && rhombus->m_downPos.m_y <= wall.m_segment.m_p0Pos.m_y &&
        wall.m_segment.m_p0Pos.m_y <= rhombus->m_leftPos.m_y) {
        clSegment2D edge(rhombus->m_downPos, rhombus->m_leftPos);
        bool hit = false;
        if (rhombus->m_leftPos.m_y < wall.m_segment.m_p0Pos.m_y || !edge.projectionY(wall.m_segment.m_p0Pos, projected)) {
            if (wall.m_segment.projectionY(rhombus->m_leftPos, projected)) {
                hit = true;
                push = (projected - rhombus->m_leftPos) + Vec2f(0.01f, 0.0f);
            }
        } else {
            hit = true;
            push = (wall.m_segment.m_p0Pos - projected) + Vec2f(0.01f, 0.0f);
        }
        if (hit && 0.0f < push.m_x) {
            rhombus->move(push);
            moved = 1;
            if (!(m_unk2CA & SIDE_LEFT)) {
                m_unk2FC = rhombus->m_leftPos;
                m_unk2CA |= SIDE_LEFT;
            } else if (m_unk2FC.m_x < rhombus->m_leftPos.m_x) {
                m_unk2FC = rhombus->m_leftPos;
            }
        }
    }
    // the wall's lower end continues into a ceiling on the next line: same along the edge left corner -> up corner
    if (isActiveLineWork(&next) && next.m_collisionLine->m_isUpper) {
        clSegment2D edge(rhombus->m_leftPos, rhombus->m_upPos);
        if (rhombus->m_leftPos.m_y <= wall.m_segment.m_p1Pos.m_y && wall.m_segment.m_p1Pos.m_y <= rhombus->m_upPos.m_y) {
            bool hit = false;
            if (wall.m_segment.m_p1Pos.m_y < rhombus->m_leftPos.m_y || !edge.projectionY(wall.m_segment.m_p1Pos, projected)) {
                if (wall.m_segment.projectionY(rhombus->m_leftPos, projected)) {
                    hit = true;
                    push = (projected - rhombus->m_leftPos) + Vec2f(0.01f, 0.0f);
                    if (0.0f < push.m_x) {
                        rhombus->move(push);
                        moved = 1;
                        if (!(m_unk2CA & SIDE_LEFT)) {
                            m_unk2FC = rhombus->m_leftPos;
                        } else if (rhombus->m_leftPos.m_x < m_unk2FC.m_x) {
                            m_unk2FC = rhombus->m_leftPos;
                        }
                        m_unk2CA |= SIDE_LEFT;
                    }
                }
            } else {
                push = (wall.m_segment.m_p1Pos - projected) + Vec2f(0.01f, 0.0f);
                if (0.0f < push.m_x) {
                    rhombus->move(push);
                    moved = 1;
                    if (!(m_unk2CA & SIDE_LEFT)) {
                        m_unk2FC = rhombus->m_leftPos;
                    } else if (rhombus->m_leftPos.m_x < m_unk2FC.m_x) {
                        m_unk2FC = rhombus->m_leftPos;
                    }
                    m_unk2CA |= SIDE_LEFT;
                }
            }
        }
    }
    if (flags & 8) {
        // the previous line is a ceiling: the up corner must not end up beyond the wall, nor above the ceiling
        if (isActiveLineWork(&prev) && prev.m_collisionLine->m_isUpper) {
            if (wall.m_segment.projectionY(rhombus->m_upPos, projected)) {
                push = (projected - rhombus->m_upPos) + Vec2f(0.01f, 0.0f);
                if (0.0f < push.m_x) {
                    rhombus->move(push);
                    moved = 1;
                    if (!(m_unk2CA & SIDE_LEFT)) {
                        m_unk2FC = rhombus->m_leftPos;
                    } else if (m_unk2FC.m_x < rhombus->m_leftPos.m_x) {
                        m_unk2FC = rhombus->m_leftPos;
                    }
                    m_unk2CA |= SIDE_LEFT;
                }
            }
            grCollSegmentEx ceiling;
            fn_80111968(prev.m_collision, &ceiling, prev.m_collisionLine, 0);
            if (rhombus->m_upPos.m_x <= ceiling.m_segment.m_p0Pos.m_x && ceiling.m_segment.projectionX(rhombus->m_upPos, projected)) {
                push = (projected - rhombus->m_upPos) + Vec2f(0.0f, -0.01f);
                if (push.m_y < 0.0f) {
                    rhombus->move(push);
                    moved = 1;
                    if (!(m_unk2CA & SIDE_UP)) {
                        m_unk2EC = rhombus->m_upPos;
                        m_unk30C = *lineWork;
                    } else if (rhombus->m_upPos.m_y < m_unk2EC.m_y) {
                        m_unk2EC = rhombus->m_upPos;
                        m_unk30C = *lineWork;
                    }
                    m_unk2CA |= SIDE_UP;
                }
            }
        }
    }
    // the next line is a floor the rhombus can stand on: keep the down corner above it
    if (isActiveLineWork(&next) && next.m_collisionLine->m_isUnder && (next.m_collisionLine->m_unkE & m_exclusiveLineAttr) == 0 &&
        fn_80137114(m_collStatus, &next) == 0) {
        grCollSegmentEx floor;
        fn_80111968(next.m_collision, &floor, next.m_collisionLine, 0);
        if (floor.m_segment.projectionX(rhombus->m_centerPos, projected) && projected.m_y < rhombus->m_centerPos.m_y) {
            if (wall.m_segment.projectionY(rhombus->m_downPos, projected) && projected.m_x < rhombus->m_rightPos.m_x) {
                push = (projected - rhombus->m_downPos) + Vec2f(0.01f, 0.0f);
                if (0.0f < push.m_x) {
                    rhombus->move(push);
                    moved = 1;
                    if (!(m_unk2CA & SIDE_LEFT)) {
                        m_unk2CA |= SIDE_LEFT;
                        m_unk2FC = rhombus->m_leftPos;
                    } else if (m_unk2FC.m_x < rhombus->m_leftPos.m_x) {
                        m_unk2FC = rhombus->m_leftPos;
                    }
                }
            }
            if (rhombus->m_downPos.m_x <= floor.m_segment.m_p1Pos.m_x && floor.m_segment.projectionX(rhombus->m_downPos, projected)) {
                push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
                if (0.0f < push.m_y) {
                    push.m_x = (floor.m_segment.m_p0Pos.m_x - rhombus->m_downPos.m_x) + 0.01f;
                    if (push.m_x < 0.0f) {
                        push.m_x = 0.0f;
                    }
                    rhombus->move(push);
                    moved = 1;
                    if (!(m_unk2CA & SIDE_DOWN)) {
                        m_unk2CA |= SIDE_DOWN;
                        m_unk2F4 = rhombus->m_downPos;
                        m_unk31C = next;
                    } else if (m_unk2F4.m_y < rhombus->m_downPos.m_y) {
                        m_unk2F4 = rhombus->m_downPos;
                        m_unk31C = next;
                    }
                }
            }
        }
    }
    return moved;
}

// Pushes the up corner out of a ceiling line (when the rhombus did not pass through it from above). flags is not read.
// While in the air (and not in the last-correction pass) a ceiling that moved over the corner this frame also counts.
// Returns 1 when the rhombus was moved.
int grCollisionTestRhombus::testStaticUp(grCollisionLineWork* lineWork, u8 flags) {
    int moved = 0;
    clRhombus2D* rhombus = &m_shape1.m_rhombus;
    grCollSegmentEx wall;
    grCollSegmentEx pre;
    Vec2f projected;
    Vec2f push;
    fn_80111968(lineWork->m_collision, &wall, lineWork->m_collisionLine, 0);
    fn_80111AF4(lineWork->m_collision, &pre, lineWork->m_collisionLine, 0);
    if (!m_unkC_6 && m_collStatus->m_setting == grCollStatus::Setting_Air) {
        bool unchanged = false;
        if (wall.m_segment.m_p0Pos.m_x == pre.m_segment.m_p0Pos.m_x && wall.m_segment.m_p0Pos.m_y == pre.m_segment.m_p0Pos.m_y &&
            wall.m_segment.m_p1Pos.m_x == pre.m_segment.m_p1Pos.m_x && wall.m_segment.m_p1Pos.m_y == pre.m_segment.m_p1Pos.m_y) {
            unchanged = true;
        }
        if (!unchanged) {
            clQuad2D quad;
            quad.set(pre.m_segment, wall.m_segment);
            if (quad.checkInclude(rhombus->m_upPos) && wall.m_segment.projectionX(rhombus->m_upPos, projected)) {
                push = (projected - rhombus->m_upPos) + Vec2f(0.0f, 0.01f);
                if (0.0f < push.m_y) {
                    rhombus->move(push);
                    moved = 1;
                    if (!(m_unk2CA & SIDE_UP)) {
                        m_unk2CA |= SIDE_UP;
                        m_unk2EC = rhombus->m_upPos;
                        m_unk30C = *lineWork;
                    } else if (rhombus->m_upPos.m_y < m_unk2EC.m_y) {
                        m_unk2EC = rhombus->m_upPos;
                        m_unk30C = *lineWork;
                    }
                }
            }
        }
    }
    if (wall.m_segment.m_p1Pos.m_x <= rhombus->m_upPos.m_x && rhombus->m_upPos.m_x <= wall.m_segment.m_p0Pos.m_x &&
        wall.m_segment.projectionX(rhombus->m_upPos, projected)) {
        bool hit = false;
        if (projected.m_y < rhombus->m_centerPos.m_y) {
            if (rhombus->m_upPos.m_x < pre.m_segment.m_p1Pos.m_x || pre.m_segment.m_p0Pos.m_x < rhombus->m_upPos.m_x) {
                if (pre.m_segment.m_p1Pos.m_x <= rhombus->m_upPos.m_x) {
                    if (rhombus->m_upPos.m_y < pre.m_segment.m_p0Pos.m_y) {
                        hit = true;
                    }
                } else if (rhombus->m_upPos.m_y < pre.m_segment.m_p1Pos.m_y) {
                    hit = true;
                }
            } else {
                Vec2f preProjected;
                if (pre.m_segment.projectionX(rhombus->m_upPos, preProjected)) {
                    if (rhombus->m_upPos.m_y <= preProjected.m_y) {
                        hit = true;
                    }
                }
            }
        } else {
            hit = true;
        }
        if (hit) {
            push = (projected - rhombus->m_upPos) + Vec2f(0.0f, -0.01f);
            if (push.m_y < 0.0f) {
                rhombus->move(push);
                moved = 1;
                bool record = false;
                if (!(m_unk2CA & SIDE_UP)) {
                    record = true;
                } else if (rhombus->m_upPos.m_y < m_unk2EC.m_y) {
                    record = true;
                }
                if (record) {
                    m_unk2CA |= SIDE_UP;
                    m_unk2EC = rhombus->m_upPos;
                    m_unk30C = *lineWork;
                }
            }
        }
    }
    return moved;
}

// Pushes the down corner up out of a floor line (standing on it). Returns 1 when the rhombus was moved.
// 1) while not in the last-correction pass: the floor just reached the corner from below (swept by the corner since last frame);
// 2) otherwise the floor directly below the saved (pre-correction) down corner; 3) in the last-correction pass: a floor the
// previous down corner was above; 4) on the ground with a wall-side hit: the floor under the corner between its neighbours.
int grCollisionTestRhombus::testStaticDown(grCollisionLineWork* lineWork, u8 flags) {
    int moved = 0;
    clRhombus2D* rhombus = &m_shape1.m_rhombus;
    grCollSegmentEx wall;
    grCollSegmentEx pre;
    fn_80111968(lineWork->m_collision, &wall, lineWork->m_collisionLine, 0);
    fn_80111AF4(lineWork->m_collision, &pre, lineWork->m_collisionLine, 0);
    Vec2f projected;
    Vec2f push;
    grCollisionLineWork neighbor;
    grCollisionLineWork found;
    if (!m_unkC_6) {
        bool reach = false;
        if (!wall.m_segment.projectionX(rhombus->m_downPos, projected) || rhombus->m_downPos.m_y - 0.01f <= projected.m_y) {
            bool unchanged = false;
            if (wall.m_segment.m_p0Pos.m_x == pre.m_segment.m_p0Pos.m_x && wall.m_segment.m_p0Pos.m_y == pre.m_segment.m_p0Pos.m_y &&
                wall.m_segment.m_p1Pos.m_x == pre.m_segment.m_p1Pos.m_x && wall.m_segment.m_p1Pos.m_y == pre.m_segment.m_p1Pos.m_y) {
                unchanged = true;
            }
            if (unchanged) {
                if (wall.m_segment.m_p0Pos.m_x <= rhombus->m_downPos.m_x && rhombus->m_downPos.m_x <= wall.m_segment.m_p1Pos.m_x &&
                    wall.m_segment.projectionX(rhombus->m_downPos, projected) && (float)__fabs(rhombus->m_downPos.m_y - projected.m_y) <= 0.02f) {
                    reach = true;
                }
            } else {
                clQuad2D quad;
                quad.set(pre.m_segment, wall.m_segment);
                if (!quad.checkInclude(rhombus->m_downPos)) {
                    float downX = rhombus->m_downPos.m_x;
                    float downY = rhombus->m_downPos.m_y;
                    Vec2f below;
                    if (wall.m_segment.m_p0Pos.m_x <= downX && downX <= wall.m_segment.m_p1Pos.m_x &&
                        wall.m_segment.projectionX(rhombus->m_downPos, below) && downY - 0.01f <= below.m_y) {
                        Vec2f preProjected;
                        if (downX < pre.m_segment.m_p0Pos.m_x || pre.m_segment.m_p1Pos.m_x < downX) {
                            if (pre.m_segment.m_p0Pos.m_x <= downX) {
                                if (downY < pre.m_segment.m_p1Pos.m_y) {
                                    grCollisionLineWork edgeNext;
                                    if (lineWork->getNext(&edgeNext) && edgeNext.m_collisionLine->m_isLeft) {
                                        reach = true;
                                    }
                                } else {
                                    reach = true;
                                }
                            } else if (downY < pre.m_segment.m_p0Pos.m_y) {
                                grCollisionLineWork edgePrev;
                                if (lineWork->getPrev(&edgePrev) && edgePrev.m_collisionLine->m_isRight) {
                                    reach = true;
                                }
                            } else {
                                reach = true;
                            }
                        } else {
                            if (!pre.m_segment.projectionX(rhombus->m_downPos, preProjected) || downY < preProjected.m_y) {
                                if (below.m_y < rhombus->m_centerPos.m_y) {
                                    reach = true;
                                }
                            } else {
                                reach = true;
                            }
                        }
                        if (!reach && lineWork->getPosXDownLinePre(m_savedRhombus.m_downPos.m_x, &found)) {
                            clSegment2D foundPre;
                            fn_801119F8(found.m_collision, &foundPre, found.m_collisionLine);
                            if (pre.m_segment.projectionX(m_savedRhombus.m_downPos, preProjected) &&
                                preProjected.m_y < m_savedRhombus.m_downPos.m_y) {
                                reach = true;
                            }
                        }
                    }
                } else {
                    reach = true;
                }
            }
        }
        if (reach && wall.m_segment.projectionX(rhombus->m_downPos, projected) && projected.m_y < m_unk2EC.m_y - 0.01f) {
            push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
            if (0.0f < push.m_y) {
                rhombus->move(push);
                moved = 1;
                if (!(m_unk2CA & SIDE_DOWN)) {
                    m_unk2CA |= SIDE_DOWN;
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                } else if (m_unk2F4.m_y < rhombus->m_downPos.m_y) {
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                }
            }
        }
    }
    if (moved == 0 && wall.m_segment.m_p0Pos.m_x <= rhombus->m_downPos.m_x && rhombus->m_downPos.m_x <= wall.m_segment.m_p1Pos.m_x &&
        wall.m_segment.projectionX(rhombus->m_downPos, projected) && rhombus->m_downPos.m_y < projected.m_y &&
        lineWork->getPosXDownLine(m_savedRhombus.m_downPos.m_x, &found)) {
        clSegment2D foundSegment;
        found.m_collision->getSegment(&foundSegment, found.m_collisionLine);
        Vec2f foundProjected;
        if (foundSegment.projectionX(m_savedRhombus.m_downPos, foundProjected) && foundProjected.m_y < m_savedRhombus.m_downPos.m_y) {
            push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
            if (0.0f < push.m_y) {
                rhombus->move(push);
                moved = 1;
                if (!(m_unk2CA & SIDE_DOWN)) {
                    m_unk2CA |= SIDE_DOWN;
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                } else if (m_unk2F4.m_y < rhombus->m_downPos.m_y) {
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                }
            }
        }
    }
    if (m_unkC_6 && !(rhombus->m_downPos.m_x < wall.m_segment.m_p0Pos.m_x || wall.m_segment.m_p1Pos.m_x < rhombus->m_downPos.m_x) &&
        wall.m_segment.projectionX(rhombus->m_downPos, projected) && rhombus->m_downPos.m_y < projected.m_y) {
        Vec2f prevDown = m_shape0.m_rhombus.m_downPos;
        bool reach = false;
        bool slide = false;
        if (wall.m_segment.m_p0Pos.m_x <= prevDown.m_x) {
            if (prevDown.m_x <= wall.m_segment.m_p1Pos.m_x) {
                Vec2f prevProjected;
                if (wall.m_segment.projectionX(prevDown, prevProjected) && prevProjected.m_y < prevDown.m_y) {
                    reach = true;
                } else {
                    slide = true;
                }
            } else if (prevDown.m_y <= wall.m_segment.m_p1Pos.m_y) {
                slide = true;
            } else {
                reach = true;
            }
        } else if (prevDown.m_y <= wall.m_segment.m_p0Pos.m_y) {
            slide = true;
        } else {
            reach = true;
        }
        if (slide) {
            if (projected.m_y <= rhombus->m_centerPos.m_y) {
                grCollisionLineWork edge;
                if (!lineWork->m_collisionLine->m_isThroughable) {
                    reach = true;
                } else if (!lineWork->getPrev(&edge) || !edge.m_collisionLine->m_isRight) {
                    if (lineWork->getNext(&edge) && edge.m_collisionLine->m_isRight) {
                        reach = true;
                    }
                } else {
                    reach = true;
                }
            }
        }
        if (reach) {
            push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
            if (0.0f < push.m_y) {
                rhombus->move(push);
                moved = 1;
                if (!(m_unk2CA & SIDE_DOWN)) {
                    m_unk2CA |= SIDE_DOWN;
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                } else if (m_unk2F4.m_y < rhombus->m_downPos.m_y) {
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                }
            }
        }
    }
    if (m_collStatus->m_setting == grCollStatus::Setting_Ground && (flags & (DIR_RIGHT | DIR_UP)) != 0) {
        float minX = wall.m_segment.m_p0Pos.m_x;
        float maxX = wall.m_segment.m_p1Pos.m_x;
        lineWork->getPrev(&neighbor);
        if (isActiveLineWork(&neighbor) && neighbor.m_collisionLine->m_isUnder) {
            grCollSegmentEx side;
            fn_80111968(neighbor.m_collision, &side, neighbor.m_collisionLine, 0);
            minX = side.m_segment.m_p1Pos.m_x;
        }
        lineWork->getNext(&neighbor);
        if (isActiveLineWork(&neighbor) && neighbor.m_collisionLine->m_isUnder) {
            grCollSegmentEx side;
            fn_80111968(neighbor.m_collision, &side, neighbor.m_collisionLine, 0);
            maxX = side.m_segment.m_p0Pos.m_x;
        }
        float limit = psMin(rhombus->m_downPos.m_y + 1.0f, rhombus->m_centerPos.m_y);
        if (minX <= rhombus->m_downPos.m_x && rhombus->m_downPos.m_x <= maxX && wall.m_segment.projectionX(rhombus->m_downPos, projected) &&
            projected.m_y < limit) {
            push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
            if (0.0f < push.m_y) {
                rhombus->move(push);
                moved = 1;
                if (!(m_unk2CA & SIDE_DOWN)) {
                    m_unk2CA |= SIDE_DOWN;
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                } else if (m_unk2F4.m_y < rhombus->m_downPos.m_y) {
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                }
                updateTouchLine(3, lineWork, 1.0f);
            }
        }
    }
    return moved;
}

// Called for a floor the down corner stands near after a line change: lets the rhombus follow a floor that moved up to the down
// corner (the quad between the line's previous and current position), or while in the air / on the ground re-seats the corner on
// the floor. Returns 1 when the rhombus was moved.
int grCollisionTestRhombus::testStaticChangeDownLineCheck(grCollisionLineWork* lineWork) {
    int moved = 0;
    clRhombus2D* rhombus = &m_shape1.m_rhombus;
    grCollSegmentEx wall;
    grCollSegmentEx pre;
    fn_80111968(lineWork->m_collision, &wall, lineWork->m_collisionLine, 0);
    fn_80111AF4(lineWork->m_collision, &pre, lineWork->m_collisionLine, 0);
    Vec2f startMove = wall.m_segment.m_p0Pos - pre.m_segment.m_p0Pos;
    Vec2f endMove = wall.m_segment.m_p1Pos - pre.m_segment.m_p1Pos;
    u8 lineMoved = 1.1920929e-07f < startMove.m_x * startMove.m_x + startMove.m_y * startMove.m_y;
    if (1.1920929e-07f < endMove.m_x * endMove.m_x + endMove.m_y * endMove.m_y) {
        lineMoved |= 8;
    }
    Vec2f shapeMove = m_shape2.m_rhombus.m_downPos - m_shape1.m_rhombus.m_downPos;
    Vec2f projected;
    Vec2f push;
    if (lineMoved != 0) {
        clQuad2D quad(pre.m_segment, wall.m_segment);
        if (quad.checkInclude(rhombus->m_downPos)) {
            Vec2f preProjected;
            pre.m_segment.projectionX(rhombus->m_downPos, preProjected);
            wall.m_segment.projectionX(rhombus->m_downPos, projected);
            if (rhombus->m_downPos.m_y <= psMax(preProjected.m_y, projected.m_y) &&
                rhombus->m_downPos.m_y + shapeMove.m_y <= projected.m_y && wall.m_segment.m_p0Pos.m_x <= projected.m_x &&
                projected.m_x <= wall.m_segment.m_p1Pos.m_x) {
                push = projected - rhombus->m_downPos;
                rhombus->move(push);
                m_shape2.m_rhombus.move(push);
                moved = 1;
                m_unk2CA |= SIDE_DOWN;
                m_unk2F4 = rhombus->m_downPos;
                m_unk31C = *lineWork;
                updateTouchLine(3, lineWork, 1.0f);
            }
        }
    }
    if (!m_unkC_6) {
        grCollStatus::Setting setting = m_collStatus->m_setting;
        if (setting == grCollStatus::Setting_Air) {
            wall.m_segment.projectionX(rhombus->m_downPos, projected);
            if (wall.m_segment.m_p0Pos.m_x <= rhombus->m_downPos.m_x && rhombus->m_downPos.m_x <= wall.m_segment.m_p1Pos.m_x &&
                projected.m_y < m_savedRhombus.m_downPos.m_y && rhombus->m_downPos.m_y < projected.m_y) {
                push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
                if (0.0f < push.m_y) {
                    rhombus->move(push);
                    moved = 1;
                    m_unk2CA |= SIDE_DOWN;
                    m_unk2F4 = rhombus->m_downPos;
                    m_unk31C = *lineWork;
                    updateTouchLine(3, lineWork, 1.0f);
                }
            }
        } else if (setting == grCollStatus::Setting_Ground) {
            if (isActiveLineWork(&m_unk224)) {
                wall.m_segment.projectionX(rhombus->m_downPos, projected);
                if (wall.m_segment.m_p0Pos.m_x <= rhombus->m_downPos.m_x && rhombus->m_downPos.m_x <= wall.m_segment.m_p1Pos.m_x) {
                    clSegment2D groundSegment;
                    clSegment2D groundTrace;
                    fn_80111A38(m_unk224.m_collision, &groundSegment, &groundTrace, m_unk224.m_collisionLine);
                    Vec2f groundProjected;
                    Vec2f traceProjected;
                    groundSegment.projectionX(rhombus->m_downPos, groundProjected);
                    groundTrace.projectionX(rhombus->m_downPos, traceProjected);
                    if (projected.m_y <= traceProjected.m_y && groundProjected.m_y < projected.m_y) {
                        push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
                        rhombus->move(push);
                        moved = 1;
                        m_unk2CA |= SIDE_DOWN;
                        m_unk2F4 = rhombus->m_downPos;
                        m_unk31C = *lineWork;
                        updateTouchLine(3, lineWork, 1.0f);
                    }
                }
            }
        }
    }
    return moved;
}

// The last static correction pass: if the line crosses the rhombus (getIntersectInfo: 3 = right edges, 0xc = left, 9 = up, 6 = down)
// and is the matching wall / ceiling / floor type, move the rhombus by the missing amount. Returns whether the line crosses it.
int grCollisionTestRhombus::testStaticCorrectionLast(grCollisionLineWork* lineWork) {
    clRhombus2D* rhombus = &m_shape1.m_rhombus;
    grCollSegmentEx wall;
    fn_80111968(lineWork->m_collision, &wall, lineWork->m_collisionLine, 0);
    grCollisionLineWork next;
    lineWork->getNext(&next);
    u32 info = rhombus->getIntersectInfo(wall.m_segment);
    if (info != 0) {
        if (isIgnoreLine(lineWork) == 0) {
            grCollisionLine* line = lineWork->m_collisionLine;
            Vec2f projected;
            Vec2f push;
            if (line->m_isRight && info == 3) {
                if (wall.m_segment.projectionY(rhombus->m_rightPos, projected)) {
                    push = (projected - rhombus->m_rightPos) + Vec2f(-0.01f, 0.0f);
                    rhombus->move(push);
                    m_unk2CA |= SIDE_RIGHT;
                }
            } else if (line->m_isLeft && info == 0xc) {
                if (wall.m_segment.projectionY(rhombus->m_leftPos, projected)) {
                    push = (projected - rhombus->m_leftPos) + Vec2f(0.01f, 0.0f);
                    rhombus->move(push);
                    m_unk2CA |= SIDE_LEFT;
                }
            } else if (line->m_isUpper && info == 9) {
                // HYPOTHESIS: the projection point is the left corner (as in the original), the push is measured at the up corner
                if (wall.m_segment.projectionX(rhombus->m_leftPos, projected)) {
                    push = (projected - rhombus->m_upPos) + Vec2f(0.0f, -0.01f);
                    rhombus->move(push);
                    m_unk30C = *lineWork;
                    m_unk2CA |= SIDE_UP;
                }
            } else if (line->m_isUnder && info == 6) {
                if (wall.m_segment.projectionX(rhombus->m_leftPos, projected)) {
                    push = (projected - rhombus->m_downPos) + Vec2f(0.0f, 0.01f);
                    rhombus->move(push);
                    m_unk2CA |= SIDE_DOWN;
                }
            }
        }
    }
    return info != 0;
}


// ===== cluster R2 =====

// MATCH-ONLY: the original computes min of two floats with a paired-single subtract and select.
static inline float psMinInPlace(register float a, register float b) {
    register float diff;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel diff, diff, b, a
    }
    // clang-format on
    return diff;
}

// HYPOTHESIS: the fsel based clamp helper (same shape as the one in gr_collision_body.cpp).
// MATCH-ONLY: inline fsel with a separate register for each step (nw4r::math::FSelect keeps the value in place).
static inline float clampFloat(register float value, register float low, register float high) {
    register float difference, bounded, result;
    difference = value - low;
    // clang-format off
    asm { fsel bounded, difference, value, low }
    // clang-format on
    difference = bounded - high;
    // clang-format off
    asm { fsel result, difference, high, bounded }
    // clang-format on
    return result;
}

// The line work has a line, a joint and a collision (the two-stage check of getPrev / getNext callers).
static inline bool isValidLineWork(grCollisionLineWork* lw) {
    bool stage2 = false;
    bool stage1 = false;
    if (lw->m_collisionLine != NULL && lw->m_collisionJoint != NULL) {
        stage1 = true;
    }
    if (stage1 && lw->m_collision != NULL) {
        stage2 = true;
    }
    return stage2;
}

// The line work is valid and its joint and its collision are enabled.
static inline bool isEnabledLineWork(grCollisionLineWork* lw) {
    bool enabled;
    if (isValidLineWork(lw) && lw->m_collisionJoint->m_0x54_4 && lw->m_collision->m_isEnabled) {
        enabled = true;
    } else {
        enabled = false;
    }
    return enabled;
}

// The single-flag form of the same check (testCollisionNotMove and the sliding-line selector).
static inline bool isActiveLineWorkR2(grCollisionLineWork* lw) {
    bool valid = false;
    if (lw->m_collisionLine != NULL && lw->m_collisionJoint != NULL && lw->m_collision != NULL) {
        valid = true;
    }
    bool active;
    if (valid && lw->m_collisionJoint->m_0x54_4 && lw->m_collision->m_isEnabled) {
        active = true;
    } else {
        active = false;
    }
    return active;
}

// The second half of isActiveLineWorkR2 for a caller that already computed the first (all three pointers set).
static inline bool isActiveGiven(grCollisionLineWork* lw, bool valid) {
    bool active;
    if (valid && lw->m_collisionJoint->m_0x54_4 && lw->m_collision->m_isEnabled) {
        active = true;
    } else {
        active = false;
    }
    return active;
}

// Bytes 16 and 17 of a line (the m_noAttach.. flags and the material) as one halfword; masked with m_exclusiveLineAttr.
static inline u16 getLineAttr(grCollisionLine* line) {
    return *(u16*)((u8*)line + 0x10);
}

// Does the rhombus' upper corner, moving from m_shape1 to m_shape2, hit the line (a floor-less line seen from below)?
// rate = in/out: the lowest rate of a hit so far. The corner itself is swept against the line; then the neighbour lines
// that continue to the left / right are tested with the point where the line ends above the corner.
int grCollisionTestRhombus::testUp(grCollisionLineWork* lineWork, float* rate) {
    clRhombus2D* cur;
    int hit = 0;
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    cur = &m_shape1.m_rhombus;
    if (!(m_unmovedMask & VTX_UP)) {
        Vec2f from;
        Vec2f to;
        Vec2f dir;
        float t;
        from = cur->m_upPos;
        to = m_shape2.m_rhombus.m_upPos;
        dir = to - from;
        hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
        if (hit > 0) {
            updateTouchLine(0, lineWork, t);
            *rate = psMinInPlace(*rate, t);
            m_hitDirs |= DIR_UP;
            m_unk2CA |= SIDE_UP;
            m_unk30C = *lineWork;
        }
        grCollSegmentEx nextSegment;
        grCollisionLineWork next;
        Vec2f onLine;
        Vec2f proj;
        if (segment.m_segment.projectionX(to, onLine)) {
          bool lineAbove = onLine.m_y > from.m_y;
          if (lineAbove) {
            lineWork->getNext(&next);
            grCollisionLineWork* nextLw = &next; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check)
            if (isEnabledLineWork(&next) && nextLw->m_collisionLine->m_isLeft) {
                fn_80111968(next.m_collision, &nextSegment, next.m_collisionLine, 0);
                if ((!nextSegment.m_segment.projectionY(cur->m_centerPos, proj) || !(cur->m_centerPos.m_x < proj.m_x)) &&
                    onLine.m_x <= segment.m_segment.m_p0Pos.m_x && to.m_y >= onLine.m_y) {
                    t = ((onLine.m_y - 0.01f) - from.m_y) / (to.m_y - from.m_y);
                    if (t >= 0.0f) {
                        t = clampFloat(t, 0.0f, 1.0f);
                        updateTouchLine(0, lineWork, t);
                        *rate = psMinInPlace(*rate, t);
                        m_hitDirs |= DIR_UP;
                        m_unk2CA |= SIDE_UP;
                    }
                }
            }
            lineWork->getPrev(&next);
            grCollisionLineWork* prevLw = &next; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check)
            if (isEnabledLineWork(&next) && prevLw->m_collisionLine->m_isRight) {
                fn_80111968(next.m_collision, &nextSegment, next.m_collisionLine, 0);
                if ((!nextSegment.m_segment.projectionY(cur->m_centerPos, proj) || !(proj.m_x < cur->m_centerPos.m_x)) &&
                    segment.m_segment.m_p1Pos.m_x <= onLine.m_x && to.m_y >= onLine.m_y) {
                    t = ((onLine.m_y - 0.01f) - from.m_y) / (to.m_y - from.m_y);
                    if (t >= 0.0f) {
                        t = clampFloat(t, 0.0f, 1.0f);
                        updateTouchLine(0, lineWork, t);
                        *rate = psMinInPlace(*rate, t);
                        m_hitDirs |= DIR_UP;
                        m_unk2CA |= SIDE_UP;
                    }
                }
            }
          }
        }
    }
    return hit > 0;
}

// Does the rhombus' lower corner, moving from m_shape1 to m_shape2, land on the line from above? When it does not hit the line
// itself, the line is tested as the end of a floor the corner is about to leave: if the corner moves along the floor onto the
// line's continuation the line is put on the pre-ignore list (it is walked over, not collided with).
int grCollisionTestRhombus::testDown(grCollisionLineWork* lineWork, float* rate) {
    int hit = 0;
    u32 ignore = 0;
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    Vec2f from;
    Vec2f to;
    Vec2f dir;
    float t;
    if (!(m_unmovedMask & VTX_DOWN)) {
        Vec2f proj;
        from = m_shape1.m_rhombus.m_downPos;
        to = m_shape2.m_rhombus.m_downPos;
        dir = to - from;
        hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
        if (hit > 0) {
            updateTouchLine(3, lineWork, t);
            *rate = psMinInPlace(*rate, t);
            m_hitDirs |= DIR_DOWN;
            m_unk2CA |= SIDE_DOWN;
        } else if (segment.m_segment.m_p0Pos.m_x <= to.m_x && to.m_x <= segment.m_segment.m_p1Pos.m_x &&
                   segment.m_segment.projectionX(to, proj) && to.m_y < proj.m_y) {
            grCollisionLineWork edge;
            grCollisionLineWork neighbor;
            if (dir.m_x > 0.0f) {
                if (lineWork->getLeftDownLine(&edge)) {
                    if (isEnabledLineWork(&edge) && !edge.getPrev(&neighbor)) {
                        ignore = 1;
                    }
                }
            } else if (dir.m_x < 0.0f) {
                if (lineWork->getRightDownLine(&edge)) {
                    if (isEnabledLineWork(&edge) && !edge.getNext(&neighbor)) {
                        ignore = 1;
                    }
                }
            }
            if (ignore) {
                pushbackPreIgnoreLine(lineWork);
            }
        }
    }
    if (ignore != 1) {
        grCollisionLineWork next;
        if (!((lineWork->getPrev(&next) && next.m_collisionLine->m_isRight) ||
              (lineWork->getNext(&next) && next.m_collisionLine->m_isLeft))) {
            from = m_shape1.m_rhombus.m_upPos;
            to = m_shape2.m_rhombus.m_upPos;
            dir = to - from;
            Vec2f proj;
            bool above = false;
            if (from.m_x < segment.m_segment.m_p0Pos.m_x) {
                if (from.m_y < segment.m_segment.m_p0Pos.m_y) {
                    above = true;
                }
            } else if (segment.m_segment.m_p1Pos.m_x < from.m_x) {
                if (from.m_y < segment.m_segment.m_p1Pos.m_y) {
                    above = true;
                }
            } else if (segment.m_segment.projectionX(from, proj) && from.m_y < proj.m_y) {
                above = true;
            }
            if (above && segment.m_segment.m_p0Pos.m_x <= to.m_x && to.m_x <= segment.m_segment.m_p1Pos.m_x &&
                segment.m_segment.projectionX(to, proj) && proj.m_y <= to.m_y) {
                pushbackPreIgnoreLine(lineWork);
            }
        }
    }
    return hit > 0;
}

// Does the rhombus' left corner, moving from m_shape1 to m_shape2, hit the line (a wall seen from its right side)?
// When the wall's centre is on the right of the rhombus nothing else is tested. Otherwise the neighbour lines that continue
// the wall above (a ceiling) and below (a floor) are tested with the upper / lower corner, and then the corner is swept
// against the wall again from the position the corner was pushed to.
int grCollisionTestRhombus::testLeft(grCollisionLineWork* lineWork, float* rate) {
    clRhombus2D* cur;
    int hit = 0;
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    cur = &m_shape1.m_rhombus;
    Vec2f from;
    Vec2f to;
    Vec2f dir;
    float t;
    if (!(m_unmovedMask & VTX_LEFT)) {
        from = cur->m_leftPos;
        to = m_shape2.m_rhombus.m_leftPos;
        dir = to - from;
        hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
        if (hit > 0) {
            updateTouchLine(1, lineWork, t);
            *rate = psMinInPlace(*rate, t);
            m_hitDirs |= DIR_LEFT;
            m_unk2CA |= SIDE_LEFT;
        }
    }
    Vec2f centerOnLine;
    if (segment.m_segment.projectionY(cur->m_centerPos, centerOnLine) && cur->m_centerPos.m_x < centerOnLine.m_x) {
        return hit > 0;
    }
    if (!(m_unmovedMask & VTX_UP)) {
        grCollSegmentEx prevSegment;
        grCollisionLineWork prev;
        Vec2f onLine;
        lineWork->getPrev(&prev);
        grCollisionLineWork* prevLw = &prev; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check)
        if (isEnabledLineWork(&prev) && prevLw->m_collisionLine->m_isUpper) {
            from = cur->m_upPos;
            to = m_shape2.m_rhombus.m_upPos;
            dir = to - from;
            fn_80111968(prev.m_collision, &prevSegment, prev.m_collisionLine, 0);
            if (to.m_x <= prevSegment.m_segment.m_p0Pos.m_x && prevSegment.m_segment.projectionX(to, onLine) &&
                onLine.m_y > from.m_y && to.m_y >= onLine.m_y) {
                t = ((onLine.m_y - 0.01f) - from.m_y) / (to.m_y - from.m_y);
                t = clampFloat(t, 0.0f, 1.0f);
                updateTouchLine(0, &prev, t);
                *rate = psMinInPlace(*rate, t);
                m_hitDirs |= DIR_UP;
                m_unk2CA |= SIDE_UP;
                hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
                if (hit > 0) {
                    updateTouchLine(1, lineWork, t);
                    *rate = psMinInPlace(*rate, t);
                    m_hitDirs |= DIR_LEFT;
                    m_unk2CA |= SIDE_LEFT;
                }
            }
        }
    }
    if (!(m_unmovedMask & VTX_DOWN)) {
        grCollSegmentEx nextSegment;
        grCollisionLineWork next;
        Vec2f onLine;
        lineWork->getNext(&next);
        grCollisionLineWork* nextLw = &next; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check)
        if (isEnabledLineWork(&next) && nextLw->m_collisionLine->m_isUnder && (getLineAttr(next.m_collisionLine) & m_exclusiveLineAttr) == 0 &&
            !fn_80137114(m_collStatus, &next)) {
            from = cur->m_downPos;
            to = m_shape2.m_rhombus.m_downPos;
            dir = to - from;
            fn_80111968(next.m_collision, &nextSegment, next.m_collisionLine, 0);
            if (to.m_x <= nextSegment.m_segment.m_p1Pos.m_x && nextSegment.m_segment.projectionX(to, onLine) &&
                onLine.m_y < from.m_y && to.m_y <= onLine.m_y) {
                t = ((onLine.m_y + 0.01f) - from.m_y) / (to.m_y - from.m_y);
                t = clampFloat(t, 0.0f, 1.0f);
                updateTouchLine(3, &next, t);
                *rate = psMinInPlace(*rate, t);
                m_hitDirs |= DIR_LEFT;
                m_unk2CA |= SIDE_DOWN;
                hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
                if (hit > 0) {
                    updateTouchLine(1, lineWork, t);
                    *rate = psMinInPlace(*rate, t);
                    m_hitDirs |= DIR_LEFT;
                    m_unk2CA |= SIDE_LEFT;
                }
            }
        }
    }
    if (m_collStatus->m_setting == 1 && !(m_unmovedMask & VTX_DOWN)) {
        from = cur->m_downPos;
        to = m_shape2.m_rhombus.m_downPos;
        dir = to - from;
        if (testMovePtToSeg(&from, &dir, &segment.m_segment, &t) > 0) {
            updateTouchLine(1, lineWork, t);
            *rate = psMinInPlace(*rate, t);
            m_hitDirs |= DIR_LEFT;
            m_unk2CA |= SIDE_LEFT;
        }
    }
    return hit > 0;
}

// Does the rhombus' right corner, moving from m_shape1 to m_shape2, hit the line (a wall seen from its left side)? The
// mirror image of testLeft: when the wall's centre is on the left of the rhombus nothing else is tested, otherwise the
// neighbour lines above (next) and below (previous) the wall are tested with the upper / lower corner.
int grCollisionTestRhombus::testRight(grCollisionLineWork* lineWork, float* rate) {
    clRhombus2D* cur;
    int hit = 0;
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    cur = &m_shape1.m_rhombus;
    Vec2f from;
    Vec2f to;
    Vec2f dir;
    float t;
    if (!(m_unmovedMask & VTX_RIGHT)) {
        from = cur->m_rightPos;
        to = m_shape2.m_rhombus.m_rightPos;
        dir = to - from;
        hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
        if (hit > 0) {
            updateTouchLine(2, lineWork, t);
            *rate = psMinInPlace(*rate, t);
            m_hitDirs |= DIR_RIGHT;
            m_unk2CA |= SIDE_RIGHT;
        }
    }
    Vec2f centerOnLine;
    if (segment.m_segment.projectionY(cur->m_centerPos, centerOnLine) && centerOnLine.m_x < cur->m_centerPos.m_x) {
        return hit > 0;
    }
    if (!(m_unmovedMask & VTX_UP)) {
        grCollSegmentEx nextSegment;
        grCollisionLineWork next;
        Vec2f onLine;
        lineWork->getNext(&next);
        grCollisionLineWork* nextLw = &next; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check)
        if (isEnabledLineWork(&next) && nextLw->m_collisionLine->m_isUpper) {
            from = cur->m_upPos;
            to = m_shape2.m_rhombus.m_upPos;
            dir = to - from;
            fn_80111968(next.m_collision, &nextSegment, next.m_collisionLine, 0);
            if (nextSegment.m_segment.m_p1Pos.m_x <= to.m_x && nextSegment.m_segment.projectionX(to, onLine) &&
                onLine.m_y > from.m_y && to.m_y >= onLine.m_y) {
                t = ((onLine.m_y - 0.01f) - from.m_y) / (to.m_y - from.m_y);
                t = clampFloat(t, 0.0f, 1.0f);
                updateTouchLine(0, &next, t);
                *rate = psMinInPlace(*rate, t);
                m_hitDirs |= DIR_UP;
                m_unk2CA |= SIDE_UP;
                hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
                if (hit > 0) {
                    updateTouchLine(2, lineWork, t);
                    *rate = psMinInPlace(*rate, t);
                    m_hitDirs |= DIR_RIGHT;
                    m_unk2CA |= SIDE_RIGHT;
                }
            }
        }
    }
    if (!(m_unmovedMask & VTX_DOWN)) {
        grCollSegmentEx prevSegment;
        grCollisionLineWork prev;
        Vec2f onLine;
        lineWork->getPrev(&prev);
        grCollisionLineWork* prevLw = &prev; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check)
        if (isEnabledLineWork(&prev) && prevLw->m_collisionLine->m_isUnder && (getLineAttr(prev.m_collisionLine) & m_exclusiveLineAttr) == 0 &&
            !fn_80137114(m_collStatus, &prev)) {
            from = cur->m_downPos;
            to = m_shape2.m_rhombus.m_downPos;
            dir = to - from;
            fn_80111968(prev.m_collision, &prevSegment, prev.m_collisionLine, 0);
            if (prevSegment.m_segment.m_p0Pos.m_x <= to.m_x && prevSegment.m_segment.projectionX(to, onLine) &&
                onLine.m_y < from.m_y && to.m_y <= onLine.m_y) {
                t = ((onLine.m_y + 0.01f) - from.m_y) / (to.m_y - from.m_y);
                t = clampFloat(t, 0.0f, 1.0f);
                updateTouchLine(3, &prev, t);
                *rate = psMinInPlace(*rate, t);
                m_hitDirs |= DIR_RIGHT;
                m_unk2CA |= SIDE_DOWN;
                hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
                if (hit > 0) {
                    updateTouchLine(2, lineWork, t);
                    *rate = psMinInPlace(*rate, t);
                    m_hitDirs |= DIR_RIGHT;
                    m_unk2CA |= SIDE_RIGHT;
                }
            }
        }
    }
    if (m_collStatus->m_setting == 1 && !(m_unmovedMask & VTX_DOWN)) {
        from = cur->m_downPos;
        to = m_shape2.m_rhombus.m_downPos;
        dir = to - from;
        hit = testMovePtToSeg(&from, &dir, &segment.m_segment, &t);
        if (hit > 0) {
            updateTouchLine(2, lineWork, t);
            *rate = psMinInPlace(*rate, t);
            m_hitDirs |= DIR_RIGHT;
            m_unk2CA |= SIDE_RIGHT;
        }
    }
    return hit > 0;
}

// Snaps each corner of m_shape2 onto the one of m_shape1 when it moves less than 1e-5 (rounding noise of the interpolation),
// then records which corners of m_shape1 and m_shape2 are identical in m_unmovedMask.
void grCollisionTestRhombus::testCollisionSetup() {
    Vec2f upMove = m_shape2.m_rhombus.m_upPos - m_shape1.m_rhombus.m_upPos;
    if (upMove.length() < 0.00001f) {
        m_shape2.m_rhombus.m_upPos = m_shape1.m_rhombus.m_upPos;
    }
    Vec2f downMove = m_shape2.m_rhombus.m_downPos - m_shape1.m_rhombus.m_downPos;
    if (downMove.length() < 0.00001f) {
        m_shape2.m_rhombus.m_downPos = m_shape1.m_rhombus.m_downPos;
    }
    Vec2f leftMove = m_shape2.m_rhombus.m_leftPos - m_shape1.m_rhombus.m_leftPos;
    if (leftMove.length() < 0.00001f) {
        m_shape2.m_rhombus.m_leftPos = m_shape1.m_rhombus.m_leftPos;
    }
    Vec2f rightMove = m_shape2.m_rhombus.m_rightPos - m_shape1.m_rhombus.m_rightPos;
    if (rightMove.length() < 0.00001f) {
        m_shape2.m_rhombus.m_rightPos = m_shape1.m_rhombus.m_rightPos;
    }
    m_unmovedMask = 0;
    bool same = false;
    if (m_shape1.m_rhombus.m_upPos.m_x == m_shape2.m_rhombus.m_upPos.m_x && m_shape1.m_rhombus.m_upPos.m_y == m_shape2.m_rhombus.m_upPos.m_y) {
        same = true;
    }
    if (same) {
        m_unmovedMask |= VTX_UP;
    }
    same = false;
    if (m_shape1.m_rhombus.m_downPos.m_x == m_shape2.m_rhombus.m_downPos.m_x && m_shape1.m_rhombus.m_downPos.m_y == m_shape2.m_rhombus.m_downPos.m_y) {
        same = true;
    }
    if (same) {
        m_unmovedMask |= VTX_DOWN;
    }
    same = false;
    if (m_shape1.m_rhombus.m_rightPos.m_x == m_shape2.m_rhombus.m_rightPos.m_x && m_shape1.m_rhombus.m_rightPos.m_y == m_shape2.m_rhombus.m_rightPos.m_y) {
        same = true;
    }
    if (same) {
        m_unmovedMask |= VTX_RIGHT;
    }
    same = false;
    if (m_shape1.m_rhombus.m_leftPos.m_x == m_shape2.m_rhombus.m_leftPos.m_x && m_shape1.m_rhombus.m_leftPos.m_y == m_shape2.m_rhombus.m_leftPos.m_y) {
        same = true;
    }
    if (same) {
        m_unmovedMask |= VTX_LEFT;
    }
}

// Tests the rhombus against one line when it does not move relative to the line: each side of the rhombus is tested
// against the line's directions (a left wall with the left corner, a right wall with the right corner, a ceiling with the
// upper corner, a floor with the lower corner), skipping the sides the line work masks out (m_unkC). A corner (a floor with an
// attached ceiling, a ceiling with an attached floor) and every wall are also tested as a sliding line. Returns whether
// any side hit the line.
int grCollisionTestRhombus::testCollisionNotMove(grCollisionLineWork* lineWork) {
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    if (isIgnoreLine(lineWork)) {
        return 0;
    }
    grCollisionLineWork neighbor;
    grCollisionLineWork* neighborLw = &neighbor; // MATCH-ONLY: the line flags are read through a pointer to the local (no CSE with the checks)
    testCollisionSetup();
    float rate = 1.0f;
    int hits = 0;
    grCollisionLine* line = lineWork->m_collisionLine;
    m_hitDirs = 0;
    if (line->m_isLeft && !(lineWork->m_unkC & 1)) {
        if (testLeft(lineWork, &rate)) {
            hits++;
        }
    }
    if (lineWork->m_collisionLine->m_isRight && !(lineWork->m_unkC & 2)) {
        if (testRight(lineWork, &rate)) {
            hits++;
        }
    }
    if (lineWork->m_collisionLine->m_isUpper && !(lineWork->m_unkC & 4)) {
        if (testUp(lineWork, &rate)) {
            hits++;
        }
    }
    if (lineWork->m_collisionLine->m_isUnder && !(lineWork->m_unkC & 8)) {
        if (testDown(lineWork, &rate)) {
            hits++;
        }
    }
    line = lineWork->m_collisionLine;
    if (line->m_isLeft || line->m_isRight ||
        (line->m_isUpper && lineWork->getNext(&neighbor) && isActiveLineWorkR2(&neighbor) && neighborLw->m_collisionLine->m_isUnder) ||
        (lineWork->getPrev(&neighbor) && isActiveLineWorkR2(&neighbor) && neighborLw->m_collisionLine->m_isUnder) ||
        (lineWork->m_collisionLine->m_isUnder && lineWork->getNext(&neighbor) && isActiveLineWorkR2(&neighbor) &&
         neighborLw->m_collisionLine->m_isUpper) ||
        (lineWork->getPrev(&neighbor) && isActiveLineWorkR2(&neighbor) && neighborLw->m_collisionLine->m_isUpper)) {
        testCollisionNotMoveSlatingLine(lineWork, &rate);
    }
    if (m_unk8 > rate) {
        m_unk8 = rate;
    }
    if (hits) {
        return 1;
    }
    return 0;
}

// HYPOTHESIS: chooses the sliding direction of a wall (or a floor / ceiling with an attached wall): along X (the rhombus mostly
// moves horizontally and the line has an attached floor / ceiling the move can slide on) or along Y, then runs the matching
// variant for the line's type. Always returns 1.
int grCollisionTestRhombus::testCollisionNotMoveSlatingLine(grCollisionLineWork* lineWork, float* rate) {
    bool valid = false;
    int moveX = 0;
    u8 horizontal = 0;
    if (m_collStatus->m_setting != 1) {
        horizontal = m_unk47C;
    }
    if (horizontal) {
        grCollisionLineWork next;
        grCollisionLineWork prev;
        grCollisionLine* line = lineWork->m_collisionLine;
        if (line->m_isRight) {
            grCollisionLineWork rightNext;
            grCollisionLineWork* rightNextLw = &rightNext; // MATCH-ONLY: pointer to the local (no CSE)
            lineWork->getNext(&rightNext);
            if (rightNext.m_collisionLine != NULL && rightNext.m_collisionJoint != NULL && rightNext.m_collision != NULL) {
                valid = true;
            }
            if (isActiveGiven(&rightNext, valid) && rightNextLw->m_collisionLine->m_isUnder) {
                moveX = 1;
            } else {
                lineWork->getPrev(&prev);
                grCollisionLineWork* prevLw = &prev; // MATCH-ONLY: pointer to the local (no CSE)
                if (isActiveLineWorkR2(&prev) && prevLw->m_collisionLine->m_isUpper) {
                    moveX = 1;
                }
            }
        } else if (line->m_isLeft) {
            grCollisionLineWork leftPrev;
            grCollisionLineWork* leftPrevLw = &leftPrev; // MATCH-ONLY: pointer to the local (no CSE)
            lineWork->getPrev(&leftPrev);
            if (leftPrev.m_collisionLine != NULL && leftPrev.m_collisionJoint != NULL && leftPrev.m_collision != NULL) {
                valid = true;
            }
            if (isActiveGiven(&leftPrev, valid) && leftPrevLw->m_collisionLine->m_isUnder) {
                moveX = 1;
            } else {
                lineWork->getNext(&next);
                grCollisionLineWork* nextLw = &next; // MATCH-ONLY: pointer to the local (no CSE)
                if (isActiveLineWorkR2(&next) && nextLw->m_collisionLine->m_isUpper) {
                    moveX = 1;
                }
            }
        } else {
            if (m_collStatus->m_tracerType) {
                Vec2f move = m_shape2.m_rhombus.m_centerPos - m_shape1.m_rhombus.m_centerPos;
                if ((line->m_isUnder || line->m_isUpper) && move.m_y > 0.0f && move.m_y > (float)__fabs(move.m_x)) {
                    moveX = 0;
                } else {
                    moveX = 1;
                }
            } else {
                moveX = 1;
            }
        }
    }
    if (moveX) {
        grCollisionLine* line = lineWork->m_collisionLine;
        if (line->m_isRight) {
            if (!(lineWork->m_unkC & 2)) {
                testCollisionMoveSlatingLineMoveXRight(lineWork, rate);
            }
        } else if (line->m_isLeft) {
            if (!(lineWork->m_unkC & 1)) {
                testCollisionMoveSlatingLineMoveXLeft(lineWork, rate);
            }
        } else if (line->m_isUpper) {
            testCollisionMoveSlatingLineMoveXUp(lineWork, rate);
        } else if (line->m_isUnder) {
            testCollisionMoveSlatingLineMoveXDown(lineWork, rate);
        }
    } else {
        grCollisionLine* line = lineWork->m_collisionLine;
        if (line->m_isRight) {
            if (!(lineWork->m_unkC & 2)) {
                testCollisionMoveSlatingLineMoveYRight(lineWork, rate);
            }
        } else if (line->m_isLeft) {
            if (!(lineWork->m_unkC & 1)) {
                testCollisionMoveSlatingLineMoveYLeft(lineWork, rate);
            }
        } else if (line->m_isUpper) {
            testCollisionMoveSlatingLineMoveYUp(lineWork, rate);
        } else if (line->m_isUnder) {
            testCollisionMoveSlatingLineMoveYDown(lineWork, rate);
        }
    }
    return 1;
}


// ===== cluster R3 =====
// grCollisionTestRhombus: the X-axis corner sweeps of testCollisionNotMoveSlatingLine (main.dol 0x8012CFA0-0x8012F6AC).
//
// testCollisionNotMoveSlatingLine picks the X family when the move is mostly horizontal; the Y family (the next unit) is its
// mirror image. A dispatcher (Right/Left/Up/Down) looks at the neighbours of the wall/floor/ceiling line and calls up to
// two corner variants. A corner variant sweeps the quad spanned by a pair of corners (vertical + horizontal corner) of
// m_shape1 (where the rhombus was) and of m_shape2 (where it ended up) against the line; when the line cuts the swept
// area and the corner ends up on the wrong side of it, m_shape2 is pushed out along X to the line (offset to the line
// in X plus a 0.01 tilt in Y) and the hit is recorded with updateTouchLine.
//
// The corner variants are one skeleton, spelled out per variant because the original does not share code between them
// (each differs in frame layout and register use). Per-variant parameters (cn = the neighbour accessor, seg end = the
// end of the line used for the inside test and the near-side test, search = the line search for the continuation):
//
//   variant     cn        corners (vert, horiz)  seg end  search (on)            eps     touch id  removeIgnoreLine
//   RightDown   getNext   down, right            p1       getPosXDownLine (next) +0.01   7         yes
//   RightUp     getPrev   up, right              p0       getPosXUpLine   (next) -0.01   5         no
//   LeftDown    getPrev   down, left             p0       getPosXDownLine (next) +0.01   6         yes
//   LeftUp      getNext   up, left               p1       getPosXUpLine   (next) -0.01   4         no
//   UpRight     -         up, right              p1       getPosXUpLine   (line) -0.01   7         no
//   UpLeft      -         up, left               p0       getPosXUpLine   (line) -0.01   7         no
//   DownRight   getPrev*  down, right            p0       getPosXDownLine (line) +0.01   7         no      (* fetched, unused)
//   DownLeft    -         down, left             p1       getPosXDownLine (line) +0.01   7         no
//
// The hit test is "move.y > 0" for +0.01 and "move.y < 0" for -0.01. The dispatchers' conditions are in
// grCollisionTestRhombus' layout notes (m_hitDirs / m_unmovedMask masks per corner).

// A line work with all three pointers set whose joint is active and whose collision is enabled, checked in two stages
// the way the corner variants write it (the dispatchers write the same test inline with one flag).
static inline bool isActiveLineWorkStaged(const grCollisionLineWork& lw) {
    bool stage2 = false;
    bool stage1 = false;
    stage1 = lw.m_collisionLine != NULL && lw.m_collisionJoint != NULL;
    if (stage1 && lw.m_collision != NULL) {
        stage2 = true;
    }
    bool active;
    if (stage2 && lw.m_collisionJoint->m_0x54_4 && lw.m_collision->m_isEnabled) {
        active = true;
    } else {
        active = false;
    }
    return active;
}

// HYPOTHESIS: the segment of a line as the corner variants fetch it (the four-argument getSegment, extra word 0).
static inline void fetchSegment(grCollSegmentEx* out, grCollisionLineWork* lw) {
    fn_80111968(lw->m_collision, out, lw->m_collisionLine, 0);
}


// 0x8012CFA0: dispatches to the two corner sweeps of the right line (neighbour lines checked for type and corner state).
int grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXRight(grCollisionLineWork* lineWork, float* rate) {
    grCollSegmentEx unused;
    fetchSegment(&unused, lineWork);
    bool valid = false;
    grCollisionLineWork prev;
    grCollisionLineWork next;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    if ((m_hitDirs & 10) == 0) {
        if (next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL) {
            valid = true;
        }
        bool active;
        if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        grCollisionLineWork* nextLw = &next; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check above)
        if (active && nextLw->m_collisionLine->m_isUnder) {
            int still = m_unmovedMask & 6; // MATCH-ONLY: the original compares the mask against 'all bits' and against 0
            if (still != 6 && still == 0) {
                testCollisionMoveSlatingLineMoveXRightDown(lineWork, rate);
            }
        }
    }
    if ((m_hitDirs & 6) == 0) {
        valid = false;
        if (prev.m_collisionLine != NULL && prev.m_collisionJoint != NULL && prev.m_collision != NULL) {
            valid = true;
        }
        bool active;
        if (valid && prev.m_collisionJoint->m_0x54_4 && prev.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        grCollisionLineWork* prevLw = &prev; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check above)
        if (active && prevLw->m_collisionLine->m_isUpper) {
            int still = m_unmovedMask & 5; // MATCH-ONLY: the original compares the mask against 'all bits' and against 0
            if (still != 5 && still == 0) {
                testCollisionMoveSlatingLineMoveXRightUp(lineWork, rate);
            }
        }
    }
    return 1;
}

// 0x8012D1A8: dispatches to the two corner sweeps of the left line (neighbour lines checked for type and corner state).
int grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXLeft(grCollisionLineWork* lineWork, float* rate) {
    bool valid = false;
    grCollisionLineWork prev;
    grCollisionLineWork next;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    if ((m_hitDirs & 9) == 0) {
        if (prev.m_collisionLine != NULL && prev.m_collisionJoint != NULL && prev.m_collision != NULL) {
            valid = true;
        }
        bool active;
        if (valid && prev.m_collisionJoint->m_0x54_4 && prev.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        grCollisionLineWork* prevLw = &prev; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check above)
        if (active && prevLw->m_collisionLine->m_isUnder) {
            int still = m_unmovedMask & 10; // MATCH-ONLY: the original compares the mask against 'all bits' and against 0
            if (still != 10 && still == 0) {
                testCollisionMoveSlatingLineMoveXLeftDown(lineWork, rate);
            }
        }
    }
    if ((m_hitDirs & 5) == 0) {
        valid = false;
        if (next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL) {
            valid = true;
        }
        bool active;
        if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        grCollisionLineWork* nextLw = &next; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check above)
        if (active && nextLw->m_collisionLine->m_isUpper) {
            int still = m_unmovedMask & 9; // MATCH-ONLY: the original compares the mask against 'all bits' and against 0
            if (still != 9 && still == 0) {
                testCollisionMoveSlatingLineMoveXLeftUp(lineWork, rate);
            }
        }
    }
    return 1;
}

// 0x8012D39C: sweeps the down/right corner pair; the neighbour line is next.
void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXRightDown(grCollisionLineWork* lineWork, float* rate) {
    bool hit = false;
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx seg;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollSegmentEx nextSeg;
    grCollisionLineWork next;
    grCollisionLineWork search;
    Vec2f foot;
    Vec2f move;
    lineWork->getNext(&next);
    fetchSegment(&seg, lineWork);
    from.m_segment.m_p0Pos = m_shape1.m_rhombus.m_downPos;
    from.m_segment.m_p1Pos = m_shape1.m_rhombus.m_rightPos;
    from.m_unk10 = 0;
    to.m_segment.m_p0Pos = rhombus2->m_downPos;
    to.m_segment.m_p1Pos = rhombus2->m_rightPos;
    to.m_unk10 = 0;
    if (from.m_segment.m_p0Pos.m_x == to.m_segment.m_p0Pos.m_x && from.m_segment.m_p0Pos.m_y == to.m_segment.m_p0Pos.m_y) {
        hit = true;
    }
    if (hit) {
        return;
    }
    bool unchanged = false;
    if (from.m_segment.m_p1Pos.m_x == to.m_segment.m_p1Pos.m_x && from.m_segment.m_p1Pos.m_y == to.m_segment.m_p1Pos.m_y) {
        unchanged = true;
    }
    if (unchanged) {
        return;
    }
    quad.set(from.m_segment, to.m_segment);
    if (quad.checkIntersect(seg.m_segment) == 0) {
        return;
    }
    if (quad.checkInclude(seg.m_segment.m_p1Pos) == 0) {
        return;
    }
    hit = false;
    if (to.m_segment.m_p0Pos.m_x < seg.m_segment.m_p1Pos.m_x) {
        if (to.m_segment.projectionX(seg.m_segment.m_p1Pos, foot)) {
            move = (seg.m_segment.m_p1Pos - foot) + Vec2f(0.0f, 0.01f);
            if (move.m_y > 0.0f) {
                hit = true;
            }
        }
    } else {
        const Vec2f& corner = to.m_segment.m_p0Pos; // MATCH-ONLY: the corner's address stays in a register across the calls
        fetchSegment(&nextSeg, &next);
        if (nextSeg.m_segment.m_p1Pos.m_x < to.m_segment.m_p0Pos.m_x) {
            next.getPosXDownLine(to.m_segment.m_p0Pos.m_x, &search);
            if (isActiveLineWorkStaged(search)) {
                fetchSegment(&nextSeg, &search);
            }
        }
        if (to.m_segment.m_p0Pos.m_x < nextSeg.m_segment.m_p1Pos.m_x &&
            nextSeg.m_segment.projectionX(corner, foot)) {
            move = (foot - to.m_segment.m_p0Pos) + Vec2f(0.0f, 0.01f);
        } else {
            move = (nextSeg.m_segment.m_p1Pos - to.m_segment.m_p0Pos) + Vec2f(0.0f, 0.01f);
        }
        if (move.m_y > 0.0f) {
            hit = true;
        }
    }
    if (hit) {
        rhombus2->move(move);
        *rate = 1.0f;
        updateTouchLine(7, lineWork, 1.0f);
        fn_801371D8(m_collStatus, &next);
    }
}

// 0x8012D7D8: sweeps the up/right corner pair; the neighbour line is prev.
void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXRightUp(grCollisionLineWork* lineWork, float* rate) {
    bool hit = false;
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx seg;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollSegmentEx nextSeg;
    grCollisionLineWork next;
    grCollisionLineWork search;
    Vec2f foot;
    Vec2f move;
    lineWork->getPrev(&next);
    fetchSegment(&seg, lineWork);
    from.m_segment.m_p0Pos = m_shape1.m_rhombus.m_upPos;
    from.m_segment.m_p1Pos = m_shape1.m_rhombus.m_rightPos;
    from.m_unk10 = 0;
    to.m_segment.m_p0Pos = rhombus2->m_upPos;
    to.m_segment.m_p1Pos = rhombus2->m_rightPos;
    to.m_unk10 = 0;
    if (from.m_segment.m_p0Pos.m_x == to.m_segment.m_p0Pos.m_x && from.m_segment.m_p0Pos.m_y == to.m_segment.m_p0Pos.m_y) {
        hit = true;
    }
    if (hit) {
        return;
    }
    bool unchanged = false;
    if (from.m_segment.m_p1Pos.m_x == to.m_segment.m_p1Pos.m_x && from.m_segment.m_p1Pos.m_y == to.m_segment.m_p1Pos.m_y) {
        unchanged = true;
    }
    if (unchanged) {
        return;
    }
    quad.set(from.m_segment, to.m_segment);
    if (quad.checkIntersect(seg.m_segment) == 0) {
        return;
    }
    if (quad.checkInclude(seg.m_segment.m_p0Pos) == 0) {
        return;
    }
    hit = false;
    if (to.m_segment.m_p0Pos.m_x < seg.m_segment.m_p0Pos.m_x) {
        if (to.m_segment.projectionX(seg.m_segment.m_p0Pos, foot)) {
            move = (seg.m_segment.m_p0Pos - foot) + Vec2f(0.0f, -0.01f);
            if (move.m_y < 0.0f) {
                hit = true;
            }
        }
    } else {
        const Vec2f& corner = to.m_segment.m_p0Pos; // MATCH-ONLY: the corner's address stays in a register across the calls
        fetchSegment(&nextSeg, &next);
        if (nextSeg.m_segment.m_p0Pos.m_x < to.m_segment.m_p0Pos.m_x) {
            next.getPosXUpLine(to.m_segment.m_p0Pos.m_x, &search);
            if (isActiveLineWorkStaged(search)) {
                fetchSegment(&nextSeg, &search);
            }
        }
        if (to.m_segment.m_p0Pos.m_x < nextSeg.m_segment.m_p0Pos.m_x &&
            nextSeg.m_segment.projectionX(corner, foot)) {
            move = (foot - to.m_segment.m_p0Pos) + Vec2f(0.0f, -0.01f);
        } else {
            move = (nextSeg.m_segment.m_p0Pos - to.m_segment.m_p0Pos) + Vec2f(0.0f, -0.01f);
        }
        if (move.m_y < 0.0f) {
            hit = true;
        }
    }
    if (hit) {
        rhombus2->move(move);
        *rate = 1.0f;
        updateTouchLine(5, lineWork, 1.0f);
    }
}

// 0x8012DC08: sweeps the down/left corner pair; the neighbour line is prev.
void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXLeftDown(grCollisionLineWork* lineWork, float* rate) {
    bool hit = false;
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx seg;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollSegmentEx nextSeg;
    grCollisionLineWork next;
    grCollisionLineWork search;
    Vec2f foot;
    Vec2f move;
    lineWork->getPrev(&next);
    fetchSegment(&seg, lineWork);
    from.m_segment.m_p0Pos = m_shape1.m_rhombus.m_downPos;
    from.m_segment.m_p1Pos = m_shape1.m_rhombus.m_leftPos;
    from.m_unk10 = 0;
    to.m_segment.m_p0Pos = rhombus2->m_downPos;
    to.m_segment.m_p1Pos = rhombus2->m_leftPos;
    to.m_unk10 = 0;
    if (from.m_segment.m_p0Pos.m_x == to.m_segment.m_p0Pos.m_x && from.m_segment.m_p0Pos.m_y == to.m_segment.m_p0Pos.m_y) {
        hit = true;
    }
    if (hit) {
        return;
    }
    bool unchanged = false;
    if (from.m_segment.m_p1Pos.m_x == to.m_segment.m_p1Pos.m_x && from.m_segment.m_p1Pos.m_y == to.m_segment.m_p1Pos.m_y) {
        unchanged = true;
    }
    if (unchanged) {
        return;
    }
    quad.set(from.m_segment, to.m_segment);
    if (quad.checkIntersect(seg.m_segment) == 0) {
        return;
    }
    if (quad.checkInclude(seg.m_segment.m_p0Pos) == 0) {
        return;
    }
    hit = false;
    if (seg.m_segment.m_p0Pos.m_x < to.m_segment.m_p0Pos.m_x) {
        if (to.m_segment.projectionX(seg.m_segment.m_p0Pos, foot)) {
            move = (seg.m_segment.m_p0Pos - foot) + Vec2f(0.0f, 0.01f);
            if (move.m_y > 0.0f) {
                hit = true;
            }
        }
    } else {
        const Vec2f& corner = to.m_segment.m_p0Pos; // MATCH-ONLY: the corner's address stays in a register across the calls
        fetchSegment(&nextSeg, &next);
        if (to.m_segment.m_p0Pos.m_x < nextSeg.m_segment.m_p0Pos.m_x) {
            next.getPosXDownLine(to.m_segment.m_p0Pos.m_x, &search);
            if (isActiveLineWorkStaged(search)) {
                fetchSegment(&nextSeg, &search);
            }
        }
        if (nextSeg.m_segment.m_p0Pos.m_x < to.m_segment.m_p0Pos.m_x &&
            nextSeg.m_segment.projectionX(corner, foot)) {
            move = (foot - to.m_segment.m_p0Pos) + Vec2f(0.0f, 0.01f);
        } else {
            move = (nextSeg.m_segment.m_p0Pos - to.m_segment.m_p0Pos) + Vec2f(0.0f, 0.01f);
        }
        if (move.m_y > 0.0f) {
            hit = true;
        }
    }
    if (hit) {
        rhombus2->move(move);
        *rate = 1.0f;
        updateTouchLine(6, lineWork, 1.0f);
        fn_801371D8(m_collStatus, &next);
    }
}

// 0x8012E044: sweeps the up/left corner pair; the neighbour line is next.
void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXLeftUp(grCollisionLineWork* lineWork, float* rate) {
    bool hit = false;
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx seg;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollSegmentEx nextSeg;
    grCollisionLineWork next;
    grCollisionLineWork search;
    Vec2f foot;
    Vec2f move;
    lineWork->getNext(&next);
    fetchSegment(&seg, lineWork);
    from.m_segment.m_p0Pos = m_shape1.m_rhombus.m_upPos;
    from.m_segment.m_p1Pos = m_shape1.m_rhombus.m_leftPos;
    from.m_unk10 = 0;
    to.m_segment.m_p0Pos = rhombus2->m_upPos;
    to.m_segment.m_p1Pos = rhombus2->m_leftPos;
    to.m_unk10 = 0;
    if (from.m_segment.m_p0Pos.m_x == to.m_segment.m_p0Pos.m_x && from.m_segment.m_p0Pos.m_y == to.m_segment.m_p0Pos.m_y) {
        hit = true;
    }
    if (hit) {
        return;
    }
    bool unchanged = false;
    if (from.m_segment.m_p1Pos.m_x == to.m_segment.m_p1Pos.m_x && from.m_segment.m_p1Pos.m_y == to.m_segment.m_p1Pos.m_y) {
        unchanged = true;
    }
    if (unchanged) {
        return;
    }
    quad.set(from.m_segment, to.m_segment);
    if (quad.checkIntersect(seg.m_segment) == 0) {
        return;
    }
    if (quad.checkInclude(seg.m_segment.m_p1Pos) == 0) {
        return;
    }
    hit = false;
    if (seg.m_segment.m_p1Pos.m_x < to.m_segment.m_p0Pos.m_x) {
        if (to.m_segment.projectionX(seg.m_segment.m_p1Pos, foot)) {
            move = (seg.m_segment.m_p1Pos - foot) + Vec2f(0.0f, -0.01f);
            if (move.m_y < 0.0f) {
                hit = true;
            }
        }
    } else {
        const Vec2f& corner = to.m_segment.m_p0Pos; // MATCH-ONLY: the corner's address stays in a register across the calls
        fetchSegment(&nextSeg, &next);
        if (to.m_segment.m_p0Pos.m_x < nextSeg.m_segment.m_p1Pos.m_x) {
            next.getPosXUpLine(to.m_segment.m_p0Pos.m_x, &search);
            if (isActiveLineWorkStaged(search)) {
                fetchSegment(&nextSeg, &search);
            }
        }
        if (nextSeg.m_segment.m_p1Pos.m_x < to.m_segment.m_p0Pos.m_x &&
            nextSeg.m_segment.projectionX(corner, foot)) {
            move = (foot - to.m_segment.m_p0Pos) + Vec2f(0.0f, -0.01f);
        } else {
            move = (nextSeg.m_segment.m_p1Pos - to.m_segment.m_p0Pos) + Vec2f(0.0f, -0.01f);
        }
        if (move.m_y < 0.0f) {
            hit = true;
        }
    }
    if (hit) {
        rhombus2->move(move);
        *rate = 1.0f;
        updateTouchLine(4, lineWork, 1.0f);
    }
}

// 0x8012E474: dispatches to the two corner sweeps of the up line (neighbour lines checked for type and corner state).
int grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXUp(grCollisionLineWork* lineWork, float* rate) {
    grCollSegmentEx unused;
    fetchSegment(&unused, lineWork);
    bool valid = false;
    grCollisionLineWork prev;
    grCollisionLineWork next;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    if ((m_hitDirs & 10) == 0) {
        if (next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL) {
            valid = true;
        }
        bool active;
        if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        grCollisionLineWork* nextLw = &next; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check above)
        if (active && nextLw->m_collisionLine->m_isUnder) {
            int still = m_unmovedMask & 6; // MATCH-ONLY: the original compares the mask against 'all bits' and against 0
            if (still != 6 && still == 0) {
                testCollisionMoveSlatingLineMoveXUpRight(lineWork, rate);
            }
        }
    }
    if ((m_hitDirs & 6) == 0) {
        valid = false;
        if (prev.m_collisionLine != NULL && prev.m_collisionJoint != NULL && prev.m_collision != NULL) {
            valid = true;
        }
        bool active;
        if (valid && prev.m_collisionJoint->m_0x54_4 && prev.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        grCollisionLineWork* prevLw = &prev; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check above)
        if (active && prevLw->m_collisionLine->m_isUnder) {
            int still = m_unmovedMask & 5; // MATCH-ONLY: the original compares the mask against 'all bits' and against 0
            if (still != 5 && still == 0) {
                testCollisionMoveSlatingLineMoveXUpLeft(lineWork, rate);
            }
        }
    }
    return 1;
}

// 0x8012E67C: sweeps the up/right corner pair against the line itself (no neighbour line).
void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXUpRight(grCollisionLineWork* lineWork, float* rate) {
    bool sameFirst = false;
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx seg;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollisionLineWork search;
    Vec2f foot;
    Vec2f move;
    fetchSegment(&seg, lineWork);
    from.m_segment.m_p0Pos = m_shape1.m_rhombus.m_upPos;
    from.m_segment.m_p1Pos = m_shape1.m_rhombus.m_rightPos;
    from.m_unk10 = 0;
    to.m_segment.m_p0Pos = rhombus2->m_upPos;
    to.m_segment.m_p1Pos = rhombus2->m_rightPos;
    to.m_unk10 = 0;
    if (from.m_segment.m_p0Pos.m_x == to.m_segment.m_p0Pos.m_x && from.m_segment.m_p0Pos.m_y == to.m_segment.m_p0Pos.m_y) {
        sameFirst = true;
    }
    if (sameFirst) {
        return;
    }
    bool unchanged = false;
    if (from.m_segment.m_p1Pos.m_x == to.m_segment.m_p1Pos.m_x && from.m_segment.m_p1Pos.m_y == to.m_segment.m_p1Pos.m_y) {
        unchanged = true;
    }
    if (unchanged) {
        return;
    }
    quad.set(from.m_segment, to.m_segment);
    if (quad.checkIntersect(seg.m_segment) == 0) {
        return;
    }
    if (quad.checkInclude(seg.m_segment.m_p1Pos) == 0) {
        return;
    }
    bool hit = false;
    if (to.m_segment.m_p0Pos.m_x < seg.m_segment.m_p1Pos.m_x) {
        if (to.m_segment.projectionX(seg.m_segment.m_p1Pos, foot)) {
            move = (seg.m_segment.m_p1Pos - foot) + Vec2f(0.0f, -0.01f);
            if (move.m_y < 0.0f) {
                hit = true;
            }
        }
    } else {
        const Vec2f& corner = to.m_segment.m_p0Pos; // MATCH-ONLY: the corner's address stays in a register across the calls
        bool usable = false;
        if (to.m_segment.m_p0Pos.m_x <= seg.m_segment.m_p0Pos.m_x) {
            usable = true;
        } else if (lineWork->getPosXUpLine(to.m_segment.m_p0Pos.m_x, &search)) {
            if (isActiveLineWorkStaged(search)) {
                fetchSegment(&seg, &search);
                usable = true;
            }
        }
        if (usable && seg.m_segment.projectionX(corner, foot)) {
            move = (foot - to.m_segment.m_p0Pos) + Vec2f(0.0f, -0.01f);
            if (move.m_y < 0.0f) {
                hit = true;
            }
        }
    }
    if (hit) {
        rhombus2->move(move);
        *rate = 1.0f;
        updateTouchLine(7, lineWork, 1.0f);
    }
}

// 0x8012E9FC: sweeps the up/left corner pair against the line itself (no neighbour line).
void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXUpLeft(grCollisionLineWork* lineWork, float* rate) {
    bool sameFirst = false;
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx seg;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollisionLineWork search;
    Vec2f foot;
    Vec2f move;
    fetchSegment(&seg, lineWork);
    from.m_segment.m_p0Pos = m_shape1.m_rhombus.m_upPos;
    from.m_segment.m_p1Pos = m_shape1.m_rhombus.m_leftPos;
    from.m_unk10 = 0;
    to.m_segment.m_p0Pos = rhombus2->m_upPos;
    to.m_segment.m_p1Pos = rhombus2->m_leftPos;
    to.m_unk10 = 0;
    if (from.m_segment.m_p0Pos.m_x == to.m_segment.m_p0Pos.m_x && from.m_segment.m_p0Pos.m_y == to.m_segment.m_p0Pos.m_y) {
        sameFirst = true;
    }
    if (sameFirst) {
        return;
    }
    bool unchanged = false;
    if (from.m_segment.m_p1Pos.m_x == to.m_segment.m_p1Pos.m_x && from.m_segment.m_p1Pos.m_y == to.m_segment.m_p1Pos.m_y) {
        unchanged = true;
    }
    if (unchanged) {
        return;
    }
    quad.set(from.m_segment, to.m_segment);
    if (quad.checkIntersect(seg.m_segment) == 0) {
        return;
    }
    if (quad.checkInclude(seg.m_segment.m_p0Pos) == 0) {
        return;
    }
    bool hit = false;
    if (seg.m_segment.m_p0Pos.m_x < to.m_segment.m_p0Pos.m_x) {
        if (to.m_segment.projectionX(seg.m_segment.m_p0Pos, foot)) {
            move = (seg.m_segment.m_p0Pos - foot) + Vec2f(0.0f, -0.01f);
            if (move.m_y < 0.0f) {
                hit = true;
            }
        }
    } else {
        const Vec2f& corner = to.m_segment.m_p0Pos; // MATCH-ONLY: the corner's address stays in a register across the calls
        bool usable = false;
        if (seg.m_segment.m_p1Pos.m_x <= to.m_segment.m_p0Pos.m_x) {
            usable = true;
        } else if (lineWork->getPosXUpLine(to.m_segment.m_p0Pos.m_x, &search)) {
            if (isActiveLineWorkStaged(search)) {
                fetchSegment(&seg, &search);
                usable = true;
            }
        }
        if (usable && seg.m_segment.projectionX(corner, foot)) {
            move = (foot - to.m_segment.m_p0Pos) + Vec2f(0.0f, -0.01f);
            if (move.m_y < 0.0f) {
                hit = true;
            }
        }
    }
    if (hit) {
        rhombus2->move(move);
        *rate = 1.0f;
        updateTouchLine(7, lineWork, 1.0f);
    }
}

// 0x8012ED7C: dispatches to the two corner sweeps of the down line (neighbour lines checked for type and corner state).
int grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXDown(grCollisionLineWork* lineWork, float* rate) {
    grCollSegmentEx unused;
    fetchSegment(&unused, lineWork);
    bool valid = false;
    grCollisionLineWork prev;
    grCollisionLineWork next;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    if ((m_hitDirs & 8) == 0) {
        if (prev.m_collisionLine != NULL && prev.m_collisionJoint != NULL && prev.m_collision != NULL) {
            valid = true;
        }
        bool active;
        if (valid && prev.m_collisionJoint->m_0x54_4 && prev.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        grCollisionLineWork* prevLw = &prev; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check above)
        if (active && prevLw->m_collisionLine->m_isUpper) {
            int still = m_unmovedMask & 6; // MATCH-ONLY: the original compares the mask against 'all bits' and against 0
            if (still != 6 && still == 0) {
                testCollisionMoveSlatingLineMoveXDownRight(lineWork, rate);
            }
        }
    }
    if ((m_hitDirs & 8) == 0) {
        valid = false;
        if (next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL) {
            valid = true;
        }
        bool active;
        if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        grCollisionLineWork* nextLw = &next; // MATCH-ONLY: the line flag is read through a pointer to the local (no CSE with the check above)
        if (active && nextLw->m_collisionLine->m_isUpper) {
            int still = m_unmovedMask & 9; // MATCH-ONLY: the original compares the mask against 'all bits' and against 0
            if (still != 9 && still == 0) {
                testCollisionMoveSlatingLineMoveXDownLeft(lineWork, rate);
            }
        }
    }
    return 1;
}

// 0x8012EF84: sweeps the down/right corner pair against the line itself (no neighbour line).
void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXDownRight(grCollisionLineWork* lineWork, float* rate) {
    bool hit = false;
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx seg;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollisionLineWork search;
    grCollisionLineWork prev;
    Vec2f foot;
    Vec2f move;
    fetchSegment(&seg, lineWork);
    lineWork->getPrev(&prev);
    from.m_segment.m_p0Pos = m_shape1.m_rhombus.m_downPos;
    from.m_segment.m_p1Pos = m_shape1.m_rhombus.m_rightPos;
    from.m_unk10 = 0;
    to.m_segment.m_p0Pos = rhombus2->m_downPos;
    to.m_segment.m_p1Pos = rhombus2->m_rightPos;
    to.m_unk10 = 0;
    bool sameFirst = false;
    if (from.m_segment.m_p0Pos.m_x == to.m_segment.m_p0Pos.m_x && from.m_segment.m_p0Pos.m_y == to.m_segment.m_p0Pos.m_y) {
        sameFirst = true;
    }
    if (sameFirst) {
        return;
    }
    bool unchanged = false;
    if (from.m_segment.m_p1Pos.m_x == to.m_segment.m_p1Pos.m_x && from.m_segment.m_p1Pos.m_y == to.m_segment.m_p1Pos.m_y) {
        unchanged = true;
    }
    if (unchanged) {
        return;
    }
    quad.set(from.m_segment, to.m_segment);
    if (quad.checkIntersect(seg.m_segment) == 0) {
        return;
    }
    if (quad.checkInclude(seg.m_segment.m_p0Pos) == 0) {
        return;
    }
    if (to.m_segment.m_p0Pos.m_x < seg.m_segment.m_p0Pos.m_x) {
        if (to.m_segment.projectionX(seg.m_segment.m_p0Pos, foot)) {
            move = (seg.m_segment.m_p0Pos - foot) + Vec2f(0.0f, 0.01f);
            if (move.m_y > 0.0f) {
                hit = true;
            }
        }
    } else {
        const Vec2f& corner = to.m_segment.m_p0Pos; // MATCH-ONLY: the corner's address stays in a register across the calls
        bool usable = false;
        if (to.m_segment.m_p0Pos.m_x <= seg.m_segment.m_p1Pos.m_x) {
            usable = true;
        } else if (lineWork->getPosXDownLine(to.m_segment.m_p0Pos.m_x, &search)) {
            if (isActiveLineWorkStaged(search)) {
                fetchSegment(&seg, &search);
                usable = true;
            }
        }
        if (usable && seg.m_segment.projectionX(corner, foot)) {
            move = (foot - to.m_segment.m_p0Pos) + Vec2f(0.0f, 0.01f);
            if (move.m_y > 0.0f) {
                hit = true;
            }
        }
    }
    if (hit) {
        rhombus2->move(move);
        *rate = 1.0f;
        updateTouchLine(7, lineWork, 1.0f);
    }
}

// 0x8012F32C: sweeps the down/left corner pair against the line itself (no neighbour line).
void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveXDownLeft(grCollisionLineWork* lineWork, float* rate) {
    bool sameFirst = false;
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx seg;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollisionLineWork search;
    Vec2f foot;
    Vec2f move;
    fetchSegment(&seg, lineWork);
    from.m_segment.m_p0Pos = m_shape1.m_rhombus.m_downPos;
    from.m_segment.m_p1Pos = m_shape1.m_rhombus.m_leftPos;
    from.m_unk10 = 0;
    to.m_segment.m_p0Pos = rhombus2->m_downPos;
    to.m_segment.m_p1Pos = rhombus2->m_leftPos;
    to.m_unk10 = 0;
    if (from.m_segment.m_p0Pos.m_x == to.m_segment.m_p0Pos.m_x && from.m_segment.m_p0Pos.m_y == to.m_segment.m_p0Pos.m_y) {
        sameFirst = true;
    }
    if (sameFirst) {
        return;
    }
    bool unchanged = false;
    if (from.m_segment.m_p1Pos.m_x == to.m_segment.m_p1Pos.m_x && from.m_segment.m_p1Pos.m_y == to.m_segment.m_p1Pos.m_y) {
        unchanged = true;
    }
    if (unchanged) {
        return;
    }
    quad.set(from.m_segment, to.m_segment);
    if (quad.checkIntersect(seg.m_segment) == 0) {
        return;
    }
    if (quad.checkInclude(seg.m_segment.m_p1Pos) == 0) {
        return;
    }
    bool hit = false;
    if (seg.m_segment.m_p1Pos.m_x < to.m_segment.m_p0Pos.m_x) {
        if (to.m_segment.projectionX(seg.m_segment.m_p1Pos, foot)) {
            move = (seg.m_segment.m_p1Pos - foot) + Vec2f(0.0f, 0.01f);
            if (move.m_y > 0.0f) {
                hit = true;
            }
        }
    } else {
        const Vec2f& corner = to.m_segment.m_p0Pos; // MATCH-ONLY: the corner's address stays in a register across the calls
        bool usable = false;
        if (seg.m_segment.m_p0Pos.m_x <= to.m_segment.m_p0Pos.m_x) {
            usable = true;
        } else if (lineWork->getPosXDownLine(to.m_segment.m_p0Pos.m_x, &search)) {
            if (isActiveLineWorkStaged(search)) {
                fetchSegment(&seg, &search);
                usable = true;
            }
        }
        if (usable && seg.m_segment.projectionX(corner, foot)) {
            move = (foot - to.m_segment.m_p0Pos) + Vec2f(0.0f, 0.01f);
            if (move.m_y > 0.0f) {
                hit = true;
            }
        }
    }
    if (hit) {
        rhombus2->move(move);
        *rate = 1.0f;
        updateTouchLine(7, lineWork, 1.0f);
    }
}


// ===== cluster R4 =====

// ===== cluster R4: grCollisionTestRhombus::testCollisionMoveSlatingLineMoveY* (0x8012F6AC-0x80132004) =====
//
// MoveSlatingLine: the dynamic test of a wall / floor corner against a line that slides (testCollisionNotMoveSlatingLine).
// The Y dispatchers look at the neighbour lines and call up to two corner variants; a corner variant sweeps the quad between a
// pair of corners of m_shape1 (before the move) and m_shape2 (after the move) against the line; on a hit it pushes m_shape2
// back along the line (clRhombus2D::move) and records the touch.

// The neighbour line work is valid and its joint and collision are enabled (dispatchers).
static inline bool isEnabledNeighbor(const grCollisionLineWork& lw) {
    bool valid = false;
    if (lw.m_collisionLine != NULL && lw.m_collisionJoint != NULL && lw.m_collision != NULL) {
        valid = true;
    }
    if (valid && lw.m_collisionJoint->m_0x54_4 && lw.m_collision->m_isEnabled) {
        return true;
    }
    return false;
}

// The line work has a line, a joint and a collision (two-stage check: the line search results of the corner variants).
static inline bool isValidLineWork(const grCollisionLineWork& lw) {
    bool stage2 = false;
    bool stage1 = false;
    stage1 = lw.m_collisionLine != NULL && lw.m_collisionJoint != NULL;
    if (stage1 && lw.m_collision != NULL) {
        stage2 = true;
    }
    return stage2;
}

// The line work found by a line search is valid and its joint and its collision are enabled.
static inline bool isEnabledLineWork(const grCollisionLineWork& lw) {
    bool enabled;
    if (isValidLineWork(lw) && lw.m_collisionJoint->m_0x54_4 && lw.m_collision->m_isEnabled) {
        enabled = true;
    } else {
        enabled = false;
    }
    return enabled;
}

// Two corner positions are exactly equal (the corner did not move this pass).
static inline bool isSamePos(const Vec2f& a, const Vec2f& b) {
    bool same = false;
    if (a.m_x == b.m_x && a.m_y == b.m_y) {
        same = true;
    }
    return same;
}

// Builds the vertex pair (a corner of the vertical axis, a corner of the horizontal axis) that clQuad2D::set takes.
static inline void setCornerPair(grCollSegmentEx& pair, const Vec2f& first, const Vec2f& second) {
    pair.m_segment.m_p0Pos = first;
    pair.m_segment.m_p1Pos = second;
    pair.m_unk10 = 0;
}

// MATCH-ONLY: the first neighbour check of a dispatcher keeps its first-stage flag (`valid`, declared at the top of the function)
// in a register of its own; the second one is the plain isEnabledNeighbor(). The CSE of the neighbour's line pointer between the
// null check and the `m_isUnder` / `m_isUpper` test does not happen in the original (it reloads it); not reproduced yet
// (dispatchers: 89-90 % because of the two extra reloads and the prologue schedule).
#define NEIGHBOR_ENABLED(lw) \
    (lw.m_collisionLine != NULL && lw.m_collisionJoint != NULL && lw.m_collision != NULL)

int grCollisionTestRhombus::testCollisionMoveSlatingLineMoveYRight(grCollisionLineWork* lineWork, float* rate) {
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    bool valid = false;
    grCollisionLineWork prev;
    grCollisionLineWork next;
    if (!(m_hitDirs & (DIR_RIGHT | DIR_DOWN))) {
        lineWork->getNext(&next);
        if (NEIGHBOR_ENABLED(next)) {
            valid = true;
        }
        bool enabled;
        if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
            enabled = true;
        } else {
            enabled = false;
        }
        if (enabled && next.m_collisionLine->m_isUnder && (m_unmovedMask & 6) != 6 && (m_unmovedMask & 6) == 0) {
            testCollisionMoveSlatingLineMoveYRightDown(lineWork, rate);
        }
    }
    if (!(m_hitDirs & (DIR_RIGHT | DIR_UP))) {
        lineWork->getPrev(&prev);
        if (isEnabledNeighbor(prev) && prev.m_collisionLine->m_isUpper && (m_unmovedMask & 5) != 5 && (m_unmovedMask & 5) == 0) {
            testCollisionMoveSlatingLineMoveYRightUp(lineWork, rate);
        }
    }
    return 1;
}

int grCollisionTestRhombus::testCollisionMoveSlatingLineMoveYLeft(grCollisionLineWork* lineWork, float* rate) {
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    bool valid = false;
    grCollisionLineWork prev;
    grCollisionLineWork next;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    if (!(m_hitDirs & (DIR_LEFT | DIR_DOWN))) {
        if (NEIGHBOR_ENABLED(prev)) {
            valid = true;
        }
        bool enabled;
        if (valid && prev.m_collisionJoint->m_0x54_4 && prev.m_collision->m_isEnabled) {
            enabled = true;
        } else {
            enabled = false;
        }
        if (enabled && prev.m_collisionLine->m_isUnder && (m_unmovedMask & 10) != 10 && (m_unmovedMask & 10) == 0) {
            testCollisionMoveSlatingLineMoveYLeftDown(lineWork, rate);
        }
    }
    if (!(m_hitDirs & (DIR_LEFT | DIR_UP))) {
        if (isEnabledNeighbor(next) && next.m_collisionLine->m_isUpper && (m_unmovedMask & 9) != 9 && (m_unmovedMask & 9) == 0) {
            testCollisionMoveSlatingLineMoveYLeftUp(lineWork, rate);
        }
    }
    return 1;
}

int grCollisionTestRhombus::testCollisionMoveSlatingLineMoveYUp(grCollisionLineWork* lineWork, float* rate) {
    if (!(m_hitDirs & (DIR_RIGHT | DIR_UP))) {
        bool valid = false;
        grCollisionLineWork next;
        lineWork->getNext(&next);
        if (NEIGHBOR_ENABLED(next)) {
            valid = true;
        }
        bool enabled;
        if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
            enabled = true;
        } else {
            enabled = false;
        }
        if (enabled && next.m_collisionLine->m_isUnder && (m_unmovedMask & 5) != 5 && (m_unmovedMask & 5) == 0) {
            testCollisionMoveSlatingLineMoveYUpRight(lineWork, rate);
        }
    }
    if (!(m_hitDirs & (DIR_LEFT | DIR_UP))) {
        bool valid = false;
        grCollisionLineWork prev;
        lineWork->getPrev(&prev);
        if (NEIGHBOR_ENABLED(prev)) {
            valid = true;
        }
        bool enabled;
        if (valid && prev.m_collisionJoint->m_0x54_4 && prev.m_collision->m_isEnabled) {
            enabled = true;
        } else {
            enabled = false;
        }
        if (enabled && prev.m_collisionLine->m_isUnder && (m_unmovedMask & 9) != 9 && (m_unmovedMask & 9) == 0) {
            testCollisionMoveSlatingLineMoveYUpLeft(lineWork, rate);
        }
    }
    return 1;
}

int grCollisionTestRhombus::testCollisionMoveSlatingLineMoveYDown(grCollisionLineWork* lineWork, float* rate) {
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    bool valid = false;
    grCollisionLineWork prev;
    grCollisionLineWork next;
    lineWork->getPrev(&prev);
    lineWork->getNext(&next);
    if (!(m_hitDirs & (DIR_RIGHT | DIR_DOWN))) {
        if (NEIGHBOR_ENABLED(prev)) {
            valid = true;
        }
        bool enabled;
        if (valid && prev.m_collisionJoint->m_0x54_4 && prev.m_collision->m_isEnabled) {
            enabled = true;
        } else {
            enabled = false;
        }
        if (enabled && prev.m_collisionLine->m_isUpper && (m_unmovedMask & 6) != 6 && (m_unmovedMask & 5) == 0) {
            testCollisionMoveSlatingLineMoveYDownRight(lineWork, rate);
        }
    }
    if (!(m_hitDirs & (DIR_LEFT | DIR_DOWN))) {
        if (isEnabledNeighbor(next) && next.m_collisionLine->m_isUpper && (m_unmovedMask & 10) != 10 && (m_unmovedMask & 9) == 0) {
            testCollisionMoveSlatingLineMoveYDownLeft(lineWork, rate);
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------------------
// Corner variants. The 16 MoveSlatingLine corner variants share one skeleton (see the layout notes): sweep the quad between a pair of
// corners of m_shape1 (before) and m_shape2 (after the move) against the line; if the line is hit, compute the offset that puts the
// m_shape2 corner back on the line, move m_shape2 by it and record the touch. The X family (cluster R3) is the same with the axes
// exchanged.
//
// MATCH-ONLY: the Y family is generated from two macros, one per vertical corner (down / up). A function template (always_inline
// member or free function) or helper functions that return the side-dependent value change the register allocation and the stack
// layout of the inlined body, so the skeleton is spelled out once per vertical corner and instantiated with the per-variant parameters:
//
//   variant      SIDE_CORNER  SIDE_END  FAR_END  MARGIN   PUSHED  SEARCH (line past the end)   TOUCH_ID
//   YRightDown   m_rightPos   m_p1Pos   m_p0Pos  -0.01f   <       getPosYRightLine (80162F4)   7
//   YLeftDown    m_leftPos    m_p0Pos   m_p1Pos  +0.01f   >       getPosYLeftLine              6
//   YRightUp     m_rightPos   m_p1Pos   m_p0Pos  -0.01f   <       getPosYRightLine             5
//   YLeftUp      m_leftPos    m_p0Pos   m_p1Pos  +0.01f   >       getPosYLeftLine              4
//
// SIDE_END is the end of the line on the side of the horizontal corner, FAR_END the opposite one. Down tests the SIDE_END against the
// swept corner and falls back to the line past the FAR_END; Up tests the FAR_END and falls back to the line past the SIDE_END.
// ---------------------------------------------------------------------------------------------------------------------------

// Down corner pair against the line.
#define MOVE_SLATING_Y_DOWN(NAME, SIDE_CORNER, SIDE_END, FAR_END, MARGIN, PUSHED, SEARCH, TOUCH_ID, NEXT_FIRST, NEXT_LATER) \
void grCollisionTestRhombus::NAME(grCollisionLineWork* lineWork, float* rate) { \
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus; \
    clQuad2D quad; \
    NEXT_FIRST \
    grCollSegmentEx segment; \
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0); \
    NEXT_LATER \
    grCollSegmentEx from; \
    setCornerPair(from, m_shape1.m_rhombus.m_downPos, m_shape1.m_rhombus.SIDE_CORNER); \
    grCollSegmentEx to; \
    setCornerPair(to, rhombus2->m_downPos, rhombus2->SIDE_CORNER); \
    if (!isSamePos(from.m_segment.m_p0Pos, to.m_segment.m_p0Pos) && !isSamePos(from.m_segment.m_p1Pos, to.m_segment.m_p1Pos)) { \
        quad.set(from.m_segment, to.m_segment); \
        if (quad.checkIntersect(segment.m_segment) && quad.checkInclude(segment.m_segment.SIDE_END)) { \
            Vec2f onLine; \
            Vec2f move; \
            if (segment.m_segment.SIDE_END.m_y < to.m_segment.m_p1Pos.m_y) { \
                if (to.m_segment.projectionY(segment.m_segment.SIDE_END, onLine)) { \
                    move = (segment.m_segment.SIDE_END - onLine) + Vec2f(MARGIN, 0.0f); \
                    if (move.m_x PUSHED 0.0f) { \
                        hit = 1; \
                    } \
                } \
            } else { \
                bool use = false; \
                const Vec2f& corner = to.m_segment.m_p1Pos; \
                if (to.m_segment.m_p1Pos.m_y >= segment.m_segment.FAR_END.m_y) { \
                    use = true; \
                } else if (lineWork->SEARCH(corner.m_y, &next)) { \
                    if (isEnabledLineWork(next)) { \
                        fn_80111968(next.m_collision, &segment, next.m_collisionLine, 0); \
                        use = true; \
                    } \
                } \
                if (use) { \
                    if (segment.m_segment.projectionY(corner, onLine)) { \
                        move = (onLine - corner) + Vec2f(MARGIN, 0.0f); \
                        if (move.m_x PUSHED 0.0f) { \
                            hit = 1; \
                        } \
                    } \
                } \
            } \
            if (hit) { \
                rhombus2->move(move); \
                *rate = 1.0f; \
                updateTouchLine(TOUCH_ID, lineWork, 1.0f); \
            } \
        } \
    } \
}

// Up corner pair against the line. Unlike the Down pair the line is first followed to its next segment when it ends below the corner,
// and the fallback pushes the corner onto the end of the line.
#define MOVE_SLATING_Y_UP(NAME, SIDE_CORNER, SIDE_END, FAR_END, MARGIN, PUSHED, SEARCH, TOUCH_ID) \
void grCollisionTestRhombus::NAME(grCollisionLineWork* lineWork, float* rate) { \
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus; \
    clQuad2D quad; \
    grCollisionLineWork next; \
    int hit = 0; \
    grCollSegmentEx segment; \
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0); \
    grCollSegmentEx from; \
    setCornerPair(from, m_shape1.m_rhombus.m_upPos, m_shape1.m_rhombus.SIDE_CORNER); \
    grCollSegmentEx to; \
    setCornerPair(to, rhombus2->m_upPos, rhombus2->SIDE_CORNER); \
    if (!isSamePos(from.m_segment.m_p0Pos, to.m_segment.m_p0Pos) && !isSamePos(from.m_segment.m_p1Pos, to.m_segment.m_p1Pos)) { \
        quad.set(from.m_segment, to.m_segment); \
        if (quad.checkIntersect(segment.m_segment) && quad.checkInclude(segment.m_segment.FAR_END)) { \
            Vec2f onLine; \
            Vec2f move; \
            if (segment.m_segment.FAR_END.m_y > to.m_segment.m_p1Pos.m_y) { \
                if (to.m_segment.projectionY(segment.m_segment.FAR_END, onLine)) { \
                    move = (segment.m_segment.FAR_END - onLine) + Vec2f(MARGIN, 0.0f); \
                    if (move.m_x PUSHED 0.0f) { \
                        hit = 1; \
                    } \
                } \
            } else { \
                const Vec2f& corner = to.m_segment.m_p1Pos; \
                if (segment.m_segment.SIDE_END.m_y < to.m_segment.m_p1Pos.m_y) { \
                    lineWork->SEARCH(corner.m_y, &next); \
                    if (isEnabledLineWork(next)) { \
                        fn_80111968(next.m_collision, &segment, next.m_collisionLine, 0); \
                    } \
                } \
                if (to.m_segment.m_p1Pos.m_y < segment.m_segment.SIDE_END.m_y && segment.m_segment.projectionY(corner, onLine)) { \
                    move = (onLine - corner) + Vec2f(MARGIN, 0.0f); \
                } else { \
                    move = (segment.m_segment.SIDE_END - corner) + Vec2f(MARGIN, 0.0f); \
                } \
                if (move.m_x PUSHED 0.0f) { \
                    hit = 1; \
                } \
            } \
            if (hit) { \
                rhombus2->move(move); \
                *rate = 1.0f; \
                updateTouchLine(TOUCH_ID, lineWork, 1.0f); \
            } \
        } \
    } \
}

MOVE_SLATING_Y_DOWN(testCollisionMoveSlatingLineMoveYRightDown, m_rightPos, m_p1Pos, m_p0Pos, -0.01f, <, getPosYRightLine, 7, , grCollisionLineWork next; int hit = 0;)
MOVE_SLATING_Y_UP(testCollisionMoveSlatingLineMoveYRightUp, m_rightPos, m_p1Pos, m_p0Pos, -0.01f, <, getPosYRightLine, 5)
MOVE_SLATING_Y_DOWN(testCollisionMoveSlatingLineMoveYLeftDown, m_leftPos, m_p0Pos, m_p1Pos, 0.01f, >, getPosYLeftLine, 6, grCollisionLineWork next; int hit = 0;, )
MOVE_SLATING_Y_UP(testCollisionMoveSlatingLineMoveYLeftUp, m_leftPos, m_p0Pos, m_p1Pos, 0.01f, >, getPosYLeftLine, 4)

// 0x80132000: a 4-byte tail branch (the compound test of the rhombus is the plain not-moving test).
int grCollisionTestRhombus::testCollisionCompound(grCollisionLineWork* lineWork) {
    return grCollisionTestRhombus::testCollisionNotMove(lineWork);
}

void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveYUpRight(grCollisionLineWork* lineWork, float* rate) {
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx segment;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollSegmentEx segment2;
    int hit = 0;
    grCollisionLineWork neighbor;
    grCollisionLineWork next;
    lineWork->getNext(&neighbor);
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    setCornerPair(from, m_shape1.m_rhombus.m_upPos, m_shape1.m_rhombus.m_rightPos);
    setCornerPair(to, rhombus2->m_upPos, rhombus2->m_rightPos);
    if (!isSamePos(from.m_segment.m_p0Pos, to.m_segment.m_p0Pos) && !isSamePos(from.m_segment.m_p1Pos, to.m_segment.m_p1Pos)) {
        quad.set(from.m_segment, to.m_segment);
        if (quad.checkIntersect(segment.m_segment) && quad.checkInclude(segment.m_segment.m_p1Pos)) {
            Vec2f onLine;
            Vec2f move;
            if (segment.m_segment.m_p1Pos.m_y > to.m_segment.m_p1Pos.m_y) {
                if (to.m_segment.projectionY(segment.m_segment.m_p1Pos, onLine)) {
                    move = (segment.m_segment.m_p1Pos - onLine) + Vec2f(0.0f, -0.01f);
                    if (move.m_x < 0.0f) {
                        hit = 1;
                    }
                }
            } else {
                fn_80111968(neighbor.m_collision, &segment2, neighbor.m_collisionLine, 0);
                const Vec2f* pos = &rhombus2->m_downPos;
                if (rhombus2->m_downPos.m_y < segment2.m_segment.m_p0Pos.m_y) {
                    if (to.m_segment.projectionY(*pos, onLine)) {
                        move = (*pos - onLine) + Vec2f(0.0f, 0.01f);
                        if (move.m_x > 0.0f) {
                            hit = 1;
                        }
                    }
                } else {
                    bool use = false;
                    if (pos->m_x <= segment2.m_segment.m_p1Pos.m_x) {
                        use = true;
                    } else if (neighbor.getPosYRightLine(pos->m_y, &next)) {
                        if (isEnabledLineWork(next)) {
                            fn_80111968(next.m_collision, &segment2, next.m_collisionLine, 0);
                            use = true;
                        }
                    }
                    if (use) {
                        if (segment2.m_segment.projectionY(*pos, onLine)) {
                            move = (onLine - *pos) + Vec2f(0.0f, 0.01f);
                            if (move.m_x > 0.0f) {
                                hit = 1;
                            }
                        }
                    }
                }
            }
            if (hit) {
                rhombus2->move(move);
                *rate = 1.0f;
                updateTouchLine(5, lineWork, 1.0f);
            }
        }
    }
}

void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveYUpLeft(grCollisionLineWork* lineWork, float* rate) {
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx segment;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollSegmentEx segment2;
    int hit = 0;
    grCollisionLineWork neighbor;
    grCollisionLineWork next;
    lineWork->getPrev(&neighbor);
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    setCornerPair(from, m_shape1.m_rhombus.m_upPos, m_shape1.m_rhombus.m_leftPos);
    setCornerPair(to, rhombus2->m_upPos, rhombus2->m_leftPos);
    if (!isSamePos(from.m_segment.m_p0Pos, to.m_segment.m_p0Pos) && !isSamePos(from.m_segment.m_p1Pos, to.m_segment.m_p1Pos)) {
        quad.set(from.m_segment, to.m_segment);
        if (quad.checkIntersect(segment.m_segment) && quad.checkInclude(segment.m_segment.m_p0Pos)) {
            Vec2f onLine;
            Vec2f move;
            if (segment.m_segment.m_p0Pos.m_y > to.m_segment.m_p1Pos.m_y) {
                if (to.m_segment.projectionY(segment.m_segment.m_p0Pos, onLine)) {
                    move = (segment.m_segment.m_p0Pos - onLine) + Vec2f(0.01f, 0.0f);
                    if (move.m_x > 0.0f) {
                        hit = 1;
                    }
                }
            } else {
                // HYPOTHESIS: the second corner pair of the left variant is built from m_shape1 (the right variant keeps the first pair).
                setCornerPair(to, m_shape1.m_rhombus.m_downPos, m_shape1.m_rhombus.m_leftPos);
                fn_80111968(neighbor.m_collision, &segment2, neighbor.m_collisionLine, 0);
                const Vec2f& pos = rhombus2->m_downPos;
                if (rhombus2->m_downPos.m_y < segment2.m_segment.m_p1Pos.m_y) {
                    if (to.m_segment.projectionY(pos, onLine)) {
                        move = (pos - onLine) + Vec2f(0.01f, 0.0f);
                        if (move.m_x > 0.0f) {
                            hit = 1;
                        }
                    }
                } else {
                    bool use = false;
                    if (segment2.m_segment.m_p1Pos.m_x <= pos.m_x) {
                        use = true;
                    } else if (neighbor.getPosYLeftLine(pos.m_y, &next)) {
                        if (isEnabledLineWork(next)) {
                            fn_80111968(next.m_collision, &segment2, next.m_collisionLine, 0);
                            use = true;
                        }
                    }
                    if (use) {
                        if (segment2.m_segment.projectionY(pos, onLine)) {
                            move = (onLine - pos) + Vec2f(0.0f, 0.01f);
                            if (move.m_x > 0.0f) {
                                hit = 1;
                            }
                        }
                    }
                }
            }
            if (hit) {
                rhombus2->move(move);
                *rate = 1.0f;
                updateTouchLine(5, lineWork, 1.0f);
            }
        }
    }
}

void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveYDownRight(grCollisionLineWork* lineWork, float* rate) {
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx segment;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollSegmentEx segment2;
    int hit = 0;
    grCollisionLineWork neighbor;
    grCollisionLineWork next;
    lineWork->getPrev(&neighbor);
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    fn_80111968(neighbor.m_collision, &segment2, neighbor.m_collisionLine, 0);
    setCornerPair(from, m_shape1.m_rhombus.m_downPos, m_shape1.m_rhombus.m_rightPos);
    setCornerPair(to, rhombus2->m_downPos, rhombus2->m_rightPos);
    if (!isSamePos(from.m_segment.m_p0Pos, to.m_segment.m_p0Pos) && !isSamePos(from.m_segment.m_p1Pos, to.m_segment.m_p1Pos)) {
        quad.set(from.m_segment, to.m_segment);
        if (quad.checkIntersect(segment.m_segment) && quad.checkInclude(segment.m_segment.m_p0Pos)) {
            Vec2f onLine;
            Vec2f move;
            if (to.m_segment.m_p1Pos.m_y > segment.m_segment.m_p0Pos.m_y) {
                if (to.m_segment.projectionY(segment.m_segment.m_p0Pos, onLine)) {
                    move = (segment.m_segment.m_p0Pos - onLine) + Vec2f(-0.01f, 0.0f);
                    if (move.m_x < 0.0f) {
                        hit = 1;
                    }
                }
            } else if (segment2.m_segment.m_p1Pos.m_y < rhombus2->m_upPos.m_y) {
                // HYPOTHESIS: the sweep continues against the up / right edge of the target rhombus.
                to.m_segment.m_p0Pos = rhombus2->m_upPos;
                to.m_segment.m_p1Pos = rhombus2->m_rightPos;
                if (to.m_segment.projectionY(segment2.m_segment.m_p1Pos, onLine)) {
                    move = (segment2.m_segment.m_p1Pos - onLine) + Vec2f(-0.01f, 0.0f);
                    if (move.m_x < 0.0f) {
                        hit = 1;
                    }
                }
            } else {
                const Vec2f& pos = rhombus2->m_upPos;
                bool use = false;
                if (rhombus2->m_upPos.m_x <= segment2.m_segment.m_p0Pos.m_x) {
                    if (segment2.m_segment.m_p0Pos.m_y <= pos.m_y && pos.m_y <= segment2.m_segment.m_p1Pos.m_y) {
                        use = true;
                    }
                } else if (neighbor.getPosYRightLine(pos.m_y, &next)) {
                    if (isEnabledLineWork(next)) {
                        fn_80111968(next.m_collision, &segment2, next.m_collisionLine, 0);
                        use = true;
                    }
                }
                if (use) {
                    if (segment2.m_segment.projectionY(pos, onLine)) {
                        move = (onLine - pos) + Vec2f(-0.01f, 0.0f);
                        if (move.m_x < 0.0f) {
                            hit = 1;
                        }
                    }
                }
            }
            if (hit) {
                rhombus2->move(move);
                *rate = 1.0f;
                updateTouchLine(7, lineWork, 1.0f);
            }
        }
    }
}

void grCollisionTestRhombus::testCollisionMoveSlatingLineMoveYDownLeft(grCollisionLineWork* lineWork, float* rate) {
    clRhombus2D* rhombus2 = &m_shape2.m_rhombus;
    clQuad2D quad;
    grCollSegmentEx segment;
    grCollSegmentEx from;
    grCollSegmentEx to;
    grCollSegmentEx segment2;
    int hit = 0;
    grCollisionLineWork neighbor;
    grCollisionLineWork next;
    lineWork->getNext(&neighbor);
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    fn_80111968(neighbor.m_collision, &segment2, neighbor.m_collisionLine, 0);
    setCornerPair(from, m_shape1.m_rhombus.m_downPos, m_shape1.m_rhombus.m_leftPos);
    setCornerPair(to, rhombus2->m_downPos, rhombus2->m_leftPos);
    if (!isSamePos(from.m_segment.m_p0Pos, to.m_segment.m_p0Pos) && !isSamePos(from.m_segment.m_p1Pos, to.m_segment.m_p1Pos)) {
        quad.set(from.m_segment, to.m_segment);
        if (quad.checkIntersect(segment.m_segment) && quad.checkInclude(segment.m_segment.m_p1Pos)) {
            Vec2f onLine;
            Vec2f move;
            if (segment.m_segment.m_p1Pos.m_y < to.m_segment.m_p1Pos.m_y) {
                if (to.m_segment.projectionY(segment.m_segment.m_p1Pos, onLine)) {
                    move = (segment.m_segment.m_p1Pos - onLine) + Vec2f(0.01f, 0.0f);
                    if (move.m_x > 0.0f) {
                        hit = 1;
                    }
                }
            } else if (segment2.m_segment.m_p0Pos.m_y < rhombus2->m_upPos.m_y) {
                // HYPOTHESIS: the sweep continues against the up / left edge of the target rhombus.
                to.m_segment.m_p0Pos = rhombus2->m_upPos;
                to.m_segment.m_p1Pos = rhombus2->m_leftPos;
                if (to.m_segment.projectionY(segment2.m_segment.m_p0Pos, onLine)) {
                    move = (segment2.m_segment.m_p0Pos - onLine) + Vec2f(0.01f, 0.0f);
                    if (move.m_x > 0.0f) {
                        hit = 1;
                    }
                }
            } else {
                const Vec2f& pos = rhombus2->m_upPos;
                bool use = false;
                if (segment2.m_segment.m_p1Pos.m_x <= rhombus2->m_upPos.m_x) {
                    if (segment2.m_segment.m_p1Pos.m_y <= pos.m_y && pos.m_y <= segment2.m_segment.m_p0Pos.m_y) {
                        use = true;
                    }
                } else if (neighbor.getPosYLeftLine(pos.m_y, &next)) {
                    if (isEnabledLineWork(next)) {
                        fn_80111968(next.m_collision, &segment2, next.m_collisionLine, 0);
                        use = true;
                    }
                }
                if (use) {
                    if (segment2.m_segment.projectionY(pos, onLine)) {
                        move = (onLine - pos) + Vec2f(0.01f, 0.0f);
                        if (move.m_x > 0.0f) {
                            hit = 1;
                        }
                    }
                }
            }
            if (hit) {
                rhombus2->move(move);
                *rate = 1.0f;
                updateTouchLine(6, lineWork, 1.0f);
            }
        }
    }
}

