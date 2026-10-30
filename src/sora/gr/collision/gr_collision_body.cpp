// MATCH-ONLY: this unit copies whole vectors with the aggregate word copy of a plain base (see mt_vector.h).
#define MT_VEC2F_PLAIN_BASE
#include <gr/collision/gr_collision_collection.h>
#include <gr/collision/gr_collision_status.h>
#include <gr/collision/gr_collision_test.h>
#include <gr/ground.h>
#include <nw4r/math/math_arithmetic.h>
#include <revolution/OS/OSTime.h>
#include <types.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_linework.h>
#include <mt/mt_coll2d.h>
#include <gr/collision/gr_collision_manager.h>

// ===== cluster A =====

// MATCH-ONLY: passes the address of a returned vector temporary straight on (no named copy).
static inline const Vec2f* addressOf(const Vec2f& v) {
    return &v;
}

grCollisionTest::grCollisionTest(grCollStatus* status)
    : m_unk0(0.001f)
    , m_unk4(0.001f)
    , m_unk8(1.0f)
    , m_isFinished(false)
    , m_unkC_6(false)
    , m_needCollect5(true)
    , m_needCollect4(false)
    , m_unkC_3(false)
    , m_unkC_2(false)
    , m_unkC_1(false)
    , m_touchMask(0)
    , m_unkE(0)
    , m_collection(NULL)
    , m_preIgnoreNum(0)
    , m_ignoreJointNum(0)
    , m_unk2b8(NULL)
    , m_unk2bc(NULL)
    , m_unk2c0(NULL)
    , m_unk2c4(NULL)
    , m_unk2CC(2)
    , m_unk2CD(2)
    , m_unk2D4(0.0f)
    , m_unk2D8(0.0f)
    , m_unk2DC(0.0f)
    , m_unk2E0(0.0f) {
    m_collStatus = status;
    m_name = "no name";
    m_unk2C8 = 0;
    m_unk2C9 = 0;
    m_unk2CA = 0;
    m_unk2CB = 0;
    m_unkF = 0;
    m_unk10 = 0;
}

// HYPOTHESIS: grCollision::getSegment with a fourth argument (main.dol 0x80111968): fills the segment of a line.
extern "C" void fn_80111968(grCollision* collision, grCollSegmentEx* out, grCollisionLine* line, int unk);

// MATCH-ONLY: the original computes min/max of two floats with a paired-single subtract and select.
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

// MATCH-ONLY: the original measures a returned vector temporary through a by-value copy.
static inline float getLength(Vec2f vec) {
    return vec.length();
}

// HYPOTHESIS: grCollStatus::clearTouchLine (0x80134468): forgets the touch line of the given touch id.
extern "C" void fn_80134468(grCollStatus* status, int touchId);

// HYPOTHESIS: grCollision::getPreSegment1 (main.dol 0x80111AF4): like the four-argument getSegment, for the segment of the previous frame.
extern "C" void fn_80111AF4(grCollision* collision, grCollSegmentEx* out, grCollisionLine* line, int unk);

static inline int isFinite(float f) {
    int res = 1;
    u32 i;
    __memcpy(&i, &f, 4);
    if ((i >> 23 & 0xFF) == 0xFF)
        res = 0;
    return res;
}

// HYPOTHESIS: grCollision::getReferenceGround (main.dol 0x80111E78): the Ground that owns the joint of a line work.
extern "C" Ground* fn_80111E78(grCollisionLineWork* lineWork);

// A line work whose line, joint and collision are all set.
static inline bool hasLine(grCollisionLineWork* lineWork) {
    bool valid = false;
    if (lineWork->m_collisionLine != NULL && lineWork->m_collisionJoint != NULL && lineWork->m_collision != NULL) {
        valid = true;
    }
    return valid;
}

// HYPOTHESIS: grCollStatus::getTouchLine (main.dol 0x80134524): the line work of a touch id.
extern "C" grCollisionLineWork* fn_80134524(grCollStatus* status, int touchId);
// HYPOTHESIS: a 32-bit accumulator of the time test() takes (profiling).
extern u32 lbl_805A0368;

// A line work whose line, joint and collision are all set, whose joint is active and whose collision is enabled.
static inline bool isActiveLineWork(grCollisionLineWork* lineWork) {
    bool valid = lineWork->m_collisionLine != NULL && lineWork->m_collisionJoint != NULL && lineWork->m_collision != NULL;
    bool active;
    if (valid && lineWork->m_collisionJoint->m_0x54_4 && lineWork->m_collision->m_isEnabled) {
        active = true;
    } else {
        active = false;
    }
    return active;
}

// The unit diagonals (+-sqrt(1/2)) the corner touch normals start with: (x, y) pairs for upLeft, upRight, downLeft, downRight
// (main.dol 0x804094E8).
extern const float lbl_804094E8[8];

static inline void setVec2(Vec2f* vec, float x, float y) {
    vec->m_x = x;
    vec->m_y = y;
}

static inline void clearTouchRecord(grCollTouchRecord* record) {
    record->m_rate = 1.0f;
    record->m_id = 0xFF;
    record->m_lineWork.m_collisionLine = NULL;
    record->m_lineWork.m_collisionJoint = NULL;
    record->m_lineWork.m_collision = NULL;
    record->m_lineWork.m_unkC = 0;
}

static inline void clearTouchRecords(grCollisionTest* test) {
    for (int i = 0; i < grCollisionTest::TOUCH_NUM; i++) {
        clearTouchRecord(&test->m_touch[i]);
    }
    clearTouchRecord(&test->m_hit);
}

// HYPOTHESIS: the fsel based clamp helper (the same shape as the glide / mansion ones).
static inline float clampFloat(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

// HYPOTHESIS: grCollCollection::collect (gr_collision_collection.o): gathers the line works of the box into the collection.
extern "C" void fn_8011728C(grCollCollection* collection, u16 exclusiveLineAttr, int unk);

// The box swept by the shape between its current and its target position, 1 unit larger on every side. The current box is first
// widened to cover the extents of both shapes around the movement of their centres (and made at least as wide / high as the
// target box); out = that box united with the target box, grown by 1.
void grCollisionTest::getAABBox(clAABBox2D* out) {
    clAABBox2D curBox;
    m_unk2bc->getAABBox(curBox);
    clAABBox2D tgtBox;
    m_unk2c4->getAABBox(tgtBox);
    Vec2f curCenter = m_unk2bc->getCenterPos();
    Vec2f tgtCenter = m_unk2c4->getCenterPos();
    float tgtHeight = tgtBox.m_unk4 - tgtBox.m_unk2;
    float limit = curBox.m_unk4 - tgtHeight;
    if (curBox.m_unk2 > limit) {
        curBox.m_unk2 = limit;
    }
    limit = curBox.m_unk2 + tgtHeight;
    if (curBox.m_unk4 < limit) {
        curBox.m_unk4 = limit;
    }
    float curRight = curBox.m_unk3 - curCenter.m_x;
    float tgtRight = tgtBox.m_unk3 - tgtCenter.m_x;
    float curWidth = curBox.m_unk3 - curBox.m_unk1;
    float tgtWidth = tgtBox.m_unk3 - tgtBox.m_unk1;
    float curLeft = curBox.m_unk1 - curCenter.m_x;
    float tgtLeft = tgtBox.m_unk1 - tgtCenter.m_x;
    float maxRight = psMax(curRight, tgtRight);
    float maxWidth = psMax(curWidth, tgtWidth);
    float maxCenterX = psMax(curCenter.m_x, tgtCenter.m_x);
    float minCenterX = psMin(curCenter.m_x, tgtCenter.m_x);
    float minLeft = psMin(curLeft, tgtLeft);
    float right = psMax(maxRight + maxCenterX, curBox.m_unk1 + maxWidth);
    float left = psMin(minLeft + minCenterX, right - maxWidth);
    curBox.m_unk1 = left;
    curBox.m_unk3 = right;
    curBox.logicalOR(tgtBox, *out);
    out->m_unk3 = out->m_unk3 + 1.0f;
    out->m_unk4 = out->m_unk4 + 1.0f;
    out->m_unk1 = out->m_unk1 - 1.0f;
    out->m_unk2 = out->m_unk2 - 1.0f;
}

// Starts a test pass: nothing touched yet, the move direction cleared, the corner normals back to the diagonals.
void grCollisionTest::resetWork() {
    m_touchMask = 0;
    m_isFinished = false;
    m_unkF = 0xFF;
    m_unk10 = 0xFF;
    m_exclusiveLineAttr = m_collStatus->m_exclusiveLineAttr;
    m_unkE = 0;
    setVec2(&m_collStatus->m_180[0], lbl_804094E8[0], lbl_804094E8[1]);
    setVec2(&m_collStatus->m_180[1], lbl_804094E8[2], lbl_804094E8[3]);
    setVec2(&m_collStatus->m_180[2], lbl_804094E8[4], lbl_804094E8[5]);
    setVec2(&m_collStatus->m_180[3], lbl_804094E8[6], lbl_804094E8[7]);
    m_collStatus->m_prevCollShape->updateAABBox();
    m_collStatus->m_currentCollShape->updateAABBox();
    clearTouchRecords(this);
    m_ignoreJointNum = 0;
    m_collStatus->m_missStepR = false;
    m_collStatus->m_missStepL = false;
    m_collStatus->m_isCloudThroughOut = false;
    *(u8*)((u8*)m_collStatus + 0x70) = 0;
    if (m_collStatus->m_unkB == 1) {
        m_unk20 = 1;
    } else {
        m_unk20 = 0;
    }
    clearTouchRecords(this);
    m_unk2C8 = 0;
    m_unk2C9 = 0;
    m_unk2CA = 0;
    m_unk2CB = 0;
    m_unkE = 0;
    grCollStatus* status = m_collStatus;
    if (status->m_unk6C != 1) {
        status->m_touchUp = false;
        m_collStatus->m_touchLeft = false;
        m_collStatus->m_touchRight = false;
        m_collStatus->m_touchUpRight = false;
        m_collStatus->m_touchUpLeft = false;
        m_collStatus->m_touchDownRight = false;
        m_collStatus->m_touchDownLeft = false;
        grCollStatus* current = m_collStatus;
        if (current->m_setting == 2) {
            current->m_collLineWorkArray[3].m_collisionLine = NULL;
            current->m_collLineWorkArray[3].m_collisionJoint = NULL;
            current->m_collLineWorkArray[3].m_collision = NULL;
            current->m_collLineWorkArray[3].m_unkC = 0;
            m_collStatus->m_touchDown = false;
        }
    }
    m_unk2D4 = 0.0f;
    m_unk2D8 = 1.0f;
    m_unk10C = 0.0f;
}

void grCollisionTest::updateCollection(const clAABBox2D* box) {
    bool needCollect = false;
    grCollCollection* collection = m_collection;
    if (m_needCollect5 || m_needCollect4) {
        needCollect = true;
        m_needCollect5 = false;
        m_needCollect4 = false;
    } else if (!(box->m_unk3 <= collection->m_aabb.m_unk3 && box->m_unk4 <= collection->m_aabb.m_unk4
                 && box->m_unk1 >= collection->m_aabb.m_unk1 && box->m_unk2 >= collection->m_aabb.m_unk2)) {
        needCollect = true;
    }
    if (needCollect) {
        collection->reset();
        __memcpy(&collection->m_aabb, box, sizeof(clAABBox2D));
        grCollCollection* updated = m_collection;
        updated->m_unk12 = m_collStatus->m_targetLineCategory;
        updated->m_unk14 = m_collStatus->m_collision;
        updated->m_unk13 = m_collStatus->m_testDynamicCollision;
        fn_8011728C(updated, m_exclusiveLineAttr, 0);
    }
}

int grCollisionTest::getNextGroundLine(grCollisionLineWork* cur, grCollisionLineWork* out) {
    int result;
    if (m_unkE & DIR_LEFT) {
        result = cur->getPrev(out);
    } else {
        result = cur->getNext(out);
    }
    if (result) {
        grCollSegmentEx segment;
        fn_80111968(out->m_collision, &segment, out->m_collisionLine, 0);
        if (segment.m_segment.m_p0Pos.m_x > segment.m_segment.m_p1Pos.m_x) {
            result = 0;
        }
        if (!out->m_collisionLine->m_isUnder) {
            result = 0;
        } else {
            u8 sharedTarget = 0;
            if (cur->m_collisionLine->m_isTargetFighter && out->m_collisionLine->m_isTargetFighter) {
                sharedTarget = sharedTarget | 1;
            }
            if (cur->m_collisionLine->m_isTargetOther && out->m_collisionLine->m_isTargetOther) {
                sharedTarget = sharedTarget | 2;
            }
            if (cur->m_collisionLine->m_isTargetItem && out->m_collisionLine->m_isTargetItem) {
                sharedTarget = sharedTarget | 4;
            }
            if (sharedTarget == 0) {
                result = 0;
            } else if ((float)__fabs(segment.m_segment.m_p1Pos.m_x - segment.m_segment.m_p0Pos.m_x) < 0.1f) {
                result = 0;
            }
        }
    }
    return result;
}

int grCollisionTest::getNextRightLine(grCollisionLineWork* cur, grCollisionLineWork* out) {
    u8 dir = m_unkE;
    int result;
    if (dir & DIR_UP) {
        result = cur->getNext(out);
    } else {
        result = cur->getPrev(out);
    }
    if (result) {
        grCollSegmentEx segment;
        fn_80111968(out->m_collision, &segment, out->m_collisionLine, 0);
        if (segment.m_segment.m_p0Pos.m_y >= segment.m_segment.m_p1Pos.m_y) {
            result = 0;
        }
        if (!out->m_collisionLine->m_isRight) {
            result = 0;
        }
        if ((float)__fabs(segment.m_segment.m_p1Pos.m_y - segment.m_segment.m_p0Pos.m_y) < 0.1f) {
            result = 0;
        }
    }
    return result;
}

int grCollisionTest::getNextLeftLine(grCollisionLineWork* cur, grCollisionLineWork* out) {
    int result;
    if (m_unkE & DIR_DOWN) {
        result = cur->getNext(out);
    } else {
        result = cur->getPrev(out);
    }
    if (result) {
        grCollSegmentEx segment;
        fn_80111968(out->m_collision, &segment, out->m_collisionLine, 0);
        if (segment.m_segment.m_p0Pos.m_y <= segment.m_segment.m_p1Pos.m_y) {
            result = 0;
        }
        if (!out->m_collisionLine->m_isLeft) {
            result = 0;
        }
        if ((float)__fabs(segment.m_segment.m_p1Pos.m_y - segment.m_segment.m_p0Pos.m_y) < 0.1f) {
            result = 0;
        }
    }
    return result;
}

int grCollisionTest::getNextTopLine(grCollisionLineWork* cur, grCollisionLineWork* out) {
    int result;
    if (m_unkE & DIR_LEFT) {
        result = cur->getNext(out);
    } else {
        result = cur->getPrev(out);
    }
    if (result) {
        grCollSegmentEx segment;
        fn_80111968(out->m_collision, &segment, out->m_collisionLine, 0);
        if (segment.m_segment.m_p0Pos.m_x < segment.m_segment.m_p1Pos.m_x) {
            result = 0;
        }
        if (!out->m_collisionLine->m_isUpper) {
            result = 0;
        }
        if ((float)__fabs(segment.m_segment.m_p1Pos.m_x - segment.m_segment.m_p0Pos.m_x) < 0.1f) {
            result = 0;
        }
    }
    return result;
}

int grCollisionTest::checkMoveLenXDown(const Vec2f* pos, grCollisionLineWork* lineWork) {
    int reachedEnd = 0;
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    Vec2f foot;
    segment.m_segment.projection1(*pos, foot);
    float length = getLength(segment.m_segment.m_p1Pos - segment.m_segment.m_p0Pos);
    float slopeCos;
    float moveLen = m_unk10C;
    slopeCos = (segment.m_segment.m_p1Pos.m_x - segment.m_segment.m_p0Pos.m_x) / length;
    m_unk2D8 = slopeCos;
    float maxX = psMax(segment.m_segment.m_p0Pos.m_x, segment.m_segment.m_p1Pos.m_x);
    float minX = psMin(segment.m_segment.m_p0Pos.m_x, segment.m_segment.m_p1Pos.m_x);
    moveLen = m_collStatus->m_isCorrectIgnoreSlope ? moveLen : moveLen * slopeCos;
    if (m_unkE & DIR_RIGHT) {
        float newX = pos->m_x + moveLen;
        m_unk2D4 = newX;
        if (newX > maxX) {
            m_unk2D4 = maxX;
            reachedEnd = 1;
        }
    } else {
        float newX = pos->m_x - moveLen;
        m_unk2D4 = newX;
        if ((m_unkE & DIR_LEFT) && newX < minX) {
            reachedEnd = 1;
            m_unk2D4 = minX;
        }
    }
    return reachedEnd;
}

// The displacement of a (moving) line at pos: the line's movement since the previous frame, interpolated between its two
// ends by where pos projects onto the previous segment. outPos = pos + outOffset.
void grCollisionTest::calcOffsetVector(grCollisionLineWork* lineWork, const Vec2f* pos, Vec2f* outPos, Vec2f* outOffset) {
    grCollSegmentEx preSegment;
    fn_80111AF4(lineWork->m_collision, &preSegment, lineWork->m_collisionLine, 0);
    grCollSegmentEx segment;
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    Vec2f preDir = preSegment.m_segment.m_p1Pos - preSegment.m_segment.m_p0Pos;
    Vec2f preToPos = *pos - preSegment.m_segment.m_p0Pos;
    float preLengthSq = preDir.m_x * preDir.m_x + preDir.m_y * preDir.m_y;
    if (preLengthSq > 0.0001f) {
        float t = (preDir.m_y * preToPos.m_y + preDir.m_x * preToPos.m_x) / preLengthSq;
        t = nw4r::math::FSelect(t - 0.0f, t, 0.0f);
        t = nw4r::math::FSelect(t - 1.0f, 1.0f, t);
        Vec2f move0 = segment.m_segment.m_p0Pos - preSegment.m_segment.m_p0Pos;
        Vec2f move1 = segment.m_segment.m_p1Pos - preSegment.m_segment.m_p1Pos;
        *outOffset = move0 * (1.0f - t) + move1 * t;
    } else {
        Vec2f move0 = segment.m_segment.m_p0Pos - preSegment.m_segment.m_p0Pos;
        Vec2f move1 = segment.m_segment.m_p1Pos - preSegment.m_segment.m_p1Pos;
        *outOffset = (move0 + move1) * 0.5f;
    }
    *outPos = *pos + *outOffset;
}

// Puts the scratch shape's bottom point on the ground line: moving right the shape follows the line from its left end to its
// right end (and the other way round); when the end of the line was reached (endReached) the point is the end itself.
void grCollisionTest::calcNewShapeOnGround(grCollisionLineWork* lineWork, int endReached) {
    grCollSegmentEx segment;
    Vec2f from;
    Vec2f to;
    Vec2f newDown;
    Vec2f down = m_unk2c4->getDownPos();
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    if (m_unkE & DIR_RIGHT) {
        from = segment.m_segment.m_p0Pos;
        to = segment.m_segment.m_p1Pos;
        if (endReached) {
            to.m_x = to.m_x - 0.01f;
        }
    } else {
        from = segment.m_segment.m_p1Pos;
        to = segment.m_segment.m_p0Pos;
        if (endReached) {
            to.m_x = to.m_x - 0.01f;
        }
    }
    if (endReached) {
        newDown = to;
    } else {
        Vec2f target(m_unk2D4, down.m_y);
        float rate;
        float delta = to.m_x - from.m_x;
        if ((float)__fabs(delta) > 0.01f) {
            rate = (target.m_x - from.m_x) / delta;
        } else {
            rate = (target.m_y - from.m_y) / (to.m_y - from.m_y);
        }
        if (!isFinite(rate)) {
            rate = 0.0f;
        }
        newDown = from + (to - from) * rate;
    }
    m_unk2c0->setDownPos(newDown);
}

// The wall tests: the scratch shape's side point is put on the wall line (clipped to the line's end, shortened by m_unk4),
// then the target shape is pulled back (updateTarget) to the fraction of the move that reaches the wall.
void grCollisionTest::calcNewShapeOnRight(grCollisionLineWork* lineWork) {
    grCollSegmentEx segment;
    Vec2f from;
    Vec2f to;
    Vec2f newRight;
    Vec2f right = m_unk2c4->getRightPos();
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    bool clipped = false;
    if (m_unkE & DIR_UP) {
        from = segment.m_segment.m_p0Pos;
        to = segment.m_segment.m_p1Pos;
        if (right.m_y > to.m_y) {
            clipped = true;
            to.m_y = to.m_y + m_unk4;
        }
    } else {
        from = segment.m_segment.m_p1Pos;
        to = segment.m_segment.m_p0Pos;
        if (right.m_y < to.m_y) {
            clipped = true;
            to.m_y = to.m_y - m_unk4;
        }
    }
    if (clipped) {
        newRight = to;
    } else {
        float rate;
        float delta = to.m_y - from.m_y;
        if ((float)__fabs(delta) > 0.01f) {
            rate = (right.m_y - from.m_y) / delta;
        } else {
            rate = (right.m_x - from.m_x) / (to.m_x - from.m_x);
        }
        newRight = from + (to - from) * rate;
    }
    Vec2f curRight = m_unk2bc->getRightPos();
    if ((float)__fabs(right.m_y - curRight.m_y) > 0.01f) {
        float rate = (newRight.m_y - curRight.m_y) / (right.m_y - curRight.m_y);
        rate = clampFloat(rate, 0.0f, 1.0f);
        updateTarget(rate);
    }
    m_unk2c0->setRightPos(newRight);
}

void grCollisionTest::calcNewShapeOnLeft(grCollisionLineWork* lineWork) {
    grCollSegmentEx segment;
    Vec2f from;
    Vec2f to;
    Vec2f newLeft;
    Vec2f left = m_unk2c4->getLeftPos();
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    bool clipped = false;
    if (m_unkE & DIR_UP) {
        from = segment.m_segment.m_p1Pos;
        to = segment.m_segment.m_p0Pos;
        if (left.m_y > to.m_y) {
            clipped = true;
            to.m_y = to.m_y + m_unk4;
        }
    } else {
        from = segment.m_segment.m_p0Pos;
        to = segment.m_segment.m_p1Pos;
        if (left.m_y < to.m_y) {
            clipped = true;
            to.m_y = to.m_y - m_unk4;
        }
    }
    if (clipped) {
        newLeft = to;
    } else {
        float rate;
        float delta = to.m_y - from.m_y;
        if ((float)__fabs(delta) > 0.01f) {
            rate = (left.m_y - from.m_y) / delta;
        } else {
            rate = (left.m_x - from.m_x) / (to.m_x - from.m_x);
        }
        newLeft = from + (to - from) * rate;
    }
    Vec2f curLeft = m_unk2bc->getLeftPos();
    if ((float)__fabs(left.m_y - curLeft.m_y) > 0.01f) {
        float rate = (newLeft.m_y - curLeft.m_y) / (left.m_y - curLeft.m_y);
        rate = clampFloat(rate, 0.0f, 1.0f);
        updateTarget(rate);
    }
    m_unk2c0->setLeftPos(newLeft);
}

void grCollisionTest::calcNewShapeOnTop(grCollisionLineWork* lineWork) {
    grCollSegmentEx segment;
    Vec2f from;
    Vec2f to;
    Vec2f top(m_unk2c4->getTopPos());
    fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
    bool clipped = false;
    if (m_unkE & DIR_RIGHT) {
        from = segment.m_segment.m_p1Pos;
        to = segment.m_segment.m_p0Pos;
        if (top.m_x > to.m_x) {
            clipped = true;
        }
    } else {
        from = segment.m_segment.m_p0Pos;
        to = segment.m_segment.m_p1Pos;
        if (top.m_x < to.m_x) {
            clipped = true;
        }
    }
    Vec2f newTop;
    if (clipped) {
        newTop = to;
    } else {
        float rate;
        float delta = to.m_x - from.m_x;
        if ((float)__fabs(delta) > 0.01f) {
            rate = (top.m_x - from.m_x) / delta;
        } else {
            rate = (top.m_y - from.m_y) / (to.m_y - from.m_y);
        }
        newTop = from + (to - from) * rate;
    }
    m_unk2c0->setTopPos(newTop);
}

// Sends the collision messages (Ground::receiveCollMsg) to the Grounds the shape ended up touching: down (0, or 1 when the
// ground under the shape changed since the start of the test), up (2), left (3), right (4).
// mode: the grCollStatus setting at the start of test() (1 ground, 2 air), prevGround: the ground line at that time.
void grCollisionTest::callbackExec(int mode, grCollisionLineWork* prevGround) {
    int changed = 0;
    if (m_collStatus->m_touchDown) {
        grCollisionLineWork* lineWork = &m_touch[grCollStatus::Touch_Down].m_lineWork;
        bool valid = hasLine(lineWork);
        if (valid) {
            grCollisionJoint* joint = lineWork->m_collisionJoint;
            Ground* ground = fn_80111E78(lineWork);
            if (ground != NULL) {
                if (mode == 2) {
                    changed = 1;
                } else if (mode == 1) {
                    if (!hasLine(prevGround)) {
                        changed = 1;
                    } else if (fn_80111E78(prevGround) != ground) {
                        changed = 1;
                    }
                }
                ground->receiveCollMsg(changed != 0, m_collStatus, joint);
            }
        }
    }
    Ground* ground = NULL;
    grCollisionJoint* joint = NULL;
    if (m_collStatus->m_touchUp) {
        bool found = false;
        grCollisionLineWork* lineWork = &m_touch[grCollStatus::Touch_Up].m_lineWork;
        if (hasLine(lineWork)) {
            grCollisionJoint* touched = lineWork->m_collisionJoint;
            if (touched->m_ground != NULL) {
                ground = touched->m_ground;
                joint = touched;
                found = true;
            }
        }
        if (found) {
            ground->receiveCollMsg(2, m_collStatus, joint);
        }
    }
    if (m_collStatus->isTouch(grCollStatus::Touch_Left)) {
        bool found = false;
        grCollisionLineWork* lineWork = &m_touch[grCollStatus::Touch_Left].m_lineWork;
        if (hasLine(lineWork)) {
            grCollisionJoint* touched = lineWork->m_collisionJoint;
            if (touched->m_ground != NULL) {
                ground = touched->m_ground;
                joint = touched;
                found = true;
            }
        }
        if (found) {
            ground->receiveCollMsg(3, m_collStatus, joint);
        }
    }
    if (m_collStatus->isTouch(grCollStatus::Touch_Right)) {
        bool found = false;
        grCollisionLineWork* lineWork = &m_touch[grCollStatus::Touch_Right].m_lineWork;
        if (hasLine(lineWork)) {
            grCollisionJoint* touched = lineWork->m_collisionJoint;
            if (touched->m_ground != NULL) {
                ground = touched->m_ground;
                joint = touched;
                found = true;
            }
        }
        if (found) {
            ground->receiveCollMsg(4, m_collStatus, joint);
        }
    }
}

// Puts the shape on the stage when its status is first tested: the floor line under the bottom point (within 10 units) decides
// between ground and air; a ceiling line close above the floor squeezes the shape (setUD), and two wall lines close on both sides
// squeeze it horizontally (setLR). The lines found are stored as the status' touch lines. Returns 1.
int grCollisionTest::initPos() {
    grCollCollection collection(0x300);
    resetWork();
    clAABBox2D box;
    m_unk2bc->getAABBox(box);
    box.m_unk1 = box.m_unk1 - 10.0f;
    box.m_unk2 = box.m_unk2 - 5.0f;
    box.m_unk3 = box.m_unk3 + 10.0f;
    box.m_unk4 = box.m_unk4 + 5.0f;
    __memcpy(&collection.m_aabb, &box, sizeof(clAABBox2D));
    m_collection = &collection;
    collection.m_unk12 = m_collStatus->m_targetLineCategory;
    collection.m_unk14 = m_collStatus->m_collision;
    collection.m_unk13 = m_collStatus->m_testDynamicCollision;
    fn_8011728C(&collection, m_exclusiveLineAttr, 0);

    grCollisionLineWork* floor = NULL;
    bool onGround = true;
    int result = 1;
    Vec2f floorPos(3.4028235e+38f, 3.4028235e+38f);
    m_unk8 = 1.0f;
    Vec2f down = m_unk2bc->getDownPos();
    for (u32 i = 0; i < m_collection->m_lineWorks.m_num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        if (isActiveLineWork(lineWork) && lineWork->m_collisionLine->m_isUnder
            && (lineWork->m_collisionLine->m_targetCategory & (u8)m_collStatus->m_targetLineCategory) != 0) {
            grCollSegmentEx segment;
            fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
            if (segment.m_segment.m_p0Pos.m_x <= down.m_x && down.m_x <= segment.m_segment.m_p1Pos.m_x) {
                float rate = (down.m_x - segment.m_segment.m_p0Pos.m_x) / (segment.m_segment.m_p1Pos.m_x - segment.m_segment.m_p0Pos.m_x);
                Vec2f point = segment.m_segment.m_p0Pos + (segment.m_segment.m_p1Pos - segment.m_segment.m_p0Pos) * rate;
                float dx = (float)__fabs(down.m_x - point.m_x);
                float dy = (float)__fabs(down.m_y - point.m_y);
                if (dx <= 10.0f && dy <= 10.0f && dy < (float)__fabs(down.m_y - floorPos.m_y)) {
                    floor = lineWork;
                    floorPos = point;
                }
            }
        }
    }
    if (floorPos.m_x == 3.4028235e+38f || floorPos.m_y == 3.4028235e+38f || floor == NULL) {
        onGround = false;
    }
    grCollisionLineWork* ceilingAbove = NULL;
    grCollisionLineWork* ceilingBelow = NULL;
    if (onGround) {
        float floorY = floorPos.m_y;
        float aboveY = 3.4028235e+38f;
        float belowY = -3.4028235e+38f;
        for (u32 i = 0; i < m_collection->m_lineWorks.m_num; i++) {
            grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
            if (isActiveLineWork(lineWork) && lineWork->m_collisionLine->m_isUpper
                && (lineWork->m_collisionLine->m_targetCategory & (u8)m_collStatus->m_targetLineCategory) != 0) {
                grCollSegmentEx segment;
                fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
                if (segment.m_segment.m_p1Pos.m_x <= down.m_x && down.m_x <= segment.m_segment.m_p0Pos.m_x) {
                    float rate = (down.m_x - segment.m_segment.m_p1Pos.m_x) / (segment.m_segment.m_p0Pos.m_x - segment.m_segment.m_p1Pos.m_x);
                    Vec2f point = segment.m_segment.m_p1Pos + (segment.m_segment.m_p0Pos - segment.m_segment.m_p1Pos) * rate;
                    if (point.m_y <= floorY) {
                        if (belowY < point.m_y) {
                            belowY = point.m_y;
                            ceilingBelow = lineWork;
                        }
                    } else if (floorY < point.m_y && point.m_y < aboveY) {
                        aboveY = point.m_y;
                        ceilingAbove = lineWork;
                    }
                }
            }
        }
        m_unk2bc->setDownPos(floorPos);
        if (ceilingAbove != NULL) {
            Vec2f ceilingPos(floorPos.m_x, aboveY);
            float gap = (float)__fabs(aboveY - floorPos.m_y);
            if (gap < m_unk2bc->getHeight()) {
                Vec2f center(floorPos.m_x, (ceilingPos.m_y + floorPos.m_y) * 0.5f);
                m_unk2bc->setUD(center, ceilingPos, floorPos);
            }
            m_collStatus->m_collLineWorkArray[grCollStatus::Touch_Up] = *ceilingAbove;
            m_collStatus->m_touchUp = true;
        } else if (ceilingBelow != NULL) {
            if ((float)__fabs(floorPos.m_y - belowY) < 0.1f) {
                if (ceilingBelow->m_collisionLine->m_isCrush || floor->m_collisionLine->m_isCrush) {
                    Vec2f center(floorPos.m_x, floorPos.m_y + 0.1f);
                    Vec2f up(floorPos.m_x, floorPos.m_y + 0.2f);
                    m_unk2bc->setUD(center, up, floorPos);
                }
                m_collStatus->m_collLineWorkArray[grCollStatus::Touch_Up] = *ceilingBelow;
                m_collStatus->m_touchUp = true;
            }
        }
        m_collStatus->m_currentCollShape->set(*m_unk2bc);
        m_collStatus->setStGround();
        m_collStatus->m_collLineWorkArray[grCollStatus::Touch_Down] = *floor;
        m_collStatus->m_touchDown = true;
        result = 1;
        Vec2f groundPos = m_unk2bc->getDownPos();
        m_collStatus->m_unk1C = groundPos;
    } else {
        m_collStatus->setStAir();
    }

    Vec2f center = m_unk2bc->getCenterPos();
    grCollisionLineWork* leftWall = NULL;
    grCollisionLineWork* rightWall = NULL;
    float rightDx = 10.0f;
    float leftDx = -10.0f;
    for (u32 i = 0; i < m_collection->m_lineWorks.m_num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        grCollisionLine* line = lineWork->m_collisionLine;
        if ((line->m_isLeft || line->m_isRight) && (line->m_targetCategory & (u8)m_collStatus->m_targetLineCategory) != 0) {
            grCollSegmentEx segment;
            fn_80111968(lineWork->m_collision, &segment, lineWork->m_collisionLine, 0);
            if (lineWork->m_collisionLine->m_isLeft) {
                if (segment.m_segment.m_p0Pos.m_y >= center.m_y && center.m_y >= segment.m_segment.m_p1Pos.m_y) {
                    Vec2f projected;
                    if (segment.m_segment.projectionY(center, projected)) {
                        float dx = projected.m_x - center.m_x;
                        if ((float)__fabs(dx) < (float)__fabs(leftDx)) {
                            leftDx = dx;
                            leftWall = lineWork;
                        }
                    }
                }
            } else {
                if (segment.m_segment.m_p0Pos.m_y <= center.m_y && center.m_y <= segment.m_segment.m_p1Pos.m_y) {
                    Vec2f projected;
                    if (segment.m_segment.projectionY(center, projected)) {
                        float dx = projected.m_x - center.m_x;
                        if ((float)__fabs(dx) < (float)__fabs(rightDx)) {
                            rightDx = dx;
                            rightWall = lineWork;
                        }
                    }
                }
            }
        }
    }
    if (leftWall != NULL && rightWall != NULL && rightDx - leftDx < 5.0f) {
        if (leftWall->m_collisionLine->m_isCrush || rightWall->m_collisionLine->m_isCrush) {
            float shift = (rightDx + leftDx) * 0.5f;
            Vec2f newCenter(center.m_x + shift, center.m_y);
            Vec2f left(newCenter.m_x - 0.1f, center.m_y);
            Vec2f right(newCenter.m_x + 0.1f, center.m_y);
            m_unk2bc->setLR(newCenter, left, right);
            m_collStatus->m_collLineWorkArray[grCollStatus::Touch_Left] = *leftWall;
            m_collStatus->m_collLineWorkArray[grCollStatus::Touch_Right] = *rightWall;
            m_collStatus->m_touchLeft = true;
            m_collStatus->m_touchRight = true;
        }
    }
    return result;
}

// The per-frame collision test of the status: resets the work, then repeats (at most 11 passes, until m_isFinished) the
// cliff / air / ground / cliff-check tests that apply to the status' current setting, and finally publishes the corner touch
// normals and sends the collision messages. Returns 1 when the ground/air state changed (HYPOTHESIS).
int grCollisionTest::test() {
    u32 startTime = OSGetTime();
    resetWork();
    grCollisionLineWork prevGround;
    int mode = m_collStatus->m_setting;
    if (mode == 1) {
        prevGround = *fn_80134524(m_collStatus, grCollStatus::Touch_Down);
    }
    grCollCollection collection(0x300);
    m_collection = &collection;
    m_unk11 = 0;
    while (!m_isFinished) {
        if (m_collStatus->m_setting == 3) {
            m_collStatus->m_touchBits = 0;
            m_touchMask = 0;
            m_unk2CD = 3;
            testCliff();
        }
        if (m_collStatus->m_unk6C == 1) {
            testAttached();
            m_unk2CD = 2;
            testAir();
        } else if (m_collStatus->m_setting == 2) {
            m_collStatus->m_touchBits = 0;
            m_touchMask = 0;
            m_unk2CD = 2;
            testAir();
        }
        if (m_collStatus->m_setting == 1) {
            m_unk2CD = 1;
            testGround();
        } else if (m_collStatus->m_cliffCheckOn) {
            testCliffCheck();
        }
        m_unk11++;
        if (m_unk11 > 10) {
            break;
        }
    }
    grCollStatus* status;
    int stateChanged;
    if (mode == 2) {
        status = m_collStatus;
        stateChanged = status->m_touchDown;
    } else {
        status = m_collStatus;
        stateChanged = !status->m_touchDown;
    }
    if (status->m_ledgeSomething) {
        if (mode == 1) {
            if (status->m_setting == 1 && isActiveLineWork(&status->m_collLineWorkArray[3])) {
                // the shape keeps standing on the ground line it had
            } else {
                status->m_touchDown = true;
                m_collStatus->m_collLineWorkArray[3] = prevGround;
            }
        }
        m_collStatus->m_setting = (grCollStatus::Setting)mode;
    } else {
        status->m_isPassableGround = false;
        status = m_collStatus;
        if (status->m_setting == 1 && status->m_touchDown && status->m_collLineWorkArray[3].m_collisionLine->m_isThroughable) {
            status->m_isPassableGround = true;
        }
    }
    if (m_collStatus->m_touchBits != 0) {
        if (m_collStatus->m_touchUpLeft) {
            Vec2f left = m_unk2bc->getLeftPos();
            Vec2f top = m_unk2bc->getTopPos();
            Vec2f edge = left - top;
            m_collStatus->m_180[0] = Vec2f(-edge.m_y, edge.m_x);
        }
        if (m_collStatus->m_touchUpRight) {
            Vec2f top = m_unk2bc->getTopPos();
            Vec2f right = m_unk2bc->getRightPos();
            Vec2f edge = top - right;
            m_collStatus->m_180[1] = Vec2f(-edge.m_y, edge.m_x);
        }
        if (m_collStatus->m_touchDownLeft) {
            Vec2f down = m_unk2bc->getDownPos();
            Vec2f left = m_unk2bc->getLeftPos();
            Vec2f edge = down - left;
            m_collStatus->m_180[2] = Vec2f(-edge.m_y, edge.m_x);
        }
        if (m_collStatus->m_touchDownRight) {
            Vec2f right = m_unk2bc->getRightPos();
            Vec2f down = m_unk2bc->getDownPos();
            Vec2f edge = right - down;
            m_collStatus->m_180[3] = Vec2f(-edge.m_y, edge.m_x);
        }
    }
    if (m_collStatus->m_isEventToGround) {
        callbackExec(mode, &prevGround);
    }
    lbl_805A0368 += (u32)OSGetTime() - startTime;
    return stateChanged;
}

// HYPOTHESIS: the four-argument getSegment writes five words (grCollSegmentEx); cluster B only wants the segment.
static inline void getSegmentCompat(grCollision* collision, clSegment2D* out, grCollisionLine* line) {
    grCollSegmentEx ex;
    fn_80111968(collision, &ex, line, 0);
    *out = ex.m_segment;
}

// MATCH-ONLY: the original inlines this into the ground, wall and ceiling tests; it has to be defined before its first use.
// Records the hit of the shape against lineWork on side touchId when it is nearer than the one already stored for that side.
void grCollisionTest::updateTouchLine(int touchId, grCollisionLineWork* lineWork, float rate) {
    grCollTouchRecord* record = &m_touch[touchId];
    if (record->m_rate >= rate) {
        record->m_id = touchId;
        record->m_rate = rate;
        record->m_lineWork = *lineWork;
        m_touchMask |= 1 << touchId;
        if (m_hit.m_rate >= rate) {
            m_hit = *record;
        }
        if (touchId > 3) {
            m_unkC_3 = true;
        }
    }
}

// ===== cluster B =====

// HYPOTHESIS: grCollisionLineWork::getPrevNoVLink / getNextNoVLink (like getPrev / getNext, but they do not follow the
// virtual link to another collision).
// HYPOTHESIS: grCollision::getSegment, the variant with an extra int argument (gr_collision.o 0x80111968; the shape
// tests pass 0), used where the circle unit also calls it.
// HYPOTHESIS: grCollisionLineWork::getPosYLeftLine / getPosYRightLine (the line further along the chain at the same
// height, used to find where a wall continues); (self, out) -> found.
// HYPOTHESIS: grCollStatus::reconstructIgnoreLine (re-collects the status' ignore lines inside the box).
extern "C" void fn_801373E8(grCollStatus* status, const clAABBox2D* box);
// HYPOTHESIS: grCollStatus::setStGroundInternal (goes to the ground setting without resetting the touch lines).
extern "C" void fn_801340A0(grCollStatus* status);
// HYPOTHESIS: grCollisionLineWork::getPosXDownLine(float x, out): the line below/next to this one at height x.
// HYPOTHESIS: grCollisionLineWork::isLinkPrev / isLinkNext (target is linked to this line on the left / right).
// HYPOTHESIS: grCollStatus::getTouchLine (the line work of a touch id).
// HYPOTHESIS: grCollStatus::clearTouchLine (empties the touch line work of the touch id).
// HYPOTHESIS: grCollStatus::isIgnoreLine (the line work is on the status' ignore list).
extern "C" int fn_80137114(grCollStatus* status, grCollisionLineWork* lineWork);

// HYPOTHESIS: the "has line and joint" then "and collision" check with one flag per stage (checkCloud, testAttached).
static inline bool isValidLineWork(const grCollisionLineWork& lw) {
    bool stage2 = false;
    bool stage1 = false;
    stage1 = lw.m_collisionLine != NULL && lw.m_collisionJoint != NULL;
    if (stage1 && lw.m_collision != NULL) {
        stage2 = true;
    }
    return stage2;
}

// HYPOTHESIS: "all three pointers set" check on a line work (as written by the plain `valid` flag of
// grCollisionLineWork::getPrevVirtual).
static inline bool isValid3(const grCollisionLineWork& lw) {
    bool valid = false;
    if (lw.m_collisionLine != NULL && lw.m_collisionJoint != NULL && lw.m_collision != NULL) {
        valid = true;
    }
    return valid;
}

// HYPOTHESIS: the line work is valid and its joint and collision are enabled (same pattern as the `enabled` flag of
// grCollisionLineWork::getPrevVirtual).
static inline bool isEnabledLineWork(const grCollisionLineWork& lw) {
    bool valid = lw.m_collisionLine != NULL && lw.m_collisionJoint != NULL && lw.m_collision != NULL;
    bool enabled;
    if (valid && lw.m_collisionJoint->m_0x54_4 && lw.m_collision->m_isEnabled) {
        enabled = true;
    } else {
        enabled = false;
    }
    return enabled;
}

// Returns nonzero when the line must not be collided with: not of a category the status targets, on the status'
// ignore list, or (for a ceiling-type line, not through-able) when it is not a dead end between wall-type neighbours.
int grCollisionTest::isIgnoreLine(grCollisionLineWork* lineWork) {
    int result = 0;
    grCollisionLine* line;
    do {
        line = lineWork->m_collisionLine;
        if (line->m_isUnder && !line->m_isThroughable) {
            grCollisionLineWork neighbor;
            if (lineWork->getPrevNoVLink(&neighbor) && neighbor.m_collisionLine->m_isRight) {
                break;
            }
            if (lineWork->getNextNoVLink(&neighbor) && neighbor.m_collisionLine->m_isLeft) {
                break;
            }
            if (lineWork->getPrev(&neighbor) && neighbor.m_collisionLine->m_isRight) {
                break;
            }
            if (lineWork->getNext(&neighbor) && neighbor.m_collisionLine->m_isLeft) {
                break;
            }
            line = lineWork->m_collisionLine;
            if (line->m_unkA >= 0 && line->m_unk4 >= 0) {
                break;
            }
            if (line->m_unkC >= 0 && line->m_unk6 >= 0) {
                break;
            }
        }
        result = 1;
        if (line->m_targetCategory & (u8)m_collStatus->m_targetLineCategory) {
            result = fn_80137114(m_collStatus, lineWork);
            if (result == 0) {
                grCollisionJoint* joint = lineWork->m_collisionJoint;
                for (u32 i = 0; i < m_ignoreJointNum; i++) {
                    if (joint == m_ignoreJoints[i]) {
                        result = 1;
                        break;
                    }
                }
            }
        }
    } while (0);
    return result;
}

// HYPOTHESIS: the status' break-object event mask (grCollStatus + 0x6F), as read through the u8 pointer parameter.
struct grCollBreakObjectFlags {
    bool m_up : 1;    // bit 7, matched against touch mask bit 0 (touch id 0)
    bool m_down : 1;  // bit 6, touch mask bit 3 (touch id 3)
    bool m_left : 1;  // bit 5, touch mask bit 1 (touch id 1)
    bool m_right : 1; // bit 4, touch mask bit 2 (touch id 2)
};

// Fires the receiveCollMsg(5) event of the Ground that owns the hit joint when one of the sides flagged in eventFlags
// was touched; the matched flags are stored back into the status (+0x70). Returns 1 when the event was sent.
int grCollisionTest::notifyEventBreakObject(const u8* eventFlags, u32 touchMask) {
    const grCollBreakObjectFlags* flags = (const grCollBreakObjectFlags*)eventFlags;
    int result = 0;
    bool matched = false;
    u8 matchedMask = 0;
    if (flags->m_up && (touchMask & 1)) {
        matched = true;
        matchedMask |= 0x80;
    }
    if (flags->m_down && (touchMask & 8)) {
        matched = true;
        matchedMask |= 0x40;
    }
    if (flags->m_left && (touchMask & 2)) {
        matched = true;
        matchedMask |= 0x20;
    }
    if (flags->m_right && (touchMask & 4)) {
        matched = true;
        matchedMask |= 0x10;
    }
    if (matched) {
        grCollisionJoint* joint = m_hit.m_lineWork.m_collisionJoint;
        if (joint && joint->m_0x55_4) {
            *(u8*)((u8*)m_collStatus + 0x70) = matchedMask; // the status' attack-point-break bits (high nibble)
            Ground* ground = joint->m_ground;
            if (ground) {
                ground->receiveCollMsg(5, m_collStatus, joint);
                result = 1;
            }
        }
    }
    return result;
}

// Copies every enabled touch record's line work into the status' touch lines (testAir, testGround tail).
static inline void copyTouchLinesToStatus(grCollisionTest& test) {
    if (test.m_touchMask != 0) {
        for (int i = 0; i < grCollisionTest::TOUCH_NUM; i++) {
            if (isEnabledLineWork(test.m_touch[i].m_lineWork)) {
                test.m_collStatus->m_collLineWorkArray[i] = test.m_touch[i].m_lineWork;
            }
        }
    }
}

// Clears one touch record the way testAir does: rate 1.0, id 0xFF, empty line (the flag byte of the line work is kept).
static inline void resetTouchRecord(grCollTouchRecord& record) {
    record.m_rate = 1.0f;
    record.m_id = 0xFF;
    record.m_lineWork.m_collisionLine = NULL;
    record.m_lineWork.m_collisionJoint = NULL;
    record.m_lineWork.m_collision = NULL;
    record.m_lineWork.m_unkC = 0;
}

// The air test: sweeps the shape from its current to its target position against the collected lines. The first hit
// (m_hit) decides how it is handled: cloud pass-through, break-object event, the static "test collision stop" path, or
// by touch id (3 ground, 2 right wall, 1 left wall, 0 ceiling) the dedicated wall/ground test. When nothing is hit the shape
// is moved the whole way. Returns the status' "touching down" flag.
int grCollisionTest::testAir() {
    m_unk2D0 = 0;
    grCollCollection* collection = m_collection;
    m_unk2CB = 0;
    for (int i = 0; i < TOUCH_NUM; i++) {
        m_touch[i].m_rate = 1.0f;
    }
    m_hit.m_rate = 1.0f;
    m_hit.m_id = 0xFF;
    m_hit.m_lineWork.m_collisionLine = NULL;
    m_hit.m_lineWork.m_collisionJoint = NULL;
    m_hit.m_lineWork.m_collision = NULL;
    m_hit.m_lineWork.m_unkC = 0;

    // The swept area: the box of both shapes, grown by the shape's extents and at least 10 x 10.
    clAABBox2D box;
    getAABBox(&box);
    Vec2f down;
    Vec2f::copy(down, m_unk2bc->getDownPos());
    Vec2f top;
    Vec2f::copy(top, m_unk2bc->getTopPos());
    float height = top.m_y - down.m_y;
    Vec2f center;
    Vec2f::copy(center, m_unk2bc->getCenterPos());
    float over = (height + center.m_y) - box.m_unk4;
    if (over > 0.0f) {
        box.m_unk4 = box.m_unk4 + over;
    }
    box.m_unk2 = box.m_unk2 - height;
    Vec2f left;
    Vec2f::copy(left, m_unk2bc->getLeftPos());
    Vec2f::copy(center, m_unk2bc->getCenterPos());
    box.m_unk1 = box.m_unk1 - (center.m_x - left.m_x);
    Vec2f::copy(center, m_unk2bc->getCenterPos());
    Vec2f right;
    Vec2f::copy(right, m_unk2bc->getRightPos());
    box.m_unk3 = box.m_unk3 + (right.m_x - center.m_x);
    if (box.m_unk3 - box.m_unk1 < 10.0f) {
        float grow = (10.0f - (box.m_unk3 - box.m_unk1)) * 0.5f;
        box.m_unk3 = box.m_unk3 + grow;
        box.m_unk1 = box.m_unk1 - grow;
    }
    if (box.m_unk4 - box.m_unk2 < 10.0f) {
        float grow = (10.0f - (box.m_unk4 - box.m_unk2)) * 0.5f;
        box.m_unk4 = box.m_unk4 + grow;
        box.m_unk2 = box.m_unk2 - grow;
    }
    Vec2f from;
    Vec2f::copy(from, m_unk2bc->getCenterPos());
    Vec2f to;
    Vec2f::copy(to, m_unk2c4->getCenterPos());
    Vec2f move = to - from;
    float absX = __fabsf(move.m_x);
    float absY = __fabsf(move.m_y);
    if (absX > absY) {
        if (move.m_x > 0.0f) {
            box.m_unk4 = box.m_unk4 + absX;
        } else {
            box.m_unk2 = box.m_unk2 - absX;
        }
    } else {
        box.m_unk3 = box.m_unk3 + absY;
        box.m_unk1 = box.m_unk1 - absY;
    }

    updateCollection(&box);
    fn_801373E8(m_collStatus, &box);
    int hit = 0;
    if (collection->m_lineWorks.m_num == 0) {
        *((u8*)m_collStatus + 0x7D) = 0; // HYPOTHESIS: byte in the status' padding
    } else {
        initialCorrection();
        if (m_unkC_2 || m_unkC_1) {
            m_isFinished = true;
            return m_collStatus->m_touchDown;
        }
        for (int i = 0; i < TOUCH_NUM; i++) {
            m_touch[i].m_rate = 1.0f;
        }
        m_hit.m_lineWork.m_collisionLine = NULL;
        m_hit.m_rate = 1.0f;
        m_hit.m_id = 0xFF;
        m_hit.m_lineWork.m_collisionJoint = NULL;
        m_hit.m_lineWork.m_collision = NULL;
        m_hit.m_lineWork.m_unkC = 0;
        if (m_collection->m_lineWorks.m_num != 0) {
            hit = testCollisionMainAir();
        }
        if (hit) {
            u8 id = m_hit.m_id;
            if (m_collStatus->m_isCloudThrough) {
                if (checkCloud()) {
                    m_collStatus->m_currentCollShape->set(*m_unk2bc);
                    m_collStatus->m_isCloudThroughOut = true;
                    m_exclusiveLineAttr |= 0x800;
                    m_collStatus->setStAir();
                    m_needCollect4 = true;
                    return m_collStatus->m_touchDown;
                }
            }
            u8 breakMask = m_collStatus->m_unk6F;
            if (breakMask != 0) {
                bool attackPoint = m_collStatus->m_attackPointFlag;
                if (notifyEventBreakObject(&breakMask, id) && attackPoint) {
                    if (m_ignoreJointNum < IGNORE_JOINT_MAX) {
                        m_ignoreJoints[m_ignoreJointNum] = m_hit.m_lineWork.m_collisionJoint;
                        m_ignoreJointNum++;
                    }
                    m_collStatus->setStAir();
                    m_touchMask = 0;
                    m_needCollect4 = true;
                    for (int i = 0; i < TOUCH_NUM; i++) {
                        resetTouchRecord(m_touch[i]);
                    }
                    resetTouchRecord(m_hit);
                    return m_collStatus->m_touchDown;
                }
            }
            if (m_collStatus->m_testCollStop) {
                lastCorrection();
                m_collStatus->m_currentCollShape->set(*m_unk2bc);
                m_unk2c4->set(*m_unk2bc);
                m_collStatus->m_currentCollShape->set(*m_unk2bc);
                copyTouchLinesToStatus(*this);
                m_isFinished = true;
                return m_collStatus->m_touchDown;
            }
            if (id == 3) {
                fn_801340A0(m_collStatus);
                if (m_collStatus->m_setting == 1) {
                    Vec2f dist = m_unk2c4->getDownPos() - m_unk2bc->getDownPos();
                    dist.m_y = 0.0f;
                    Vec2f pos = m_unk2bc->getDownPos() + dist;
                    m_unk2c4->setDownPos(pos);
                }
            } else {
                if (id == 2) {
                    int result = testRightWallMain();
                    if (m_collStatus->m_setting == 1) {
                        Vec2f dist = m_unk2c4->getDownPos() - m_unk2bc->getDownPos();
                        dist.m_y = 0.0f;
                        Vec2f pos = m_unk2bc->getDownPos() + dist;
                        m_unk2c4->setDownPos(pos);
                    }
                    return result;
                }
                if (id == 1) {
                    int result = testLeftWallMain();
                    if (m_collStatus->m_setting == 1) {
                        Vec2f dist = m_unk2c4->getDownPos() - m_unk2bc->getDownPos();
                        dist.m_y = 0.0f;
                        Vec2f pos = m_unk2bc->getDownPos() + dist;
                        m_unk2c4->setDownPos(pos);
                    }
                    return result;
                }
                if (id == 0) {
                    return testTopMain();
                }
                hit = 0;
            }
        }
    }
    if (hit == 0) {
        updateCurrent(1.0f);
        if (m_hit.m_id != 0xFF) {
            ajustShape();
        }
        lastCorrection();
        m_collStatus->m_currentCollShape->set(*m_unk2bc);
        copyTouchLinesToStatus(*this);
        m_isFinished = true;
    }
    return m_collStatus->m_touchDown;
}

// Validates the lines the shape is attached to (grCollStatus + 0x6D: bit n = touch id n). A touch is dropped when its
// line work is no longer valid/enabled or the line no longer faces the touch direction; with nothing left the status
// goes to the air, otherwise (calc-line-move status) the shape is re-projected onto the line of the first remaining touch.
void grCollisionTest::testAttached() {
    u8 attached = m_collStatus->m_unk6D;
    u32 i = 0;
    do {
        u32 bit = 1 << (u8)i;
        if (attached & bit) {
            grCollisionLineWork* lineWork = &m_collStatus->m_collLineWorkArray[(u8)i];
            if (!(isValidLineWork(*lineWork) && isEnabledLineWork(*lineWork))) {
                fn_80134468(m_collStatus, (u8)i);
                attached &= ~bit;
            }
        }
        i++;
    } while (i < 4);
    if (attached != 0) {
        if (attached & 1) {
            if (isValid3(m_collStatus->m_collLineWorkArray[0]) && !m_collStatus->m_collLineWorkArray[0].m_collisionLine->m_isUpper) {
                attached &= 0xFE;
            }
        }
        if (attached & 2) {
            if (isValid3(m_collStatus->m_collLineWorkArray[1]) && !m_collStatus->m_collLineWorkArray[1].m_collisionLine->m_isLeft) {
                attached &= ~2;
            }
        }
        if (attached & 4) {
            if (isValid3(m_collStatus->m_collLineWorkArray[2]) && !m_collStatus->m_collLineWorkArray[2].m_collisionLine->m_isRight) {
                attached &= 0xFB;
            }
        }
        if (attached & 8) {
            if (isValid3(m_collStatus->m_collLineWorkArray[3]) && !m_collStatus->m_collLineWorkArray[3].m_collisionLine->m_isUnder) {
                attached &= 0xF7;
            }
        }
    }
    m_collStatus->m_unk6D = attached;
    if (attached == 0) {
        m_collStatus->setStAir();
        m_collStatus->m_unk6C = 0;
        fn_80134468(m_collStatus, 3);
    } else if (m_collStatus->m_isCalcLineMove) {
        for (u32 i = 0; i < 4; i++) {
            u8 id = i;
            if (attached & (1 << id)) {
                grCollisionLineWork* lineWork = &m_collStatus->m_collLineWorkArray[id];
                Vec2f offsetPos;
                Vec2f offset;
                switch (id) {
                case 0: {
                    clSegment2D segment;
                    Vec2f pos;
                    Vec2f out;
                    getSegmentCompat(lineWork->m_collision, &segment, lineWork->m_collisionLine);
                    Vec2f::copy(pos, m_unk2bc->getTopPos());
                    segment.projectionX(pos, out);
                    m_unk2bc->setTopPos(out);
                    calcOffsetVector(lineWork, addressOf(m_unk2bc->getTopPos()), &offsetPos, &offset);
                    m_unk2c0->setTopPos(offsetPos);
                    break;
                }
                case 1: {
                    clSegment2D segment;
                    Vec2f pos;
                    Vec2f out;
                    getSegmentCompat(lineWork->m_collision, &segment, lineWork->m_collisionLine);
                    Vec2f::copy(pos, m_unk2bc->getLeftPos());
                    segment.projectionY(pos, out);
                    m_unk2bc->setLeftPos(out);
                    calcOffsetVector(lineWork, addressOf(m_unk2bc->getLeftPos()), &offsetPos, &offset);
                    m_unk2c0->setLeftPos(offsetPos);
                    break;
                }
                case 2: {
                    clSegment2D segment;
                    Vec2f pos;
                    Vec2f out;
                    getSegmentCompat(lineWork->m_collision, &segment, lineWork->m_collisionLine);
                    Vec2f::copy(pos, m_unk2bc->getRightPos());
                    segment.projectionY(pos, out);
                    m_unk2bc->setRightPos(out);
                    calcOffsetVector(lineWork, addressOf(m_unk2bc->getRightPos()), &offsetPos, &offset);
                    m_unk2c0->setRightPos(offsetPos);
                    break;
                }
                case 3: {
                    clSegment2D segment;
                    Vec2f pos;
                    Vec2f out;
                    getSegmentCompat(lineWork->m_collision, &segment, lineWork->m_collisionLine);
                    Vec2f::copy(pos, m_unk2bc->getDownPos());
                    segment.projectionX(pos, out);
                    m_unk2bc->setDownPos(out);
                    calcOffsetVector(lineWork, addressOf(m_unk2bc->getDownPos()), &offsetPos, &offset);
                    m_unk2c0->setDownPos(offsetPos);
                    break;
                }
                }
                m_collStatus->m_currentCollShape->set(*m_unk2c0);
                break;
            }
        }
    }
}

// Cloud (pass-through floor) check for testAir: when the nearest hit is on a cloud line, records the shape corner
// that touched it (top/left/right/down by the hit's touch id) in grCollStatus::m_unk and returns 1.
int grCollisionTest::checkCloud() {
    int result = 0;
    bool valid = isValidLineWork(m_hit.m_lineWork);
    grCollStatus* status = m_collStatus;
    if (valid && m_hit.m_lineWork.m_collisionLine->m_isCloud) {
        switch (m_hit.m_id) {
        case 0:
            status->m_unk = m_unk2bc->getTopPos();
            break;
        case 1:
            status->m_unk = m_unk2bc->getLeftPos();
            break;
        case 2:
            status->m_unk = m_unk2bc->getRightPos();
            break;
        case 3:
        default:
            status->m_unk = m_unk2bc->getDownPos();
            break;
        }
        result = 1;
    }
    return result;
}

// The ground test: moves the shape along the ground line it stands on (m_unk224 / the down touch) towards its target,
// following the line chain. At the end of a line it goes on to the next one (getNextGroundLine), stops at the edge
// (stopEdgeCorrect), pushes out of an end wall (testGroundLRWallCorrect) or leaves the ground (setStAir, with the
// "miss step" flags). Other lines the swept shape hits are handled by testCollisionMainGround. Afterwards the shape is
// re-projected onto the final line and the touch lines are copied to the status. HYPOTHESIS: returns "not touching down".
int grCollisionTest::testGround() {
    m_unk2D0 = 4;
    m_unk2CB = 0;
    bool useAjust = true;
    grCollisionLineWork ground;
    grCollisionLineWork next;
    m_unk8 = 1.0f;
    grCollCollection* collection = m_collection;
    for (int i = 0; i < TOUCH_NUM; i++) {
        m_touch[i].m_rate = 1.0f;
    }
    m_hit.m_rate = 1.0f;
    m_hit.m_id = 0xFF;
    m_hit.m_lineWork.m_collisionLine = NULL;
    m_hit.m_lineWork.m_collisionJoint = NULL;
    m_hit.m_lineWork.m_collision = NULL;
    m_hit.m_lineWork.m_unkC = 0;
    bool ignoreSlope = m_collStatus->m_isCorrectIgnoreSlope;
    m_collStatus->m_unkAC = m_unk2bc->getDownPos();
    m_unk2CD = 1;
    if (m_collStatus->m_touchDown) {
        // Stand on the recorded ground line, unless it is gone or too short.
        ground = *fn_80134524(m_collStatus, 3);
        if (!(isEnabledLineWork(ground) && ground.m_collisionLine->m_isUnder)) {
            m_collStatus->setStAir();
            fn_80134468(m_collStatus, 3);
            return 0;
        }
        clSegment2D segment;
        getSegmentCompat(ground.m_collision, &segment, ground.m_collisionLine);
        Vec2f dir = segment.m_p1Pos - segment.m_p0Pos;
        if (dir.length() < 0.01f) {
            m_collStatus->setStAir();
            return 0;
        }
        grCollisionLineWork& attachedLine = m_collStatus->m_collLineWorkArray[9];
        if (isValid3(attachedLine)) {
            if (attachedLine.m_collisionLine == ground.m_collisionLine) {
                m_collStatus->setStAir();
                return 0;
            }
            if (*(u16*)((u8*)ground.m_collisionLine + 0x10) & m_collStatus->m_exclusiveLineAttr) { // HYPOTHESIS: the halfword at line + 0x10 holds the attribute bits
                m_collStatus->setStAir();
                return 0;
            }
        }
        if (m_collStatus->m_isCalcLineMove) {
            Vec2f offsetPos;
            Vec2f offset;
            calcOffsetVector(&ground, addressOf(m_unk2bc->getDownPos()), &offsetPos, &offset);
            Vec2f::copy(offsetPos, m_unk2bc->getDownPos());
            grCollisionLineWork lower;
            if (ground.getPosXDownLine(offsetPos.m_x, &lower)) {
                clSegment2D lowerSegment;
                Vec2f projected;
                lower.m_collision->getSegment(&lowerSegment, lower.m_collisionLine);
                if (lowerSegment.projectionX(offsetPos, projected)) {
                    ground = lower;
                }
            }
            offsetPos = m_unk2c4->getDownPos() + offset;
            m_unk2c4->setDownPos(offsetPos);
            m_unk2c0->set(*m_unk2c4);
            // The move vector and its direction bits are recomputed for the corrected target.
            Vec2f from;
            Vec2f::copy(from, m_unk2bc->getCenterPos());
            Vec2f to = m_unk2c4->getCenterPos();
            m_moveVec = to - from;
            m_unkE = 0;
            if (m_moveVec.m_x > 0.00001f) {
                m_unkE = DIR_RIGHT;
            }
            if (m_moveVec.m_x < -0.00001f) {
                m_unkE |= DIR_LEFT;
            }
            if (m_moveVec.m_y > 0.00001f) {
                m_unkE |= DIR_UP;
            }
            if (m_moveVec.m_y < -0.00001f) {
                m_unkE |= DIR_DOWN;
            }
        } else {
            m_unk2c0->set(*m_unk2c4);
        }
        updateTouchLine(3, &ground, 1.0f);
    }
    if (m_collStatus->m_unk6C == 1 && (m_collStatus->m_unk6D & 8)) {
        // Attached to the ground by the line-move code: keep the shape, only publish the touch lines.
        m_collStatus->m_currentCollShape->set(*m_unk2bc);
        int result = m_collStatus->m_touchDown;
        copyTouchLinesToStatus(*this);
        m_isFinished = true;
        return result;
    }
    m_unkF = 0xFE;
    clAABBox2D box;
    getAABBox(&box);
    if (isEnabledLineWork(m_collStatus->m_collLineWorkArray[3])) {
        clSegment2D downSegment;
        getSegmentCompat(m_collStatus->m_collLineWorkArray[3].m_collision, &downSegment,
                    m_collStatus->m_collLineWorkArray[3].m_collisionLine);
    }
    fn_801373E8(m_collStatus, &box);
    updateCollection(&box);
    m_unk224 = ground;
    // Remaining length to walk on the ground (m_unk10C): |dx| of the move, or measured along the sloped line
    // relative to the speed of the line the shape stands on.
    Vec2f downNow;
    Vec2f::copy(downNow, m_unk2bc->getDownPos());
    Vec2f move = m_unk2c4->getDownPos() - downNow;
    m_unk10C = 0.0f;
    if (!ignoreSlope) {
        clSegment2D lineSegment;
        getSegmentCompat(ground.m_collision, &lineSegment, ground.m_collisionLine);
        Vec2f dir = lineSegment.m_p1Pos - lineSegment.m_p0Pos;
        Vec2f lineSpeed = m_collStatus->m_touchLineSpeed;
        Vec2f relative = move - lineSpeed;
        float ratio = __fabsf(lineSpeed.m_x) / (dir.m_x / dir.length());
        if (lineSpeed.m_x * relative.m_x <= 0.0f) {
            m_unk10C = ratio - __fabsf(relative.m_x);
            if (m_unk10C < 0.0f) {
                m_unk10C = -m_unk10C;
            }
        } else {
            m_unk10C = ratio + __fabsf(relative.m_x);
            if (m_unk10C < 0.0f) {
                m_unk10C = -m_unk10C;
            }
        }
    } else {
        m_unk10C = __fabsf(move.m_x);
    }
    bool run = true;
    if (collection->m_lineWorks.m_num != 0) {
        initialCorrection();
        if (isEnabledLineWork(m_unk31C)) {
            ground = m_unk31C;
            m_unk224 = ground;
        }
        if ((m_unk2C8 & SIDE_RIGHT) && (m_unk2C8 & SIDE_LEFT)) {
            run = false;
            m_isFinished = true;
        }
        if (m_unkC_2 || m_unkC_1) {
            m_isFinished = true;
            return 1;
        }
    }
    {
        clSegment2D segment;
        getSegmentCompat(ground.m_collision, &segment, ground.m_collisionLine);
        Vec2f pos;
        Vec2f::copy(pos, m_unk2bc->getDownPos());
        Vec2f projected;
        segment.projectionX(pos, projected);
        m_unk2bc->setDownPos(projected);
    }
    if (m_collStatus->m_setting == 2) {
        // Already in the air (pushed off by the corrections): stop if the shape would turn around.
        float prevX = m_unk2b8->getDownPos().m_x;
        float curX = m_unk2bc->getDownPos().m_x;
        float targetX = m_unk2c4->getDownPos().m_x;
        useAjust = 0.0f <= (curX - prevX) * (targetX - curX);
        if (!useAjust) {
            run = false;
            m_unk2c4->set(*m_unk2bc);
        }
        if (m_touch[4].m_id != 0xFF && m_touch[5].m_id != 0xFF) {
            run = false;
            m_unk2c4->set(*m_unk2bc);
            useAjust = false;
        }
    }
    if (run) {
        grCollisionLineWork previous;
        bool done;
        do {
            while (true) {
                Vec2f startPos;
                Vec2f::copy(startPos, m_unk2bc->getDownPos());
                Vec2f checkPos;
                Vec2f::copy(checkPos, m_unk2bc->getDownPos());
                int reachesEnd = checkMoveLenXDown(&checkPos, &ground);
                int hasNext = 0;
                if (reachesEnd) {
                    hasNext = getNextGroundLine(&ground, &next);
                }
                calcNewShapeOnGround(&ground, reachesEnd);
                int hit = 0;
                if (m_collection->m_lineWorks.m_num != 0) {
                    hit = testCollisionMainGround();
                }
                grCollisionLineWork current = ground;
                if (hit) {
                    previous = current;
                    break;
                }
                if (!reachesEnd) {
                    // The move ends on this line.
                    m_unk2bc->set(*m_unk2c0);
                    if (!testGroundLRWallCorrect(&ground, 0)) {
                        m_touch[3].m_lineWork = ground;
                        m_isFinished = true;
                        stopEdgeCorrect(&ground);
                    }
                    goto finished;
                }
                if (!hasNext) {
                    // End of the line chain.
                    if (!stopEdgeCorrect(&ground)) {
                        if (!testGroundLRWallCorrect(&ground, 1)) {
                            m_collStatus->setStAir();
                            m_unk2bc->set(*m_unk2c0);
                            m_unk2c0->set(*m_unk2c4);
                            if (!(m_unkE & DIR_RIGHT)) {
                                if (m_collStatus->m_unkB == 1) {
                                    m_collStatus->m_missStepR = true;
                                }
                            } else {
                                if (m_collStatus->m_unkB == 0) {
                                    m_collStatus->m_missStepL = true;
                                }
                            }
                        }
                    } else {
                        updateTouchLine(3, &ground, 1.0f);
                        m_isFinished = true;
                    }
                    goto finished;
                }
                // Go on with the next line of the chain.
                ground = next;
                m_unk224 = next;
                previous = current;
                updateTouchLine(3, &next, 1.0f);
                clSegment2D nextSegment;
                getSegmentCompat(next.m_collision, &nextSegment, next.m_collisionLine);
                Vec2f joinPos;
                if (!(m_unkE & DIR_RIGHT)) {
                    if (nextSegment.m_p1Pos.m_x <= nextSegment.m_p0Pos.m_x) {
                        joinPos = nextSegment.m_p0Pos;
                    } else {
                        joinPos = nextSegment.m_p1Pos;
                    }
                } else if (nextSegment.m_p1Pos.m_x <= nextSegment.m_p0Pos.m_x) {
                    joinPos = nextSegment.m_p1Pos;
                } else {
                    joinPos = nextSegment.m_p0Pos;
                }
                m_unk2bc->setDownPos(joinPos);
                Vec2f walked = m_unk2bc->getDownPos() - startPos;
                m_unk10C = m_unk10C - walked.length();
                if (m_unk10C <= 0.00001f) {
                    m_isFinished = true;
                    goto finished;
                }
            }
            // The swept shape hit something. Only a different ground line (touch id 3) continues the walk.
            if (m_hit.m_id != 3 || ground.m_collisionLine == m_touch[3].m_lineWork.m_collisionLine) {
                updateTouchLine(3, &ground, 1.0f);
                m_isFinished = true;
                goto finished;
            }
            {
                grCollisionLineWork* hitLine = &m_touch[3].m_lineWork;
                grCollisionLineWork linked;
                done = false;
                if (isValid3(previous) && hitLine->m_collisionLine == previous.m_collisionLine) {
                    done = true;
                } else {
                    if (ground.isLinkNext(hitLine)) {
                        if (hitLine->getNext(&linked) && linked.m_collisionLine->m_isRight) {
                            done = true;
                            goto joined;
                        }
                    }
                    if (ground.isLinkPrev(hitLine)) {
                        if (hitLine->getPrev(&linked) && linked.m_collisionLine->m_isLeft) {
                            done = true;
                        }
                    }
                }
            joined:
                ground = *hitLine;
            }
        } while (!done);
        m_isFinished = true;
    }
finished:
    if (m_collStatus->m_setting == 1) {
        ajustShapeUp();
        if (useAjust) {
            ajustShape();
        }
        {
            clSegment2D segment;
            getSegmentCompat(ground.m_collision, &segment, ground.m_collisionLine);
            Vec2f pos;
            Vec2f::copy(pos, m_unk2bc->getDownPos());
            Vec2f projected;
            segment.projectionX(pos, projected);
            m_unk2bc->setDownPos(projected);
        }
        m_unk224 = ground;
        lastCorrection();
        grCollisionLineWork lower;
        grCollisionLineWork* cur = &m_touch[3].m_lineWork;
        Vec2f downPos;
        Vec2f::copy(downPos, m_unk2bc->getDownPos());
        if (!cur->getPosXDownLine(downPos.m_x, &lower)) {
            grCollisionLineWork neighbor;
            clSegment2D curSegment;
            cur->m_collision->getSegment(&curSegment, cur->m_collisionLine);
            Vec2f p;
            Vec2f::copy(p, m_unk2bc->getDownPos());
            bool keepGround = false;
            if (p.m_x <= curSegment.m_p0Pos.m_x && __fabsf(p.m_x - curSegment.m_p0Pos.m_x) < 1.0f) {
                if (cur->getPrev(&neighbor) && neighbor.m_collisionLine->m_isLeft) {
                    keepGround = true;
                }
            }
            if (!keepGround && curSegment.m_p1Pos.m_x <= p.m_x && __fabsf(p.m_x - curSegment.m_p1Pos.m_x) < 1.0f) {
                if (cur->getNext(&neighbor) && neighbor.m_collisionLine->m_isRight) {
                    keepGround = true;
                }
            }
            if (!keepGround) {
                m_collStatus->setStAir();
            }
        } else if (isValidLineWork(lower) && cur->m_collisionLine != lower.m_collisionLine) {
            clSegment2D lowerSegment;
            lower.m_collision->getSegment(&lowerSegment, lower.m_collisionLine);
            Vec2f pos;
            Vec2f::copy(pos, m_unk2bc->getDownPos());
            Vec2f projected;
            lowerSegment.projectionX(pos, projected);
            m_unk2bc->setDownPos(projected);
            *cur = lower;
        }
        if (m_collStatus->m_setting == 1) {
            clSegment2D curSegment;
            cur->m_collision->getSegment(&curSegment, cur->m_collisionLine);
            Vec2f pos;
            Vec2f::copy(pos, m_unk2bc->getDownPos());
            Vec2f projected;
            curSegment.projectionX(pos, projected);
            m_unk2bc->modifyDownY(projected.m_y + 0.01f);
        }
    }
    m_unkF |= 6;
    m_collStatus->m_currentCollShape->set(*m_unk2bc);
    if (m_collStatus->m_setting == 2) {
        *((u8*)m_collStatus + 0x75) = 0; // HYPOTHESIS: clears all eight touch flags
    } else {
        copyTouchLinesToStatus(*this);
    }
    return !m_collStatus->m_touchDown;
}

// While running along a ground line into a wall at its end: when the shape's leading (right or left) corner has passed
// the end of the ground line and the next line is a wall of the matching side, pushes the corner back out of the wall
// (onto the wall's line or by the overshoot) and records the wall as the touched line; with nothing to correct and
// `snapToEdge` set it moves the shape's bottom point 0.01 inside the ground line's end instead. Returns 1 when the
// shape was changed (the ground line is then recorded as the down touch and the pass is finished).
int grCollisionTest::testGroundLRWallCorrect(grCollisionLineWork* lineWork, int snapToEdge) {
    int result = 0;
    grCollisionLineWork other;
    grCollisionLineWork neighbor;
    clSegment2D segment;
    lineWork->m_collision->getSegment(&segment, lineWork->m_collisionLine);
    if (m_unkE & DIR_RIGHT) {
        Vec2f right;
        Vec2f::copy(right, m_unk2bc->getRightPos());
        if (!(right.m_x < segment.m_p1Pos.m_x)) {
            lineWork->getNext(&neighbor);
            if (isValidLineWork(neighbor) && neighbor.m_collisionLine->m_isRight) {
                neighbor.m_collision->getSegment(&segment, neighbor.m_collisionLine);
                if (segment.m_p0Pos.m_y > right.m_y) {
                    if (segment.m_p0Pos.m_x <= right.m_x) {
                        Vec2f offset(segment.m_p0Pos.m_x - right.m_x, 0.0f);
                        Vec2f pos = right + offset;
                        m_unk2bc->setRightPos(pos);
                        result = 1;
                    }
                } else if (neighbor.getPosYRightLine(right.m_y, &other)) {
                    other.m_collision->getSegment(&segment, other.m_collisionLine);
                    Vec2f proj;
                    if (segment.projectionY(right, proj)) {
                        Vec2f diff = (proj - right) + Vec2f(-0.01f, 0.0f);
                        if (diff.m_x < 0.0f) {
                            proj.m_x = proj.m_x - 0.01f;
                            m_unk2bc->setRightPos(proj);
                            result = 1;
                        }
                    }
                }
                if (result) {
                    m_touch[2].m_lineWork = neighbor;
                    m_collStatus->m_touchRight = true;
                } else if (snapToEdge) {
                    lineWork->m_collision->getSegment(&segment, lineWork->m_collisionLine);
                    Vec2f pos(segment.m_p1Pos.m_x - 0.01f, segment.m_p1Pos.m_y);
                    m_unk2bc->setDownPos(pos);
                    result = 1;
                }
            }
        }
    } else if (m_unkE & DIR_LEFT) {
        Vec2f left;
        Vec2f::copy(left, m_unk2bc->getLeftPos());
        if (!(segment.m_p0Pos.m_x < left.m_x)) {
            lineWork->getPrev(&neighbor);
            if (isValidLineWork(neighbor) && neighbor.m_collisionLine->m_isLeft) {
                neighbor.m_collision->getSegment(&segment, neighbor.m_collisionLine);
                if (segment.m_p1Pos.m_y > left.m_y) {
                    if (left.m_x <= segment.m_p1Pos.m_x) {
                        Vec2f offset(segment.m_p1Pos.m_x - left.m_x, 0.0f);
                        Vec2f pos = left + offset;
                        m_unk2bc->setLeftPos(pos);
                        result = 1;
                    }
                } else if (neighbor.getPosYLeftLine(left.m_y, &other)) {
                    other.m_collision->getSegment(&segment, other.m_collisionLine);
                    Vec2f proj;
                    if (segment.projectionY(left, proj)) {
                        Vec2f diff = (proj - left) + Vec2f(0.01f, 0.0f);
                        if (diff.m_x > 0.0f) {
                            proj.m_x = proj.m_x + 0.01f;
                            m_unk2bc->setLeftPos(proj);
                            result = 1;
                        }
                    }
                }
                if (result) {
                    m_touch[1].m_lineWork = neighbor;
                    m_collStatus->m_touchLeft = true;
                } else if (snapToEdge) {
                    lineWork->m_collision->getSegment(&segment, lineWork->m_collisionLine);
                    Vec2f pos(segment.m_p0Pos.m_x + 0.01f, segment.m_p0Pos.m_y);
                    m_unk2bc->setDownPos(pos);
                    result = 1;
                }
            }
        }
    }
    if (result) {
        m_touch[3].m_lineWork = *lineWork;
        m_isFinished = true;
    }
    return result;
}

// With the status' "stop at the edge" flag set, keeps the shape's bottom point on the ground line: moves the point
// just inside the nearer end of the line (0.01 margin) unless the move is heading back towards the line.
// Returns 1 when the shape was corrected.
int grCollisionTest::stopEdgeCorrect(grCollisionLineWork* lineWork) {
    grCollStatus* status = m_collStatus;
    int result = 0;
    if (status->m_cliffStop) {
        do {
            if (m_unk20 == 1 && status->m_cliffSide == 1 && (m_unkE & DIR_RIGHT)) {
                break;
            }
            if (m_unk20 == 0 && status->m_cliffSide == 0 && (m_unkE & DIR_LEFT)) {
                break;
            }
            clSegment2D segment;
            lineWork->m_collision->getSegment(&segment, lineWork->m_collisionLine);
            Vec2f pos;
            Vec2f::copy(pos, m_unk2bc->getDownPos());
            if (pos.m_x <= segment.m_p0Pos.m_x) {
                pos.m_x = segment.m_p0Pos.m_x + 0.01f;
            } else if (segment.m_p1Pos.m_x <= pos.m_x) {
                pos.m_x = segment.m_p1Pos.m_x - 0.01f;
            }
            m_unk2bc->setDownPos(pos);
            result = 1;
        } while (0);
    }
    return result;
}

// ===== cluster C =====

extern grCollisionManager* lbl_805A0360; // the collision manager singleton


// HYPOTHESIS: grCollision::getPreSegment (0x801119F8): the segment of a line one frame ago.
extern "C" void fn_801119F8(grCollision* collision, clSegment2D* out, grCollisionLine* line);
// HYPOTHESIS: grCollisionLineWork::getUpRightLine / getUpLeftLine (0x80116B1C / 0x80116CD8): the line above that continues to the right / left.

// HYPOTHESIS: grCollStatus::getCliffCheckBoxPre / getCliffCheckBoxCur (0x80135BA0 / 0x80135B94): the box the cliff check covers before / now.
extern "C" void fn_80135BA0(grCollStatus* status, clAABBox2D* out);
extern "C" void fn_80135B94(grCollStatus* status, clAABBox2D* out);

// HYPOTHESIS: clRay2D::checkIntersect1 (0x800403C4) and clSegment2D::projection1 (0x800407A0): the ray (start, direction)
// against a segment; the foot of a point on a segment.
struct grCollRay2D {
    Vec2f m_pos;
    Vec2f m_vec;
};

// Touch ids (grCollStatus::TouchId) of the left and right sides: Left, UpLeft, DownLeft / Right, UpRight, DownRight.
static const int sCliffLeftTouch[3] = { 1, 4, 6 };
static const int sCliffRightTouch[3] = { 2, 5, 7 };


// Bytes 16 and 17 of a line (the m_noAttach.. flags and the material) as one halfword; the wall tests mask it with
// grCollStatus::m_exclusiveLineAttr to skip lines of excluded kinds.
static inline u16 getLineAttr(grCollisionLine* line) {
    return *(u16*)((u8*)line + 0x10);
}




// The corner of the target shape is beyond the end of the current wall line (in the direction of the move).
static inline bool isBeyondLine(u8 dir, const Vec2f& target, float minY, float maxY) {
    bool beyond = false;
    if ((dir & grCollisionTest::DIR_UP) && target.m_y > maxY) {
        beyond = true;
    } else if ((dir & grCollisionTest::DIR_DOWN) && target.m_y < minY) {
        beyond = true;
    }
    return beyond;
}

// As isBeyondLine for the ceiling: the x range of the line against the target corner, in the direction of the move.
static inline bool isBeyondLineX(u8 dir, const Vec2f& target, float minX, float maxX) {
    bool beyond = false;
    if ((dir & grCollisionTest::DIR_RIGHT) && target.m_x > maxX) {
        beyond = true;
    } else if ((dir & grCollisionTest::DIR_LEFT) && target.m_x < minX) {
        beyond = true;
    }
    return beyond;
}

static inline bool isSameLine(grCollisionLine* a, grCollisionLine* b) {
    bool same = false;
    if (a == b) {
        same = true;
    }
    return same;
}

// True when all three pointers of the line work are set.
static inline bool isLinkedLineWork(grCollisionLineWork* lineWork) {
    bool linked = false;
    if (lineWork->m_collisionLine != NULL && lineWork->m_collisionJoint != NULL && lineWork->m_collision != NULL) {
        linked = true;
    }
    return linked;
}


// Starts a wall/ceiling test: the whole move is still to do and no touch has been recorded.
static inline void resetTouchRates(grCollisionTest* test) {
    test->m_unk8 = 1.0f;
    for (int i = 0; i < grCollisionTest::TOUCH_NUM; i++) {
        test->m_touch[i].m_rate = 1.0f;
    }
    clearTouchRecord(&test->m_hit);
}

static inline void clearTouchRecordsAndMask(grCollisionTest* test) {
    test->m_touchMask = 0;
    for (int i = 0; i < grCollisionTest::TOUCH_NUM; i++) {
        clearTouchRecord(&test->m_touch[i]);
    }
    clearTouchRecord(&test->m_hit);
}

// Publishes the recorded touch lines to the status.
static inline void copyTouchLinesToStatus(grCollisionTest* test) {
    for (int i = 0; i < grCollisionTest::TOUCH_NUM; i++) {
        grCollTouchRecord* record = &test->m_touch[i];
        if (isActiveLineWork(&record->m_lineWork)) {
            test->m_collStatus->m_collLineWorkArray[i] = record->m_lineWork;
        }
    }
}


// Right-wall test of the moving shape: slides the right corner along the wall line touched last frame (m_touch[2]),
// stepping to the connected wall line while the corner leaves the current one, until the swept area hits something.
// When the wall ends the shape goes to the air (or follows the neighbouring ceiling/floor line). Returns the "touching" bit.
int grCollisionTest::testRightWallMain() {
    bool flag = false;
    m_unk2D0 = 3; // HYPOTHESIS: which test is running (3 = right wall)
    grCollSegmentEx seg;
    grCollSegmentEx nextSeg;
    grCollisionLineWork wall;
    grCollisionLineWork next;
    resetTouchRates(this);
    m_unkF = 0xF9;
    m_collStatus->m_unkA4 = m_unk2bc->getRightPos();
    wall = m_touch[2].m_lineWork;
    if (isActiveLineWork(&wall)) {
        flag = true;
    } else {
        flag = false;
    }
    if (!flag) {
        m_collStatus->setStAir();
        return 0;
    }
    if (m_collStatus->m_isCalcLineMove) {
        Vec2f newPos;
        Vec2f offset;
        calcOffsetVector(&wall, addressOf(m_unk2bc->getRightPos()), &newPos, &offset);
        m_unk2bc->setRightPos(newPos);
    }
    if (m_collStatus->m_unk6C == 1 && (m_collStatus->m_unk6D & 4)) {
        // the wall is attached to a moving line: the shape was already carried along
        m_collStatus->m_currentCollShape->set(*m_unk2bc);
        int touching = m_collStatus->m_touchDown;
        if (m_touchMask != 0) {
            copyTouchLinesToStatus(this);
        }
        m_isFinished = true;
        return touching;
    }
    float rate = 1.0f;
    int touchId = 2;
    while (true) {
        const Vec2f& target = m_unk2c4->getRightPos();
        fn_80111968(wall.m_collision, &seg, wall.m_collisionLine, 0);
        float y0 = seg.m_segment.m_p0Pos.m_y;
        float y1 = seg.m_segment.m_p1Pos.m_y;
        float maxY = psMax(y0, y1);
        float minY = psMin(y0, y1);
        bool leaves = isBeyondLine(m_unkE, target, minY, maxY);
        int found = 0;
        if (leaves) {
            found = getNextRightLine(&wall, &next);
        }
        calcNewShapeOnRight(&wall);
        int hit;
        if (m_collection->m_lineWorks.m_num == 0) {
            hit = 0;
        } else {
            hit = testCollisionMain();
        }
        if (hit) {
            m_isFinished = true;
            goto adjust;
        }
        if (!leaves) {
            m_isFinished = true;
            m_unk2bc->set(*m_unk2c0);
            goto adjust;
        }
        if (!found) {
            break;
        }
        wall = next;
        updateTouchLine(touchId, &wall, rate);
        fn_80111968(wall.m_collision, &nextSeg, wall.m_collisionLine, 0);
        Vec2f pos;
        if (m_unkE & DIR_UP) {
            if (nextSeg.m_segment.m_p0Pos.m_y < nextSeg.m_segment.m_p1Pos.m_y) {
                pos = nextSeg.m_segment.m_p0Pos;
            } else {
                pos = nextSeg.m_segment.m_p1Pos;
            }
        } else {
            if (nextSeg.m_segment.m_p0Pos.m_y < nextSeg.m_segment.m_p1Pos.m_y) {
                pos = nextSeg.m_segment.m_p1Pos;
            } else {
                pos = nextSeg.m_segment.m_p0Pos;
            }
        }
        m_unk2bc->setRightPos(Vec2f(pos));
    }
    {
        // no further wall line: the shape leaves the wall
        m_collStatus->setStAir();
        fn_80134468(m_collStatus, 2);
        grCollisionLineWork neighbor;
        clSegment2D wallSeg;
        wall.m_collision->getSegment(&wallSeg, wall.m_collisionLine);
        if (m_unkE & DIR_UP) {
            if (wall.getNext(&neighbor) && neighbor.m_collisionLine->m_isUpper) {
                clSegment2D neighborSeg;
                neighbor.m_collision->getSegment(&neighborSeg, neighbor.m_collisionLine);
                Vec2f top = m_unk2bc->getTopPos();
                Vec2f onLine;
                if (neighborSeg.projectionX(top, onLine) && onLine.m_y < top.m_y) {
                    m_unk2bc->setTopPos(onLine);
                }
                m_isFinished = true;
            } else {
                Vec2f pos(wallSeg.m_p1Pos.m_x, wallSeg.m_p1Pos.m_y + 0.01f);
                m_unk2bc->setRightPos(pos);
                clearTouchRecordsAndMask(this);
            }
        } else {
            if (wall.getPrev(&neighbor) && neighbor.m_collisionLine->m_isUnder && !(getLineAttr(neighbor.m_collisionLine) & m_exclusiveLineAttr)) {
                clSegment2D neighborSeg;
                neighbor.m_collision->getSegment(&neighborSeg, neighbor.m_collisionLine);
                Vec2f down = m_unk2bc->getDownPos();
                Vec2f onLine;
                if (neighborSeg.projectionX(down, onLine) && onLine.m_y > down.m_y) {
                    m_unk2bc->setDownPos(onLine);
                }
                m_isFinished = true;
            } else {
                Vec2f pos(wallSeg.m_p0Pos.m_x, wallSeg.m_p0Pos.m_y - 0.01f);
                m_unk2bc->setRightPos(pos);
                clearTouchRecordsAndMask(this);
            }
        }
    }
adjust:
    if (!m_isFinished) {
        m_unk2c0->set(*m_unk2c4);
    }
    ajustShapeLeft();
    if (m_isFinished) {
        ajustShape();
    }
    lastCorrection();
    m_unkF |= 6;
    m_collStatus->m_currentCollShape->set(*m_unk2bc);
    int touching = m_collStatus->m_touchDown;
    if (m_touchMask != 0) {
        copyTouchLinesToStatus(this);
    }
    return touching;
}

// Left-wall test of the moving shape: slides the left corner along the wall line touched last frame (m_touch[1]),
// stepping to the connected wall line while the corner leaves the current one, until the swept area hits something.
// When the wall ends the shape goes to the air (or follows the neighbouring ceiling/floor line). Returns the "touching" bit.
int grCollisionTest::testLeftWallMain() {
    bool flag = false;
    m_unk2D0 = 2; // HYPOTHESIS: which test is running (2 = left wall)
    grCollSegmentEx seg;
    grCollSegmentEx nextSeg;
    grCollisionLineWork wall;
    grCollisionLineWork next;
    resetTouchRates(this);
    clAABBox2D box;
    getAABBox(&box); // HYPOTHESIS: the original computes the swept box here but does not use it
    m_collStatus->m_unk9C = m_unk2bc->getLeftPos();
    m_unkF = 0xF9;
    wall = m_touch[1].m_lineWork;
    if (isActiveLineWork(&wall)) {
        flag = true;
    } else {
        flag = false;
    }
    if (!flag) {
        m_collStatus->setStAir();
        return 0;
    }
    if (m_collStatus->m_isCalcLineMove) {
        Vec2f newPos;
        Vec2f offset;
        calcOffsetVector(&wall, addressOf(m_unk2bc->getLeftPos()), &newPos, &offset);
        m_unk2bc->setLeftPos(newPos);
    }
    if (m_collStatus->m_unk6C == 1 && (m_collStatus->m_unk6D & 2)) {
        // the wall is attached to a moving line: the shape was already carried along
        m_collStatus->m_currentCollShape->set(*m_unk2bc);
        int touching = m_collStatus->m_touchDown;
        if (m_touchMask != 0) {
            copyTouchLinesToStatus(this);
        }
        m_isFinished = true;
        return touching;
    }
    float rate = 1.0f;
    int touchId = 1;
    while (true) {
        const Vec2f& target = m_unk2c4->getLeftPos();
        fn_80111968(wall.m_collision, &seg, wall.m_collisionLine, 0);
        float y0 = seg.m_segment.m_p0Pos.m_y;
        float y1 = seg.m_segment.m_p1Pos.m_y;
        float maxY = psMax(y0, y1);
        float minY = psMin(y0, y1);
        bool leaves = isBeyondLine(m_unkE, target, minY, maxY);
        int found = 0;
        if (leaves) {
            found = getNextLeftLine(&wall, &next);
        }
        calcNewShapeOnLeft(&wall);
        int hit;
        if (m_collection->m_lineWorks.m_num == 0) {
            hit = 0;
        } else {
            hit = testCollisionMain();
        }
        if (hit) {
            m_isFinished = true;
            goto adjust;
        }
        if (!leaves) {
            m_isFinished = true;
            m_unk2bc->set(*m_unk2c0);
            goto adjust;
        }
        if (!found) {
            break;
        }
        wall = next;
        updateTouchLine(touchId, &wall, rate);
        fn_80111968(wall.m_collision, &nextSeg, wall.m_collisionLine, 0);
        Vec2f pos;
        if (m_unkE & DIR_UP) {
            if (nextSeg.m_segment.m_p0Pos.m_y < nextSeg.m_segment.m_p1Pos.m_y) {
                pos = nextSeg.m_segment.m_p0Pos;
            } else {
                pos = nextSeg.m_segment.m_p1Pos;
            }
        } else {
            if (nextSeg.m_segment.m_p0Pos.m_y < nextSeg.m_segment.m_p1Pos.m_y) {
                pos = nextSeg.m_segment.m_p1Pos;
            } else {
                pos = nextSeg.m_segment.m_p0Pos;
            }
        }
        m_unk2bc->setLeftPos(Vec2f(pos));
    }
    {
        // no further wall line: the shape leaves the wall
        m_collStatus->setStAir();
        fn_80134468(m_collStatus, 1);
        grCollisionLineWork neighbor;
        clSegment2D wallSeg;
        wall.m_collision->getSegment(&wallSeg, wall.m_collisionLine);
        if (m_unkE & DIR_UP) {
            if (wall.getPrev(&neighbor) && neighbor.m_collisionLine->m_isUpper) {
                clSegment2D neighborSeg;
                neighbor.m_collision->getSegment(&neighborSeg, neighbor.m_collisionLine);
                Vec2f top = m_unk2bc->getTopPos();
                Vec2f onLine;
                if (neighborSeg.projectionX(top, onLine) && onLine.m_y < top.m_y) {
                    m_unk2bc->setTopPos(onLine);
                }
                m_isFinished = true;
            } else {
                Vec2f pos(wallSeg.m_p0Pos.m_x, wallSeg.m_p0Pos.m_y + 0.01f);
                m_unk2bc->setLeftPos(pos);
                clearTouchRecordsAndMask(this);
            }
        } else {
            if (wall.getNext(&neighbor) && neighbor.m_collisionLine->m_isUnder && !(getLineAttr(neighbor.m_collisionLine) & m_exclusiveLineAttr)) {
                clSegment2D neighborSeg;
                neighbor.m_collision->getSegment(&neighborSeg, neighbor.m_collisionLine);
                Vec2f down = m_unk2bc->getDownPos();
                Vec2f onLine;
                if (neighborSeg.projectionX(down, onLine) && onLine.m_y > down.m_y) {
                    m_unk2bc->setDownPos(onLine);
                }
                m_isFinished = true;
            } else {
                Vec2f pos(wallSeg.m_p1Pos.m_x, wallSeg.m_p1Pos.m_y - 0.01f);
                m_unk2bc->setLeftPos(pos);
                clearTouchRecordsAndMask(this);
            }
        }
    }
adjust:
    if (!m_isFinished) {
        m_unk2c0->set(*m_unk2c4);
    }
    ajustShapeRight();
    if (m_isFinished) {
        ajustShape();
    }
    lastCorrection();
    m_unkF |= 6;
    m_collStatus->m_currentCollShape->set(*m_unk2bc);
    int touching = m_collStatus->m_touchDown;
    if (m_touchMask != 0) {
        copyTouchLinesToStatus(this);
    }
    return touching;
}

// Ceiling test of the moving shape: slides the top corner along the ceiling line touched last frame (m_touch[0]),
// stepping to the connected ceiling line while the corner leaves the current one, until the swept area hits something.
// When the ceiling ends the shape follows the neighbouring wall line or the touch is dropped. Returns the "touching" bit.
int grCollisionTest::testTopMain() {
    m_unk2D0 = 1; // HYPOTHESIS: which test is running (1 = ceiling)
    grCollSegmentEx seg;
    grCollSegmentEx nextSeg;
    grCollisionLineWork wall;
    grCollisionLineWork next;
    resetTouchRates(this);
    clAABBox2D box;
    getAABBox(&box);
    m_collStatus->m_unk94 = m_unk2bc->getTopPos();
    wall = m_touch[0].m_lineWork;
    if (m_collStatus->m_isCalcLineMove) {
        Vec2f newPos;
        Vec2f offset;
        calcOffsetVector(&wall, addressOf(m_unk2bc->getTopPos()), &newPos, &offset);
        m_unk2bc->setTopPos(newPos);
    }
    m_unkF = 0xF6;
    grCollStatus* status = m_collStatus;
    if (status->m_unk6C == 1 && (status->m_unk6D & 1)) {
        // the ceiling is attached to a moving line: the shape was already carried along
        m_collStatus->m_currentCollShape->set(*m_unk2bc);
        int touching = m_collStatus->m_touchDown;
        if (m_touchMask != 0) {
            copyTouchLinesToStatus(this);
        }
        m_isFinished = true;
        return touching;
    }
    if (isActiveLineWork(&status->m_collLineWorkArray[0])) {
        // the status still has an up touch line: widen the sweep box to the line so it is collected too
        float height = m_unk2c0->getHeight();
        Vec2f top = m_unk2bc->getTopPos();
        grCollisionLineWork* upLine = &status->m_collLineWorkArray[0];
        fn_80111968(upLine->m_collision, &seg, upLine->m_collisionLine, 0);
        float maxY = psMax(seg.m_segment.m_p0Pos.m_y, seg.m_segment.m_p1Pos.m_y);
        float minY = psMin(seg.m_segment.m_p0Pos.m_y, seg.m_segment.m_p1Pos.m_y);
        box.m_unk2 = psMin(box.m_unk2, psMin(maxY, top.m_y - height));
        box.m_unk4 = psMax(box.m_unk4, minY);
    }
    box.m_unk2 -= m_unk2bc->getCenterPos().m_y - m_unk2bc->getDownPos().m_y;
    box.m_unk1 -= m_unk2bc->getCenterPos().m_x - m_unk2bc->getLeftPos().m_x;
    box.m_unk3 += m_unk2bc->getRightPos().m_x - m_unk2bc->getCenterPos().m_x;
    updateCollection(&box);
    float rate = 1.0f;
    int touchId = 0;
    while (true) {
        fn_80111968(wall.m_collision, &seg, wall.m_collisionLine, 0);
        float x0 = seg.m_segment.m_p0Pos.m_x;
        float x1 = seg.m_segment.m_p1Pos.m_x;
        float maxX = psMax(x0, x1);
        float minX = psMin(x0, x1);
        bool leaves = isBeyondLineX(m_unkE, m_unk2c4->getTopPos(), minX, maxX);
        int found = 0;
        if (leaves) {
            found = getNextTopLine(&wall, &next);
        }
        calcNewShapeOnTop(&wall);
        int hit;
        if (m_collection->m_lineWorks.m_num == 0) {
            hit = 0;
        } else {
            hit = testCollisionMain();
        }
        if (hit) {
            m_unk2c0->set(*m_unk2bc);
            m_isFinished = true;
            goto adjust;
        }
        if (!leaves) {
            m_isFinished = true;
            m_unk2bc->set(*m_unk2c0);
            goto adjust;
        }
        if (!found) {
            break;
        }
        wall = next;
        updateTouchLine(touchId, &wall, rate);
        fn_80111968(wall.m_collision, &nextSeg, wall.m_collisionLine, 0);
        Vec2f pos;
        if (m_unkE & DIR_RIGHT) {
            if (nextSeg.m_segment.m_p0Pos.m_x < nextSeg.m_segment.m_p1Pos.m_x) {
                pos = nextSeg.m_segment.m_p0Pos;
            } else {
                pos = nextSeg.m_segment.m_p1Pos;
            }
        } else {
            if (nextSeg.m_segment.m_p0Pos.m_x < nextSeg.m_segment.m_p1Pos.m_x) {
                pos = nextSeg.m_segment.m_p1Pos;
            } else {
                pos = nextSeg.m_segment.m_p0Pos;
            }
        }
        m_unk2bc->setTopPos(Vec2f(pos));
    }
    {
        // no further ceiling line: the shape leaves the ceiling
        m_collStatus->setStAir();
        fn_80134468(m_collStatus, 0);
        grCollisionLineWork neighbor;
        clSegment2D wallSeg;
        wall.m_collision->getSegment(&wallSeg, wall.m_collisionLine);
        bool followed = false;
        if (m_unkE & DIR_RIGHT) {
            if (wall.getPrev(&neighbor) && isLinkedLineWork(&neighbor) && neighbor.m_collisionLine->m_isRight) {
                Vec2f pos = wallSeg.m_p0Pos;
                pos.m_x = pos.m_x - 0.01f;
                m_unk2bc->setTopPos(pos);
                followed = true;
            }
        } else {
            if (wall.getNext(&neighbor) && isLinkedLineWork(&neighbor) && neighbor.m_collisionLine->m_isLeft) {
                Vec2f pos = wallSeg.m_p1Pos;
                pos.m_x = pos.m_x + 0.01f;
                m_unk2bc->setTopPos(pos);
                followed = true;
            }
        }
        if (followed) {
            m_touch[0].m_lineWork = wall;
            m_isFinished = true;
        } else {
            clearTouchRecordsAndMask(this);
        }
    }
adjust:
    if (!m_isFinished) {
        m_unk2c0->set(*m_unk2c4);
    }
    ajustShape();
    ajustShapeDown();
    lastCorrection();
    m_unkF |= 9;
    m_collStatus->m_currentCollShape->set(*m_unk2bc);
    int touching = m_collStatus->m_touchDown;
    if (m_touchMask != 0) {
        copyTouchLinesToStatus(this);
    }
    return touching;
}

// Cliff (ledge) test: the status holds a candidate cliff line (m_collLineWorkArray[8], found by testCliffCheck). When the
// line can be hung from, the cliff edge on the facing side becomes the status cliff position and the shape is re-tested
// in the air; if it ends up in the cliff state it is kicked back to the air when something blocks the grab: a wall of
// the cliff's own side that moves away (or another line next to the cliff), or when the shape is too narrow.
// Returns the result of testAir (0 when there is no usable cliff line).
int grCollisionTest::testCliff() {
    grCollSegmentEx lineSeg;
    grCollisionLineWork cliffLine;
    grCollisionLineWork lineWork;
    clSegment2D seg;
    clSegment2D preSeg;
    grCollisionLineWork upLine;
    grCollisionLineWork nextLine;
    grCollStatus* status = m_collStatus;
    cliffLine = status->m_collLineWorkArray[8];
    bool valid = false;
    if (cliffLine.m_collisionLine != NULL && cliffLine.m_collisionJoint != NULL && cliffLine.m_collision != NULL) {
        valid = true;
    }
    lineWork = cliffLine;
    if (valid) {
        if (isActiveLineWork(&cliffLine) && cliffLine.m_collisionLine->m_isUnder) {
            bool hangRight = cliffLine.m_collisionLine->m_isHangR;
            if (hangRight || cliffLine.m_collisionLine->m_isHangL) {
                // HYPOTHESIS: the joint flags (0x2000 / 0x4000 of m_0x52) say on which side a ledge is not allowed
                u16 ledgeOk = ~cliffLine.m_collisionJoint->m_0x52;
                u16 sides = 0;
                if (hangRight && (ledgeOk & 0x2000)) {
                    sides = 1;
                }
                if (cliffLine.m_collisionLine->m_isHangL && (ledgeOk & 0x4000)) {
                    sides |= 2;
                }
                if (sides != 0) {
                    fn_80111968(lineWork.m_collision, &lineSeg, cliffLine.m_collisionLine, 0);
                    Vec2f edge;
                    if (m_unk20 == 0) {
                        edge = lineSeg.m_segment.m_p1Pos;
                    } else {
                        edge = lineSeg.m_segment.m_p0Pos;
                    }
                    m_collStatus->m_cliffPos = edge;
                    int result = testAir();
                    status = m_collStatus;
                    if (status->m_setting == grCollStatus::Setting_Air && !status->m_touchUp) {
                        status->setStCliff();
                    }
                    if (m_collStatus->m_setting == grCollStatus::Setting_Cliff) {
                        bool blocked = false;
                        bool movedAway = false;
                        grCollision* collision = lineWork.m_collision;
                        collision->getSegment(&seg, lineWork.m_collisionLine);
                        fn_801119F8(collision, &preSeg, lineWork.m_collisionLine);
                        // HYPOTHESIS: the average horizontal speed of the line; the original adds (seg.x1 - seg.x1) (always 0)
                        float speed = 0.5f * ((seg.m_p0Pos.m_x - preSeg.m_p0Pos.m_x) + (seg.m_p1Pos.m_x - seg.m_p1Pos.m_x));
                        if (m_unk20 == 1 && speed < -0.1f) {
                            movedAway = true;
                        } else if (m_unk20 == 0 && speed > 0.1f) {
                            movedAway = true;
                        }
                        if (m_unk20 == 1) {
                            for (int i = 0; i < 3; i++) {
                                if (m_collStatus->isTouch((grCollStatus::TouchId)sCliffRightTouch[i])) {
                                    grCollisionLineWork* touchLine = &m_collStatus->m_collLineWorkArray[sCliffRightTouch[i]];
                                    if (isActiveLineWork(touchLine) && touchLine->m_collisionLine->m_isRight &&
                                        lineWork.m_collisionJoint != touchLine->m_collisionJoint &&
                                        (!touchLine->getUpRightLine(&upLine) || !upLine.getNext(&nextLine) ||
                                         nextLine.m_collisionLine != cliffLine.m_collisionLine)) {
                                        blocked = true;
                                        goto cliffDone;
                                    }
                                }
                            }
                            for (int i = 0; i < 3; i++) {
                                if (m_collStatus->isTouch((grCollStatus::TouchId)sCliffLeftTouch[i])) {
                                    grCollisionLineWork* touchLine = &m_collStatus->m_collLineWorkArray[sCliffLeftTouch[i]];
                                    if (isActiveLineWork(touchLine) && touchLine->m_collisionLine->m_isLeft) {
                                        touchLine->m_collision->getSegment(&seg, touchLine->m_collisionLine);
                                        fn_801119F8(touchLine->m_collision, &preSeg, touchLine->m_collisionLine);
                                        if (0.5f * ((seg.m_p0Pos.m_x - preSeg.m_p0Pos.m_x) + (seg.m_p1Pos.m_x - seg.m_p1Pos.m_x)) > 0.1f) {
                                            blocked = true;
                                            break;
                                        }
                                        if (movedAway) {
                                            blocked = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        } else if (m_unk20 == 0) {
                            for (int i = 0; i < 3; i++) {
                                if (m_collStatus->isTouch((grCollStatus::TouchId)sCliffLeftTouch[i])) {
                                    grCollisionLineWork* touchLine = &m_collStatus->m_collLineWorkArray[sCliffLeftTouch[i]];
                                    if (isActiveLineWork(touchLine) && touchLine->m_collisionLine->m_isLeft &&
                                        lineWork.m_collisionJoint != touchLine->m_collisionJoint &&
                                        (!touchLine->getUpLeftLine(&upLine) || !upLine.getPrev(&nextLine) ||
                                         nextLine.m_collisionLine != cliffLine.m_collisionLine)) {
                                        blocked = true;
                                        goto cliffDone;
                                    }
                                }
                            }
                            for (int i = 0; i < 3; i++) {
                                if (m_collStatus->isTouch((grCollStatus::TouchId)sCliffRightTouch[i])) {
                                    grCollisionLineWork* touchLine = &m_collStatus->m_collLineWorkArray[sCliffRightTouch[i]];
                                    if (isActiveLineWork(touchLine) && touchLine->m_collisionLine->m_isRight) {
                                        touchLine->m_collision->getSegment(&seg, touchLine->m_collisionLine);
                                        fn_801119F8(touchLine->m_collision, &preSeg, touchLine->m_collisionLine);
                                        if (0.5f * ((seg.m_p0Pos.m_x - preSeg.m_p0Pos.m_x) + (seg.m_p1Pos.m_x - seg.m_p1Pos.m_x)) > 0.1f) {
                                            blocked = true;
                                            break;
                                        }
                                        if (movedAway) {
                                            blocked = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    cliffDone:
                        if (m_unk2bc->getWidth() < 0.22f) {
                            blocked = true;
                        }
                        if (blocked) {
                            m_collStatus->setStAir();
                        }
                    }
                    return result;
                }
            }
        }
    }
    m_collStatus->setStAir();
    return 0;
}

// Remembers the candidate cliff edge `pos` of `lineWork` in `info`. side: 0 = left edge, 1 = right edge.
// The first candidate is taken as is; afterwards a candidate (either side) replaces the stored one when it is
// strictly nearer to the reference point of the status.
void grCollisionTest::updateCliffInfo(grCollCliffInfo* info, grCollisionLineWork* lineWork, int side, const Vec2f* pos) {
    if (info->m_valid == 0) {
        info->m_valid = 1;
        info->m_side = side;
        info->m_pos = *pos;
        info->m_lineWork = lineWork;
    } else {
        grCollStatus* status = m_collStatus;
        Vec2f newDiff = status->m_unk1C - *pos;
        Vec2f oldDiff = status->m_unk1C - info->m_pos;
        float newDist = newDiff.lengthSq();
        float oldDist = oldDiff.lengthSq();
        if (side == 1 && newDist < oldDist) {
            info->m_side = side;
            info->m_pos = *pos;
            info->m_lineWork = lineWork;
        } else if (side == 0 && newDist < oldDist) {
            info->m_side = side;
            info->m_pos = *pos;
            info->m_lineWork = lineWork;
        }
    }
}

// Looks for a cliff edge the shape can grab this frame: among the lines collected around the swept area, floor lines
// with a hangable end whose neighbour does not continue as a wall (or floor behind a virtual link) are candidates for
// the right-hand (m_side 1) and left-hand (m_side 0) edge; updateCliffInfo keeps the nearest of each kind, the status'
// facing (m_unkB), m_cliffCheckSides and m_cliffSide decide which kinds are accepted, and checkCliffRayReach confirms
// that the shape centre can see the edge. On success the status gets the cliff line/position/side and switches to the
// cliff state (unless the cliff is already occupied). Returns 1 when a candidate was confirmed.
int grCollisionTest::testCliffCheck() {
    int found = 0;
    clAABBox2D preBox;
    clAABBox2D curBox;
    clAABBox2D unionBox;
    clAABBox2D sweepBox;
    grCollCliffInfo rightInfo;
    grCollCliffInfo leftInfo;
    grCollisionLineWork neighbor;
    grCollSegmentEx lineSeg;
    fn_80135BA0(m_collStatus, &preBox);
    fn_80135B94(m_collStatus, &curBox);
    preBox.logicalOR(curBox, unionBox);
    m_unk2bc->getAABBox(preBox);
    preBox.logicalOR(unionBox, sweepBox);
    grCollCollection* collection = m_collection;
    updateCollection(&sweepBox);
    rightInfo.m_valid = 0;
    leftInfo.m_valid = 0;
    grCollStatus* status = m_collStatus;
    u32 num = collection->m_lineWorks.m_num;
    int checkSides = status->m_cliffCheckSides;
    int facing = status->m_unkB;
    int cliffSide = status->m_cliffSide;
    if (num != 0) {
        for (u32 i = 0; i < num; i++) {
            grCollisionLineWork* lineWork = &collection->m_lineWorks.m_works[i];
            grCollisionLine* line = lineWork->m_collisionLine;
            if (line->m_isUnder) {
                u16 ledgeOk = ~lineWork->m_collisionJoint->m_0x52;
                if (ledgeOk & 0x6000) {
                    u32 ends = 0;
                    if (line->m_isHangL) {
                        ends |= 1;
                    }
                    if (line->m_isHangR) {
                        ends |= 2;
                    }
                    if (ends != 0) {
                        if (!(ledgeOk & 0x4000)) {
                            ends &= ~1;
                        }
                        if (!(ledgeOk & 0x2000)) {
                            ends &= ~2;
                        }
                        if (ends != 0) {
                            fn_80111968(lineWork->m_collision, &lineSeg, line, 0);
                            u32 rightEnd = (ends >> 1) & 1;
                            u32 leftEnd = ends & 1;
                            if ((rightEnd == 0 || !lineWork->getPrev(&neighbor) || (!neighbor.m_collisionLine->m_isLeft &&
                                 (line->m_unkA < 0 || !neighbor.m_collisionLine->m_isUnder))) &&
                                (leftEnd == 0 || !lineWork->getNext(&neighbor) || (!neighbor.m_collisionLine->m_isRight &&
                                 (line->m_unkC < 0 || !neighbor.m_collisionLine->m_isUnder)))) {
                                Vec2f* p0 = &lineSeg.m_segment.m_p0Pos;
                                Vec2f* p1 = &lineSeg.m_segment.m_p1Pos;
                                if (facing == 1) {
                                    if (checkSides == 0) {
                                        if (rightEnd != 0 && unionBox.checkInclude(*p0)) {
                                            updateCliffInfo(&rightInfo, lineWork, 1, p0);
                                        }
                                    } else if (checkSides == 1) {
                                        if (leftEnd != 0 && unionBox.checkInclude(*p1)) {
                                            updateCliffInfo(&leftInfo, lineWork, 0, p1);
                                        }
                                    } else if (checkSides == 2) {
                                        if (rightEnd != 0 && unionBox.checkInclude(*p0)) {
                                            updateCliffInfo(&rightInfo, lineWork, 1, p0);
                                        } else if (cliffSide != 1 && leftEnd != 0 && unionBox.checkInclude(*p1)) {
                                            updateCliffInfo(&leftInfo, lineWork, 0, p1);
                                        }
                                    }
                                } else if (facing == 0) {
                                    if (checkSides == 0) {
                                        if (leftEnd != 0 && unionBox.checkInclude(*p1)) {
                                            updateCliffInfo(&rightInfo, lineWork, 0, p1);
                                        }
                                    } else if (checkSides == 1) {
                                        if (rightEnd != 0 && unionBox.checkInclude(*p0)) {
                                            updateCliffInfo(&leftInfo, lineWork, 1, p0);
                                        }
                                    } else if (checkSides == 2) {
                                        if (leftEnd != 0 && unionBox.checkInclude(*p1)) {
                                            updateCliffInfo(&rightInfo, lineWork, 0, p1);
                                        } else if (cliffSide != 0 && rightEnd != 0 && unionBox.checkInclude(*p0)) {
                                            updateCliffInfo(&leftInfo, lineWork, 1, p0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        grCollCliffInfo* chosen = NULL;
        if (rightInfo.m_valid != 0) {
            chosen = &rightInfo;
            if (!checkCliffRayReach(addressOf(m_unk2bc->getCenterPos()), chosen)) {
                chosen = NULL;
            }
        }
        if (chosen == NULL && leftInfo.m_valid != 0) {
            chosen = &leftInfo;
            if (!checkCliffRayReach(addressOf(m_unk2bc->getCenterPos()), chosen)) {
                chosen = NULL;
            }
        }
        if (chosen != NULL) {
            u32 lineId = chosen->m_lineWork->getLineId();
            m_collStatus->m_cliffPos = chosen->m_pos;
            if (!lbl_805A0360->checkCliffOverlap(m_collStatus, lineId)) {
                m_collStatus->m_cliffDir = chosen->m_side;
                m_collStatus->m_collLineWorkArray[8] = *chosen->m_lineWork;
                m_collStatus->setStCliff();
            }
            found = 1;
        }
    }
    return found;
}

// Checks that the shape centre `origin` can reach the cliff edge of `info` in a straight line: the ray from the centre to
// the (slightly lowered) edge point must not cross another line, except the ones that continue the cliff line (the
// neighbour beyond the edge, and for a joint-sharing wall the line it joins). Returns 1 when the way is free.
int grCollisionTest::checkCliffRayReach(const Vec2f* origin, grCollCliffInfo* info) {
    clSegment2D seg;
    clSegment2D otherSeg;
    clSegment2D prevSeg;
    clSegment2D nextSeg;
    grCollisionLineWork lineWork = *info->m_lineWork;
    grCollisionLineWork prev;
    grCollisionLineWork next;
    grCollisionLineWork up;
    grCollision* collision = lineWork.m_collision;
    collision->getSegment(&seg, lineWork.m_collisionLine);
    Vec2f vec;
    if (info->m_side == 1) {
        if (lineWork.getPrev(&prev) && prev.m_collisionLine->m_isRight) {
            prev.m_collision->getSegment(&prevSeg, prev.m_collisionLine);
            Vec2f down(0.0f, -0.5f);
            Vec2f onPrev;
            prevSeg.projection1(seg.m_p0Pos + down, onPrev);
            vec = onPrev - *origin;
        } else {
            Vec2f down(0.0f, -0.5f);
            vec = (seg.m_p0Pos + down) - *origin;
        }
    } else {
        if (lineWork.getNext(&next) && next.m_collisionLine->m_isLeft) {
            next.m_collision->getSegment(&nextSeg, next.m_collisionLine);
            Vec2f down(0.0f, -0.5f);
            Vec2f onNext;
            nextSeg.projection1(seg.m_p1Pos + down, onNext);
            vec = onNext - *origin;
        } else {
            Vec2f down(0.0f, -0.5f);
            vec = (seg.m_p1Pos + down) - *origin;
        }
    }
    int side = info->m_side;
    grCollRay2D ray = { *origin, vec };
    lineWork.getPrev(&prev);
    lineWork.getNext(&next);
    grCollisionJoint* joint = lineWork.m_collisionJoint;
    int result = 1;
    u32 num = m_collection->m_lineWorks.m_num;
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* other = &m_collection->m_lineWorks.m_works[i];
        grCollisionLine* otherLine = other->m_collisionLine;
        if (isSameLine(lineWork.m_collisionLine, otherLine)) {
            continue;
        }
        if (side == 0) {
            // the line that continues the cliff line on its left-hand side is not an obstacle
            if (otherLine->m_isLeft && isSameLine(next.m_collisionLine, otherLine)) {
                continue;
            }
            if (otherLine->m_isUpper && isSameLine(next.m_collisionLine, otherLine)) {
                continue;
            }
        } else {
            if (otherLine->m_isRight && isSameLine(prev.m_collisionLine, otherLine)) {
                continue;
            }
            if (otherLine->m_isUpper && isSameLine(prev.m_collisionLine, otherLine)) {
                continue;
            }
        }
        other->m_collision->getSegment(&otherSeg, otherLine);
        // MATCH-ONLY: the ray is built by word copies, so the class (with its out-of-line constructor) is not used
        if (!((const clRay2D*)&ray)->checkIntersect(otherSeg)) {
            continue;
        }
        if (other->m_collisionJoint != joint) {
            result = 0;
            break;
        }
        if (info->m_side == 1) {
            if (otherLine->m_isLeft) {
                result = 0;
                break;
            }
            if (otherLine->m_isRight) {
                other->getUpRightLine(&up);
                up.getNext(&next);
                if (!isLinkedLineWork(&next)) {
                    continue;
                }
            } else {
                if (otherLine->m_isUpper) {
                    result = 0;
                    break;
                }
                if (otherLine->m_isUnder && !isSameLine(otherLine, prev.m_collisionLine)) {
                    result = 0;
                    break;
                }
            }
        } else if (info->m_side == 0) {
            if (otherLine->m_isRight) {
                result = 0;
                break;
            }
            if (otherLine->m_isLeft) {
                other->getUpLeftLine(&up);
                up.getPrev(&next);
                if (!isLinkedLineWork(&next)) {
                    continue;
                }
            } else {
                if (otherLine->m_isUpper) {
                    result = 0;
                    break;
                }
                if (otherLine->m_isUnder && !isSameLine(otherLine, next.m_collisionLine)) {
                    result = 0;
                    break;
                }
            }
        }
    }
    return result;
}

// ===== cluster D =====
// MATCH-ONLY: the Vec2f locals of this unit are copy-initialised from the shape getters with word moves (plain-base Vec2f).

// HYPOTHESIS: grCollisionLineWork::getPrevNoVLink / getNextNoVLink (like getPrev / getNext, but they do not follow the
// virtual link to another collision).

// HYPOTHESIS: grCollStatus::isIgnoreLine (the line work is on the status' ignore list).
// HYPOTHESIS: grCollisionLineWork::isLinkPrevSameDirFlag / isLinkNextSameDirFlag (walk the left / right links of the same attribute).
// HYPOTHESIS: grCollisionLineWork::getUpRightLine / getUpLeftLine (the line above-right / above-left of the end of the line).
// HYPOTHESIS: grCollStatus::addIgnoreLine (puts the line work on the status' ignore list).
extern "C" void fn_80137074(grCollStatus* status, grCollisionLineWork* lineWork);
// HYPOTHESIS: grCollStatus::setStGroundInternal (grounded without the callbacks of setStGround).

// True when the line work is complete and both its joint and its collision are enabled.
static inline bool isActiveLineWork(const grCollisionLineWork& lineWork) {
    bool valid = lineWork.m_collisionLine != NULL && lineWork.m_collisionJoint != NULL && lineWork.m_collision != NULL;
    bool active;
    if (valid && lineWork.m_collisionJoint->m_0x54_4 && lineWork.m_collision->m_isEnabled) {
        active = true;
    } else {
        active = false;
    }
    return active;
}

// The same test as isActiveLineWork, written with the complete test as an if (this is how the neighbor checks of
// testCollisionMainGround compile).
static inline bool isActiveLineWorkIf(const grCollisionLineWork& lineWork) {
    bool valid = false;
    if (lineWork.m_collisionLine != NULL && lineWork.m_collisionJoint != NULL && lineWork.m_collision != NULL) {
        valid = true;
    }
    bool active;
    if (valid && lineWork.m_collisionJoint->m_0x54_4 && lineWork.m_collision->m_isEnabled) {
        active = true;
    } else {
        active = false;
    }
    return active;
}

// Skips (continue) a wall line whose neighbor reached through `getter` is an enabled line work of the ground line.
#define SKIP_IF_GROUND_NEIGHBOR(getter)                                \
    if (getter) {                                                      \
        if (isActiveLineWork(neighbor)) {                              \
            bool same = false;                                         \
            if (neighbor.m_collisionLine == groundLine) {              \
                same = true;                                           \
            }                                                          \
            if (same) {                                                \
                continue;                                              \
            }                                                          \
        }                                                              \
    }

// m_unk10 is a mask of touch ids (grCollStatus::TouchId bits); the lines are filtered by the matching
// line-attribute bits (up -> 2, left -> 4, right -> 8, down -> 1 in grCollisionLine::m_unkE).
u16 grCollisionTest::createCheckMaskStatic() {
    u16 mask = 0;
    if (m_unk10 & grCollStatus::TOUCH_MASK_UP) {
        mask |= 2;
    }
    if (m_unk10 & grCollStatus::TOUCH_MASK_DOWN) {
        mask |= 1;
    }
    if (m_unk10 & grCollStatus::TOUCH_MASK_LEFT) {
        mask |= 4;
    }
    if (m_unk10 & grCollStatus::TOUCH_MASK_RIGHT) {
        mask |= 8;
    }
    return mask;
}

// The static correction at the start of a pass: pushes the shape out of the walls (left/right), then out of the
// floor/ceiling (up/down), keeps the touched lines and refits the work shape. Returns true when the shape was moved.
int grCollisionTest::initialCorrection() {
    m_unk2C8 = 0;
    m_unkC_6 = false;
    m_unk304 = m_unk2bc->getRightPos();
    m_unk2FC = m_unk2bc->getLeftPos();
    m_unk2EC = m_unk2bc->getTopPos();
    m_unk2F4 = m_unk2bc->getDownPos();
    m_unk2DC = m_unk304.m_x - m_unk2FC.m_x;
    m_unk2E0 = m_unk2EC.m_y - m_unk2F4.m_y;
    // HYPOTHESIS: the two found lines are cleared member by member (the flag bit is left alone).
    m_unk31C.m_collisionLine = NULL;
    m_unk31C.m_collisionJoint = NULL;
    m_unk31C.m_collision = NULL;
    m_unk31C.m_unkC = 0;
    m_unk30C.m_collisionLine = NULL;
    m_unk30C.m_collisionJoint = NULL;
    m_unk30C.m_collision = NULL;
    m_unk30C.m_unkC = 0;
    m_unkC_2 = false;
    m_unkC_1 = false;
    testStaticCorrectionSetup();
    m_unk2E4.m_x = (m_unk2bc->getLeftPos().m_x + m_unk2bc->getRightPos().m_x) * 0.5f;
    m_unk2E4.m_y = (m_unk2bc->getTopPos().m_y + m_unk2bc->getDownPos().m_y) * 0.5f;

    int result = 0;
    if (initialCorrectionLR()) {
        initialCorrectionLR();
        result = 1;
    }
    if (m_sides2C8.m_right && m_sides2C8.m_left) {
        // squeezed between two walls: when the shape became narrower, refit it between them
        if (m_unk304.m_x - m_unk2FC.m_x < m_unk2DC) {
            Vec2f center = m_unk2bc->getCenterPos();
            if (m_unk2FC.m_x < m_unk304.m_x) {
                if (m_unk2FC.m_x < center.m_x && center.m_x < m_unk304.m_x) {
                    m_unk2bc->setLR(m_unk2FC, m_unk304);
                } else {
                    center = (m_unk304 + m_unk2FC) * 0.5f;
                    m_unk2bc->setLR(center, m_unk2FC, m_unk304);
                }
            } else {
                // no room at all: a thin shape around the old center
                Vec2f left;
                Vec2f right;
                left.m_y = center.m_y;
                right.m_y = center.m_y;
                left.m_x = center.m_x - 0.1f;
                right.m_x = center.m_x + 0.1f;
                m_unk2bc->setLR(center, left, right);
                m_unkC_1 = true;
            }
            m_collStatus->m_collLineWorkArray[1] = m_touch[1].m_lineWork;
            m_collStatus->m_collLineWorkArray[2] = m_touch[2].m_lineWork;
            m_collStatus->m_touchLeft = true;
            m_collStatus->m_touchRight = true;
        }
    }

    Vec2f center = m_unk2bc->getCenterPos();
    Vec2f right = m_unk2bc->getRightPos();
    Vec2f left = m_unk2bc->getLeftPos();
    if (initialCorrectionUD()) {
        if (m_sides2C8.m_down) {
            if (m_sides2C8.m_right || m_sides2C8.m_left) {
                Vec2f top = m_unk2bc->getTopPos();
                Vec2f down = m_unk2bc->getDownPos();
                down.m_y = m_unk2F4.m_y;
                m_unk2bc->setLR(center, left, right);
                if (down.m_y < top.m_y) {
                    if (center.m_y <= down.m_y || top.m_y <= center.m_y) {
                        center.m_y = (top.m_y + down.m_y) * 0.5f;
                    }
                } else {
                    down.m_y = m_unk2F4.m_y + 0.01f;
                    down.m_x = center.m_x;
                    center.m_y = down.m_y + 0.1f;
                    top.m_y = down.m_y + 0.2f;
                    top.m_x = center.m_x;
                    m_unk2bc->setUD(center, top, down);
                    m_unkC_2 = true;
                }
                if (center.m_y >= top.m_y || center.m_y <= down.m_y) {
                    center.m_y = (top.m_y + down.m_y) * 0.5f;
                }
                m_unk2bc->setUD(center, top, down);
            }
        }
        initialCorrectionUD();
        result = 1;
    }
    if (m_collStatus->m_setting == grCollStatus::Setting_Ground) {
        m_unk2C8 |= SIDE_DOWN;
    }
    if (m_sides2C8.m_up) {
        m_touch[0].m_id = 4;
        m_touch[0].m_rate = 1.0f;
        m_touch[0].m_lineWork = m_unk30C;
    }
    if (m_sides2C8.m_down) {
        grCollStatus* status = m_collStatus;
        m_unk224 = m_touch[3].m_lineWork;
        status->m_collLineWorkArray[3] = m_touch[3].m_lineWork;
        status->m_touchDown = true;
        fn_801340A0(m_collStatus);
    }
    if (m_sides2C8.m_up && m_sides2C8.m_down) {
        // squeezed between floor and ceiling
        float height = m_unk2EC.m_y - m_unk2F4.m_y;
        Vec2f center = (m_unk2EC + m_unk2F4) * 0.5f;
        Vec2f up;
        Vec2f down;
        if (height > 0.021f) {
            if (height < m_unk2E0) {
                m_unk2bc->setUD(center, m_unk2EC, m_unk2F4);
            }
        } else {
            bool hasCeiling = false;
            if (m_unk31C.m_collisionLine != NULL && m_unk31C.m_collisionJoint != NULL && m_unk31C.m_collision != NULL) {
                hasCeiling = true;
            }
            if (hasCeiling && m_unk31C.m_collision->m_isDynamic) {
                // the ceiling is a moving collision: keep the shape just below it
                up = m_unk2EC + Vec2f(0.0f, -0.01f);
                down.m_x = m_unk2F4.m_x;
                center.m_y = up.m_y - 0.1f;
                down.m_y = up.m_y - 0.2f;
                m_unk2bc->setUD(center, up, down);
                m_unkC_2 = true;
            } else {
                down = m_unk2F4 + Vec2f(0.0f, 0.01f);
                up.m_x = m_unk2EC.m_x;
                center.m_y = down.m_y + 0.1f;
                up.m_y = down.m_y + 0.2f;
                m_unk2bc->setUD(center, up, down);
                m_unkC_2 = true;
            }
            m_collStatus->m_collLineWorkArray[0] = m_touch[0].m_lineWork;
            m_collStatus->m_touchUp = true;
        }
    }
    if (m_sides2C8.m_right && m_sides2C8.m_left && m_collStatus->m_setting == grCollStatus::Setting_Air) {
        Vec2f pos = m_unk2c0->getCenterPos();
        m_unk2c0->set(*m_unk2bc);
        m_unk2c0->setCenterPos(pos);
    }
    m_unk2E4.m_x = (m_unk2bc->getLeftPos().m_x + m_unk2bc->getRightPos().m_x) * 0.5f;
    m_unk2E4.m_y = (m_unk2bc->getTopPos().m_y + m_unk2bc->getDownPos().m_y) * 0.5f;
    return result;
}

// Pushes the shape out of the wall lines (left and right touches) it overlaps; true when something was pushed.
// While standing on the ground, a wall line that continues the ground line is left alone (the corner of floor and wall).
int grCollisionTest::initialCorrectionLR() {
    grCollCollection* collection = m_collection;
    u16 num = collection->m_lineWorks.m_num;
    int pushed = 0;
    m_unk2CA = 0;
    u16 mask = createCheckMaskStatic();
    grCollisionLineWork neighbor;
    grCollisionLine* groundLine = m_touch[3].m_lineWork.m_collisionLine;
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        grCollisionLine* line = lineWork->m_collisionLine;
        if (line->m_unkE & mask) {
            if (line->m_isLeft || line->m_isRight) {
                if (m_collStatus->m_setting == grCollStatus::Setting_Ground) {
                    if (line->m_isRight) {
                        SKIP_IF_GROUND_NEIGHBOR(lineWork->getNext(&neighbor))
                        SKIP_IF_GROUND_NEIGHBOR(lineWork->getNextNoVLink(&neighbor))
                    } else if (line->m_isLeft) {
                        SKIP_IF_GROUND_NEIGHBOR(lineWork->getPrev(&neighbor))
                        SKIP_IF_GROUND_NEIGHBOR(lineWork->getPrevNoVLink(&neighbor))
                    }
                }
                if (testStaticCorrection(lineWork)) {
                    pushed++;
                }
            }
        }
    }
    m_unk2C8 |= m_unk2CA;
    return pushed != 0;
}

// Pushes the shape out of the floor/ceiling lines (up and down touches) it overlaps; true when something was pushed.
int grCollisionTest::initialCorrectionUD() {
    grCollCollection* collection = m_collection;
    u16 num = collection->m_lineWorks.m_num;
    int pushed = 0;
    m_unk2CA = 0;
    u16 mask = createCheckMaskStatic();
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        grCollisionLine* line = lineWork->m_collisionLine;
        if (line->m_unkE & mask) {
            if (line->m_isUpper || line->m_isUnder) {
                if (testStaticCorrection(lineWork)) {
                    pushed++;
                }
            }
        }
    }
    m_unk2C8 |= m_unk2CA;
    return pushed != 0;
}

// The static correction at the end of a pass: the same pushes as initialCorrection, with the last-pass virtuals, and a
// refit that keeps the relative position of the shape. Returns true when the shape was moved.
int grCollisionTest::lastCorrection() {
    if (m_collection->m_lineWorks.m_num == 0) {
        return 0;
    }
    m_unkC_6 = true;
    m_unk2CA = 0;
    m_unk2C9 = m_unk2CB;
    m_unk304 = m_unk2bc->getRightPos();
    m_unk2FC = m_unk2bc->getLeftPos();
    m_unk2EC = m_unk2bc->getTopPos();
    m_unk2F4 = m_unk2bc->getDownPos();
    m_unk2DC = m_unk304.m_x - m_unk2FC.m_x;
    m_unk2E0 = m_unk2EC.m_y - m_unk2F4.m_y;

    int result = 0;
    if (lastCorrectionLR()) {
        lastCorrectionLR();
        result = 1;
    }
    if (m_sides2C9.m_right) {
        m_collStatus->m_touchRight = true;
    }
    if (m_sides2C9.m_left) {
        m_collStatus->m_touchLeft = true;
    }
    if (m_sides2C9.m_right && m_sides2C9.m_left) {
        // squeezed between two walls: when the shape became narrower, refit it between them
        if (m_unk304.m_x - m_unk2FC.m_x < m_unk2DC) {
            Vec2f center = m_unk2bc->getCenterPos();
            if (m_unk2FC.m_x < m_unk304.m_x) {
                if (m_unk2FC.m_x < center.m_x && center.m_x < m_unk304.m_x) {
                    m_unk2bc->setLR(m_unk2FC, m_unk304);
                } else {
                    // keep the relative position of the center inside the (now narrower) shape
                    float rightX = m_unk2bc->getRightPos().m_x;
                    float leftX = m_unk2bc->getLeftPos().m_x;
                    float ratio = (center.m_x - leftX) / (rightX - leftX);
                    center.m_x = m_unk2FC.m_x + ratio * (m_unk304.m_x - m_unk2FC.m_x);
                    if (center.m_x < m_unk2FC.m_x) {
                        center.m_x = (m_unk2FC.m_x + m_unk304.m_x) * 0.5f;
                    }
                    m_unk2bc->setLR(center, m_unk2FC, m_unk304);
                }
            } else {
                Vec2f left = m_unk2FC;
                Vec2f right = m_unk304;
                left.m_x = center.m_x - 0.1f;
                right.m_x = center.m_x + 0.1f;
                m_unk2bc->setLR(center, left, right);
            }
            m_collStatus->m_collLineWorkArray[1] = m_touch[1].m_lineWork;
            m_collStatus->m_collLineWorkArray[2] = m_touch[2].m_lineWork;
            m_collStatus->m_touchLeft = true;
            m_collStatus->m_touchRight = true;
        }
    }

    Vec2f center = m_unk2bc->getCenterPos();
    Vec2f right = m_unk2bc->getRightPos();
    Vec2f left = m_unk2bc->getLeftPos();
    if (lastCorrectionUD()) {
        if (m_sides2C8.m_down) {
            if (m_sides2C8.m_right || m_sides2C8.m_left) {
                Vec2f top = m_unk2bc->getTopPos();
                Vec2f down = m_unk2bc->getDownPos();
                down.m_y = m_unk2F4.m_y;
                m_unk2bc->setLR(center, left, right);
                if (center.m_y > top.m_y || center.m_y < down.m_y) {
                    center.m_y = (top.m_y + down.m_y) * 0.5f;
                }
                m_unk2bc->setUD(center, top, down);
            }
        }
        lastCorrectionUD();
        result = 1;
    }
    if (m_sides2C9.m_down) {
        if (m_collStatus->m_setting == grCollStatus::Setting_Air || m_collStatus->m_setting == grCollStatus::Setting_Air) { // MATCH-ONLY: the test is duplicated
            fn_801340A0(m_collStatus);
            m_collStatus->m_collLineWorkArray[3] = m_unk31C;
            m_unk224 = m_unk31C;
        }
    }
    if (m_collStatus->m_setting == grCollStatus::Setting_Ground) {
        m_unk2C9 |= SIDE_DOWN;
    }
    if (m_sides2C9.m_up) {
        bool hasCeiling = false;
        if (m_unk30C.m_collisionLine != NULL && m_unk30C.m_collisionJoint != NULL && m_unk30C.m_collision != NULL) {
            hasCeiling = true;
        }
        if (hasCeiling) {
            m_touch[0].m_id = 4;
            m_touch[0].m_rate = 0.0f;
            m_touch[0].m_lineWork = m_unk30C;
            m_collStatus->m_collLineWorkArray[0] = m_unk30C;
            m_collStatus->m_touchUp = true;
        }
    }
    if (m_sides2C9.m_up && m_sides2C9.m_down) {
        // squeezed between floor and ceiling
        float height = m_unk2EC.m_y - m_unk2F4.m_y;
        Vec2f center;
        center.m_y = (m_unk2EC.m_y + m_unk2F4.m_y) * 0.5f;
        center.m_x = m_unk2bc->getCenterPos().m_x;
        if (height > 0.21f) {
            if (height < m_unk2E0) {
                m_unk2bc->setUD(center, m_unk2EC, m_unk2F4);
            }
        } else {
            Vec2f up = m_unk2EC;
            Vec2f down = m_unk2F4 + Vec2f(0.0f, 0.01f);
            center.m_y = down.m_y + 0.1f;
            up.m_y = down.m_y + 0.2f;
            m_unk2bc->setUD(center, up, down);
        }
    }
    m_unk2bc->updateAABBox();
    updateIgnoreLine();
    return result;
}

// Same as initialCorrectionLR for the last pass (the result goes to m_unk2C9).
int grCollisionTest::lastCorrectionLR() {
    grCollCollection* collection = m_collection;
    u16 num = collection->m_lineWorks.m_num;
    int pushed = 0;
    m_unk2CA = 0;
    u16 mask = createCheckMaskStatic();
    grCollisionLineWork neighbor;
    grCollisionLine* groundLine = m_touch[3].m_lineWork.m_collisionLine;
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        grCollisionLine* line = lineWork->m_collisionLine;
        if (line->m_unkE & mask) {
            if (line->m_isLeft || line->m_isRight) {
                if (m_collStatus->m_setting == grCollStatus::Setting_Ground) {
                    if (line->m_isRight) {
                        SKIP_IF_GROUND_NEIGHBOR(lineWork->getNext(&neighbor))
                        SKIP_IF_GROUND_NEIGHBOR(lineWork->getNextNoVLink(&neighbor))
                    } else if (line->m_isLeft) {
                        SKIP_IF_GROUND_NEIGHBOR(lineWork->getPrev(&neighbor))
                        SKIP_IF_GROUND_NEIGHBOR(lineWork->getPrevNoVLink(&neighbor))
                    }
                }
                if (testStaticCorrection(lineWork)) {
                    pushed++;
                }
            }
        }
    }
    m_unk2C9 |= m_unk2CA;
    return pushed != 0;
}

#undef SKIP_IF_GROUND_NEIGHBOR

int grCollisionTest::lastCorrectionUD() {
    grCollCollection* collection = m_collection;
    u16 num = collection->m_lineWorks.m_num;
    int pushed = 0;
    m_unk2CA = 0;
    u16 mask = createCheckMaskStatic();
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        grCollisionLine* line = lineWork->m_collisionLine;
        if (line->m_unkE & mask) {
            if (line->m_isUpper || line->m_isUnder) {
                if (testStaticCorrection(lineWork)) {
                    pushed++;
                }
            }
        }
    }
    m_unk2C9 |= m_unk2CA;
    return pushed != 0;
}

// Lifts the shape's bottom onto the highest floor-facing (isUnder) line under it, stays below the top (for a ceiling hit from
// below, testTopMain): records the line as the down touch.
void grCollisionTest::ajustShapeDown() {
    m_unk2CB = 0;
    u16 mask = createCheckMaskStatic();
    u16 num = m_collection->m_lineWorks.m_num;
    Vec2f top = m_unk2bc->getTopPos();
    Vec2f down = m_unk2bc->getDownPos();
    grCollisionLineWork* best = NULL;
    float x = down.m_x;
    float y = down.m_y;
    float limit = top.m_y - 0.2f;
    grCollisionLineWork current;
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        current = *lineWork;
        if (isActiveLineWork(current) && (current.m_collisionLine->m_unkE & mask) && current.m_unkC == 0 &&
            fn_80137114(m_collStatus, &current) == 0 && current.m_collisionLine->m_isUnder) {
            clSegment2D segment;
            current.m_collision->getSegment(&segment, current.m_collisionLine);
            if (segment.m_p0Pos.m_x <= x && x <= segment.m_p1Pos.m_x) {
                Vec2f hit;
                if (segment.projectionX(down, hit)) {
                    float newY = 0.01f + hit.m_y;
                    if (y < newY && newY < limit) {
                        y = newY;
                        best = lineWork;
                    }
                }
            }
        }
    }
    if (best != NULL) {
        down.m_x = x;
        down.m_y = y;
        if (down.m_y >= limit - 0.1f) {
            down.m_y = limit - 0.1f;
        }
        Vec2f center = (top + down) * 0.5f;
        m_unk2bc->setUD(center, top, down);
        if (center.m_y > m_unk2E4.m_y) {
            m_unk2E4.m_y = center.m_y;
        }
        m_touch[3].m_id = 3;
        m_touch[3].m_rate = 0.0f;
        m_touch[3].m_lineWork = *best;
        m_touchMask |= 8;
        m_collStatus->m_collLineWorkArray[3] = *best;
        m_collStatus->m_touchDown = true;
    }
}

// Pushes the top of the shape (as tall as the target shape) under the lowest ceiling-type line above the feet, refits the
// shape and records the up touch. The lines the shape passes in the second pass are put on the status' ignore list.
void grCollisionTest::ajustShapeUp() {
    m_unk2CB = 0;
    u16 mask = createCheckMaskStatic();
    u16 num = m_collection->m_lineWorks.m_num;
    float height = m_unk2c4->getTopPos().m_y - m_unk2c4->getDownPos().m_y;
    Vec2f down = m_unk2bc->getDownPos();
    Vec2f top = down + Vec2f(0.0f, height);
    m_unk2bc->getCenterPos();
    float x = top.m_x;
    float y = top.m_y;
    float limit = 0.2f + down.m_y;
    grCollisionLineWork* best = NULL;
    grCollisionLineWork current;
    clSegment2D segment;
    Vec2f hit;
    bool hasGround = false;
    bool valid = false;
    if (m_unk224.m_collisionLine != NULL && m_unk224.m_collisionJoint != NULL && m_unk224.m_collision != NULL) {
        valid = true;
    }
    if (valid) {
        hasGround = true;
    }
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        current = *lineWork;
        if (isActiveLineWork(current) && (current.m_collisionLine->m_unkE & mask) && current.m_unkC == 0 &&
            current.m_collisionLine->m_isUpper) {
            current.m_collision->getSegment(&segment, current.m_collisionLine);
            if (segment.m_p1Pos.m_x <= x && x <= segment.m_p0Pos.m_x) {
                if (segment.projectionX(top, hit)) {
                    float newY = hit.m_y - 0.01f;
                    if (limit < newY && newY < y) {
                        y = newY;
                        best = lineWork;
                    }
                }
            }
        }
    }
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        grCollisionLine* line = lineWork->m_collisionLine;
        if (line->m_isUnder && isActiveLineWork(*lineWork) && (line->m_unkE & mask)) {
            if (hasGround) {
                bool same = false;
                if (line == m_unk224.m_collisionLine) {
                    same = true;
                }
                if (same) {
                    continue;
                }
                if (m_unk224.isLinkSameDirFlag(lineWork)) {
                    continue;
                }
            }
            lineWork->m_collision->getSegment(&segment, lineWork->m_collisionLine);
            if (segment.m_p1Pos.m_x <= x && x <= segment.m_p0Pos.m_x) {
                if (segment.projectionX(top, hit)) {
                    float newY = hit.m_y;
                    if (limit < newY && newY < y) {
                        fn_80137074(m_collStatus, lineWork);
                    }
                }
            }
        }
    }
    if (best != NULL) {
        top.m_x = x;
        top.m_y = y;
        Vec2f center = (top + down) * 0.5f;
        m_unk2bc->setUD(center, top, down);
        if (center.m_y < m_unk2E4.m_y) {
            m_unk2E4.m_y = center.m_y;
        }
        m_unk2CB |= SIDE_UP;
        m_touch[0].m_id = 0;
        m_touch[0].m_rate = 0.0f;
        m_touch[0].m_lineWork = *best;
        m_touchMask |= 1;
        m_collStatus->m_collLineWorkArray[0] = *best;
        m_collStatus->m_touchUp = true;
    }
}

// Pushes the shape's left edge out to the nearest left-facing wall line on its left (after a hit against a right wall in
// testRightWallMain), refits the shape and records the line as the left touch.
void grCollisionTest::ajustShapeLeft() {
    m_unk2CB = 0;
    u16 mask = createCheckMaskStatic();
    u16 num = m_collection->m_lineWorks.m_num;
    Vec2f left = m_unk2bc->getLeftPos();
    Vec2f right = m_unk2bc->getRightPos();
    grCollisionLineWork* best = NULL;
    float x = left.m_x;
    float y = left.m_y;
    float limit = right.m_x - 0.2f;
    grCollisionLineWork current;
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        current = *lineWork;
        if (isActiveLineWork(current) && (current.m_collisionLine->m_unkE & mask) && current.m_unkC == 0 &&
            current.m_collisionLine->m_isLeft) {
            clSegment2D segment;
            current.m_collision->getSegment(&segment, current.m_collisionLine);
            if (segment.m_p0Pos.m_y >= y && y >= segment.m_p1Pos.m_y) {
                Vec2f hit;
                if (segment.projectionY(left, hit)) {
                    float newX = 0.01f + hit.m_x;
                    if (x < newX && newX < limit) {
                        x = newX;
                        best = lineWork;
                    }
                }
            }
        }
    }
    if (best != NULL) {
        left.m_x = x;
        left.m_y = y;
        if (left.m_x < m_unk2bc->getCenterPos().m_x) {
            m_unk2bc->setLR(left, right);
        } else {
            Vec2f center = (left + right) * 0.5f;
            m_unk2bc->setLR(center, left, right);
        }
        Vec2f newCenter = m_unk2bc->getCenterPos();
        if (m_unk2E4.m_x < newCenter.m_x) {
            m_unk2E4.m_x = newCenter.m_x;
        }
        m_unk2CB |= SIDE_LEFT;
        m_touch[1].m_id = 1;
        m_touch[1].m_rate = 0.0f;
        m_touch[1].m_lineWork = *best;
        m_touchMask |= 2;
        m_collStatus->m_collLineWorkArray[1] = *best;
        m_collStatus->m_touchLeft = true;
    }
}

// Mirror of ajustShapeLeft for the right edge (after a hit against a left wall in testLeftWallMain).
void grCollisionTest::ajustShapeRight() {
    m_unk2CB = 0;
    u16 mask = createCheckMaskStatic();
    u16 num = m_collection->m_lineWorks.m_num;
    Vec2f left = m_unk2bc->getLeftPos();
    Vec2f right = m_unk2bc->getRightPos();
    grCollisionLineWork* best = NULL;
    float limit = 0.2f + left.m_x;
    float x = right.m_x;
    float y = right.m_y;
    grCollisionLineWork current;
    for (u32 i = 0; i < num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        current = *lineWork;
        if (isActiveLineWork(current) && (current.m_collisionLine->m_unkE & mask) && current.m_unkC == 0 &&
            current.m_collisionLine->m_isRight) {
            clSegment2D segment;
            current.m_collision->getSegment(&segment, current.m_collisionLine);
            if (segment.m_p1Pos.m_y >= y && y >= segment.m_p0Pos.m_y) {
                Vec2f hit;
                if (segment.projectionY(right, hit)) {
                    float newX = hit.m_x - 0.01f;
                    if (limit < newX && newX < x) {
                        x = newX;
                        best = lineWork;
                    }
                }
            }
        }
    }
    if (best != NULL) {
        right.m_x = x;
        right.m_y = y;
        if (m_unk2bc->getCenterPos().m_x < right.m_x) {
            m_unk2bc->setLR(left, right);
        } else {
            Vec2f center = (left + right) * 0.5f;
            m_unk2bc->setLR(center, left, right);
        }
        Vec2f newCenter = m_unk2bc->getCenterPos();
        if (newCenter.m_x < m_unk2E4.m_x) {
            m_unk2E4.m_x = newCenter.m_x;
        }
        m_unk2CB |= SIDE_RIGHT;
        m_touch[2].m_id = 2;
        m_touch[2].m_rate = 0.0f;
        m_touch[2].m_lineWork = *best;
        m_touchMask |= 4;
        m_collStatus->m_collLineWorkArray[2] = *best;
        m_collStatus->m_touchRight = true;
    }
}

void grCollisionTest::ajustShape() { }

// Publishes the line touched on the side of the nearest hit (m_hit.m_id) to the grCollStatus: copies the touch record's
// line work into its touch line array and sets the matching touch flag.
void grCollisionTest::updateCollStatusTouchInfo() {
    if (m_touchMask == 0) {
        return;
    }
    u8 id = m_hit.m_id;
    if (id == grCollStatus::Touch_Down) {
        grCollStatus* status = m_collStatus;
        status->m_collLineWorkArray[3] = m_touch[3].m_lineWork;
        status->m_touchDown = true;
        fn_801340A0(m_collStatus);
    } else if (id == grCollStatus::Touch_Up) {
        m_collStatus->m_collLineWorkArray[0] = m_touch[0].m_lineWork;
        m_collStatus->m_touchUp = true;
    } else if (id == grCollStatus::Touch_Right) {
        m_collStatus->m_collLineWorkArray[2] = m_touch[2].m_lineWork;
        m_collStatus->m_touchRight = true;
    } else if (id == grCollStatus::Touch_Left) {
        m_collStatus->m_collLineWorkArray[1] = m_touch[1].m_lineWork;
        m_collStatus->m_touchLeft = true;
    } else if (id == grCollStatus::Touch_Up_Left) {
        m_collStatus->m_collLineWorkArray[4] = m_touch[4].m_lineWork;
        m_collStatus->m_touchUpLeft = true;
    } else if (id == grCollStatus::Touch_Up_Right) {
        m_collStatus->m_collLineWorkArray[5] = m_touch[5].m_lineWork;
        m_collStatus->m_touchUpRight = true;
    } else if (id == grCollStatus::Touch_Down_Left) {
        m_collStatus->m_collLineWorkArray[6] = m_touch[6].m_lineWork;
        m_collStatus->m_touchDownLeft = true;
    } else if (id == grCollStatus::Touch_Down_Right) {
        m_collStatus->m_collLineWorkArray[7] = m_touch[7].m_lineWork;
        m_collStatus->m_touchDownRight = true;
    }
}

// The air collision test against the collected lines (side mask m_unkF) for one pass of testAir; returns true when a
// move is to be limited (the shape stopped at a line). Corner hits are retried; when both lower corners are hit the
// shape is narrowed. The lines of the ignore list are given to the status when the ground was not touched.
int grCollisionTest::testCollisionMainAir() {
    m_preIgnoreNum = 0;
    m_unk8 = 1.0f;
    m_unkC_3 = false;
    int result = testCollisionMainAirSub();
    u8 id = m_hit.m_id;
    int remaining = 2;
    while (id != 0xFF) {
        if ((m_touchMask & 0xC0) == 0xC0) {
            // both lower corners: narrow the shape around its center and test again
            Vec2f center = m_unk2bc->getCenterPos();
            Vec2f left = m_unk2bc->getLeftPos();
            Vec2f right = m_unk2bc->getRightPos();
            if (center.m_x - left.m_x > 0.4f) {
                left.m_x = center.m_x + 0.8f * (left.m_x - center.m_x);
            }
            if (right.m_x - center.m_x > 0.4f) {
                right.m_x = center.m_x + 0.8f * (right.m_x - center.m_x);
            }
            m_unk2bc->setLR(center, left, right);
            center = m_unk2c4->getCenterPos();
            m_unk2c0->set(*m_unk2bc);
            m_unk2c0->setCenterPos(center);
            if (testCollisionMainAirSub()) {
                result = 1;
            }
            break;
        }
        if (id > 3) {
            if (testCollisionMainAirSub()) {
                result = 1;
            }
            id = m_hit.m_id;
        }
        if (--remaining <= 0) {
            break;
        }
    }
    if (m_unkC_3) {
        m_unk2c4->set(*m_unk2c0);
    }
    if (m_touch[3].m_id == 0xFF) {
        u16 num = m_preIgnoreNum;
        grCollisionLineWork* lineWork = m_preIgnoreLines;
        for (u32 i = 0; i < num; i++) {
            fn_80137074(m_collStatus, lineWork);
            lineWork++;
        }
        m_preIgnoreNum = 0;
    }
    if (result) {
        if (m_unk8 < 0.01f) {
            m_unk8 = 0.0f;
        }
        if (m_unk8 > 0.0f) {
            updateCurrent(0.99f * m_unk8);
        }
        updateCollStatusTouchInfo();
        if (m_unk8 < 1.0f) {
            result = 1;
        }
    }
    return result;
}

// One sweep of testCollisionNotMove over the collected lines whose attribute is in the side mask m_unkF; returns true
// when at least one line was tested. A through-able line that is the status' reference line (touch line 9) is skipped.
int grCollisionTest::testCollisionMainAirSub() {
    m_unk8 = 1.0f;
    u8 sides = m_unkF;
    u16 mask = 0;
    if (sides & grCollStatus::TOUCH_MASK_UP) {
        mask |= 2;
    }
    if (sides & grCollStatus::TOUCH_MASK_DOWN) {
        mask |= 1;
    }
    if (sides & grCollStatus::TOUCH_MASK_LEFT) {
        mask |= 4;
    }
    if (sides & grCollStatus::TOUCH_MASK_RIGHT) {
        mask |= 8;
    }
    grCollStatus* status = m_collStatus;
    int count = 0;
    for (u32 i = 0; i < m_collection->m_lineWorks.m_num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        if (isActiveLineWork(*lineWork) && (lineWork->m_collisionLine->m_unkE & mask)) {
            if (lineWork->m_collisionLine->m_isThroughable) {
                bool same = false;
                if (lineWork->m_collisionLine == status->m_collLineWorkArray[9].m_collisionLine) {
                    same = true;
                }
                if (same) {
                    continue;
                }
            }
            testCollisionNotMove(lineWork);
            count++;
        }
    }
    return count != 0;
}

// Tests the collected lines like testCollisionMainAirSub, then limits the move to the nearest hit: the shape is advanced by
// the rate done so far and the touched line is published to the status. Returns true when the move was cut short.
int grCollisionTest::testCollisionMain() {
    m_unk8 = 1.0f;
    u8 sides = m_unkF;
    u16 mask = 0;
    if (sides & grCollStatus::TOUCH_MASK_UP) {
        mask |= 2;
    }
    if (sides & grCollStatus::TOUCH_MASK_DOWN) {
        mask |= 1;
    }
    if (sides & grCollStatus::TOUCH_MASK_LEFT) {
        mask |= 4;
    }
    if (sides & grCollStatus::TOUCH_MASK_RIGHT) {
        mask |= 8;
    }
    grCollStatus* status = m_collStatus;
    int count = 0;
    for (u32 i = 0; i < m_collection->m_lineWorks.m_num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        if (isActiveLineWork(*lineWork) && (lineWork->m_collisionLine->m_unkE & mask)) {
            if (lineWork->m_collisionLine->m_isThroughable) {
                bool same = false;
                if (lineWork->m_collisionLine == status->m_collLineWorkArray[9].m_collisionLine) {
                    same = true;
                }
                if (same) {
                    continue;
                }
            }
            testCollisionNotMove(lineWork);
            count++;
        }
    }
    int result = 0;
    if (count != 0) {
        if (m_unk8 < 0.01f) {
            m_unk8 = 0.0f;
        }
        if (m_unk8 > 0.0f) {
            updateCurrent(0.99f * m_unk8);
        }
        updateCollStatusTouchInfo();
        if (m_unk8 < 1.0f) {
            result = 1;
        }
    }
    return result;
}

// The ground collision test against the collected lines (side mask m_unkF): like testCollisionMain, but a wall line whose
// neighbors continue the ground line the shape stands on (m_unk224) is not tested, and the line work flag (m_unkC) records
// why it was skipped (bit 0 up-left corner, bit 1 up-right corner, bit 2 ceiling next to the ground line).
int grCollisionTest::testCollisionMainGround() {
    int count = 0;
    m_unk8 = 1.0f;
    grCollisionLineWork lineD;
    grCollisionLineWork lineC;
    grCollisionLineWork lineB;
    grCollisionLineWork lineA;
    u16 mask = 0;
    u8 sides = m_unkF;
    if (sides & grCollStatus::TOUCH_MASK_UP) {
        mask |= 2;
    }
    if (sides & grCollStatus::TOUCH_MASK_DOWN) {
        mask |= 1;
    }
    if (sides & grCollStatus::TOUCH_MASK_LEFT) {
        mask |= 4;
    }
    if (sides & grCollStatus::TOUCH_MASK_RIGHT) {
        mask |= 8;
    }
    bool hasGround = false;
    bool valid = false;
    if (m_unk224.m_collisionLine != NULL && m_unk224.m_collisionJoint != NULL && m_unk224.m_collision != NULL) {
        valid = true;
    }
    if (valid) {
        hasGround = true;
    }
    for (u32 i = 0; i < m_collection->m_lineWorks.m_num; i++) {
        grCollisionLineWork* lineWork = &m_collection->m_lineWorks.m_works[i];
        if (isActiveLineWork(*lineWork) && (lineWork->m_collisionLine->m_unkE & mask) && lineWork->m_unkC == 0) {
            if (lineWork->m_collisionLine->m_isRight) {
                // a wall whose neighbors both face up (a step) is skipped
                if (lineWork->getPrev(&lineC) && lineC.m_collisionLine->m_isUpper && lineC.getNext(&lineB) &&
                    lineB.m_collisionLine->m_isUpper) {
                    continue;
                }
            } else if (lineWork->m_collisionLine->m_isLeft) {
                if (lineWork->getNext(&lineB) && lineB.m_collisionLine->m_isUpper && lineB.getPrev(&lineC) &&
                    lineC.m_collisionLine->m_isUpper) {
                    continue;
                }
            }
            if (hasGround) {
                grCollisionLine* line = lineWork->m_collisionLine;
                if (line->m_isUnder) {
                    bool same = false;
                    if (m_unk224.m_collisionLine == line) {
                        same = true;
                    }
                    if (same) {
                        continue;
                    }
                    if (m_unk224.getPrev(&lineA)) {
                        bool same2 = false;
                        if (lineA.m_collisionLine == lineWork->m_collisionLine) {
                            same2 = true;
                        }
                        if (same2) {
                            continue;
                        }
                    }
                    if (m_unk224.getNext(&lineA)) {
                        bool same3 = false;
                        if (lineA.m_collisionLine == lineWork->m_collisionLine) {
                            same3 = true;
                        }
                        if (same3) {
                            continue;
                        }
                    }
                }
                u8 dir = m_unkE;
                if ((dir & DIR_RIGHT) && lineWork->m_collisionLine->m_isRight) {
                    lineWork->getUpRightLine(&lineA);
                    lineA.getNext(&lineD);
                    if (isActiveLineWorkIf(lineD) && lineD.m_collisionLine->m_isUnder) {
                        bool same = false;
                        if (lineA.m_collisionLine == m_unk224.m_collisionLine) {
                            same = true;
                        }
                        if (same || m_unk224.isLinkNextSameDirFlag(&lineD)) {
                            lineWork->m_unkC |= 2;
                            continue;
                        }
                    }
                } else if ((dir & DIR_LEFT) && lineWork->m_collisionLine->m_isLeft) {
                    lineWork->getUpLeftLine(&lineA);
                    lineA.getPrev(&lineD);
                    if (isActiveLineWorkIf(lineD) && lineD.m_collisionLine->m_isUnder) {
                        bool same = false;
                        if (lineA.m_collisionLine == m_unk224.m_collisionLine) {
                            same = true;
                        }
                        if (same || m_unk224.isLinkPrevSameDirFlag(&lineD)) {
                            lineWork->m_unkC |= 1;
                            continue;
                        }
                    }
                } else if (lineWork->m_collisionLine->m_isUpper) {
                    if (lineWork->getPrevNoVLink(&lineB) && lineB.m_collisionLine->m_isUnder) {
                        bool same = false;
                        if (lineB.m_collisionLine == m_unk224.m_collisionLine) {
                            same = true;
                        }
                        if (same || m_unk224.isLinkPrevSameDirFlag(&lineB)) {
                            lineWork->m_unkC |= 4;
                            continue;
                        }
                    }
                    if (lineWork->getNextNoVLink(&lineB) && lineB.m_collisionLine->m_isUnder) {
                        bool same = false;
                        if (lineB.m_collisionLine == m_unk224.m_collisionLine) {
                            same = true;
                        }
                        if (same || m_unk224.isLinkNextSameDirFlag(&lineB)) {
                            lineWork->m_unkC |= 4;
                            continue;
                        }
                    }
                    lineWork->getPrev(&lineD);
                    m_unk224.getPrev(&lineB);
                    if (isActiveLineWork(lineD) && lineD.m_collisionLine->m_isUnder && isActiveLineWork(lineB)) {
                        bool same = false;
                        if (lineD.m_collisionLine == lineB.m_collisionLine) {
                            same = true;
                        }
                        if (same) {
                            lineWork->m_unkC |= 4;
                            continue;
                        }
                    }
                    lineWork->getNext(&lineD);
                    m_unk224.getPrev(&lineB);
                    if (isActiveLineWork(lineD) && lineD.m_collisionLine->m_isUnder && isActiveLineWork(lineB)) {
                        bool same = false;
                        if (lineD.m_collisionLine == lineB.m_collisionLine) {
                            same = true;
                        }
                        if (same) {
                            lineWork->m_unkC |= 4;
                            continue;
                        }
                    }
                }
            }
            testCollisionNotMove(lineWork);
            count++;
        }
    }
    int result = 0;
    if (count != 0) {
        if (m_unk8 < 0.01f) {
            m_unk8 = 0.0f;
        }
        if (m_unk8 > 0.0f) {
            updateCurrent(0.99f * m_unk8);
        }
        updateCollStatusTouchInfo();
        if (m_unk8 < 1.0f) {
            result = 1;
        }
    }
    return result;
}

void grCollisionTest::pushbackPreIgnoreLine(grCollisionLineWork* lineWork) {
    if (m_preIgnoreNum >= PRE_IGNORE_MAX) {
        return;
    }
    if (m_preIgnoreNum >= PRE_IGNORE_MAX) {
        return;
    }
    grCollisionLineWork* dst = &m_preIgnoreLines[m_preIgnoreNum];
    *dst = *lineWork;
    m_preIgnoreNum++;
}


// Sweeps the point pt along dir (one move) against the segment seg. Returns 0 when the point misses it; for a segment that is
// not parallel to dir it returns 1 / -1 (the sign of the direction the segment is crossed in) and *outRate is the part of the
// move done at the crossing; for a collinear segment it returns 1 and *outRate is the part done at its nearer end.
int grCollisionTest::testMovePtToSeg(Vec2f* pt, Vec2f* dir, clSegment2D* seg, float* outRate) {
    int result = 0;
    Vec2f a = seg->m_p0Pos;
    Vec2f d = seg->m_p1Pos - a;
    float cross = -dir->m_y * d.m_x + dir->m_x * d.m_y;
    Vec2f n(-dir->m_y, dir->m_x);
    Vec2f m(-d.m_y, d.m_x);
    float absCross = __fabs(cross);
    if (absCross > 0.00001f) {
        Vec2f w = a - *pt;
        float t = -(n.m_x * w.m_x + n.m_y * w.m_y) / cross;
        if (!(t < 0.0f) && !(t > 1.0f)) {
            Vec2f v = *pt - a;
            float denom = dir->m_x * m.m_x + dir->m_y * m.m_y;
            float rate = -(m.m_x * v.m_x + m.m_y * v.m_y) / denom;
            *outRate = rate;
            if (!(rate > 1.0f) && !(rate < 0.0f)) {
                if (denom > 0.00001f) {
                    result = -1;
                } else {
                    result = 1;
                }
            }
        }
    } else {
        n.normalize();
        Vec2f w = a - *pt;
        float absDot = __fabs(n.m_x * w.m_x + n.m_y * w.m_y);
        if (absDot < 0.00001f) {
            float rate0 = seg->m_p0Pos.distance(pt) / dir->length();
            float rate1 = seg->m_p1Pos.distance(pt) / dir->length();
            float rate = rate0 < rate1 ? rate0 : rate1;
            if (!(rate > 1.0f) && !(rate < 0.0f)) {
                *outRate = rate;
                result = 1;
            }
        }
    }
    return result;
}

