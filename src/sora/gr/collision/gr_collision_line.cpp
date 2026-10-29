#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_linework.h>
#include <gr/collision/gr_collision_manager.h>
#include <types.h>

extern grCollisionManager* lbl_805A0360;
// HYPOTHESIS: grCollision::getLine(u16) (gr_collision.o).
extern "C" grCollisionLine* fn_80111B60(grCollision* collision, int index);
// HYPOTHESIS: grCollision::getRightLine (gr_collision.o).
extern "C" grCollisionLine* fn_80111B70(grCollision* collision, grCollisionLine* line);
// HYPOTHESIS: grCollision::getLeftLine (gr_collision.o).
extern "C" grCollisionLine* fn_80111B94(grCollision* collision, grCollisionLine* line);
// HYPOTHESIS: grCollision::getJointByLineId (gr_collision.o).
extern "C" grCollisionJoint* fn_80111BF0(grCollision* collision, int lineId);
// HYPOTHESIS: grCollision::getLineId (gr_collision.o).
extern "C" u32 fn_80111C70(grCollision* collision, grCollisionLine* line);
// HYPOTHESIS: grCollisionLineWork::isLinkPrevSameDirFlag (walks at most ten left links of the same attribute).
extern "C" bool fn_80114670(const grCollisionLineWork* work, grCollisionLineWork* target);
// HYPOTHESIS: grCollisionLineWork::isLinkNextSameDirFlag (walks at most ten right links of the same attribute).
extern "C" bool fn_80114868(const grCollisionLineWork* work, grCollisionLineWork* target);

bool grCollisionLineWork::getPrevVirtual(grCollisionLineWork* out) const {
    s16 lineId = m_collisionLine->m_unkA;
    bool result = false;
    if (lineId != -1) {
        grCollision* collision = lbl_805A0360->getCollision(m_collisionLine->m_unk8);
        if (collision != NULL) {
            grCollisionJoint* joint = fn_80111BF0(collision, lineId);
            if (joint != NULL) {
                out->m_collisionLine = fn_80111B60(collision, lineId);
                out->m_collisionJoint = joint;
                out->m_collision = collision;
                bool valid = out->m_collisionLine != NULL && out->m_collisionJoint != NULL && out->m_collision != NULL;
                bool enabled;
                if (valid && out->m_collisionJoint->m_0x54_4 && out->m_collision->m_isEnabled) {
                    enabled = true;
                } else {
                    enabled = false;
                }
                if (enabled) {
                    result = true;
                }
            }
        }
    }
    return result;
}

bool grCollisionLineWork::getNextVirtual(grCollisionLineWork* out) const {
    s16 lineId = m_collisionLine->m_unkC;
    bool result = false;
    if (lineId != -1) {
        grCollision* collision = lbl_805A0360->getCollision(m_collisionLine->m_unk9);
        if (collision != NULL) {
            grCollisionJoint* joint = fn_80111BF0(collision, lineId);
            if (joint != NULL) {
                out->m_collisionLine = fn_80111B60(collision, lineId);
                out->m_collisionJoint = joint;
                out->m_collision = collision;
                bool valid = out->m_collisionLine != NULL && out->m_collisionJoint != NULL && out->m_collision != NULL;
                bool enabled;
                if (valid && out->m_collisionJoint->m_0x54_4 && out->m_collision->m_isEnabled) {
                    enabled = true;
                } else {
                    enabled = false;
                }
                if (enabled) {
                    result = true;
                }
            }
        }
    }
    return result;
}

bool grCollisionLineWork::getNext(grCollisionLineWork* out) const {
    bool result = false;
    if (m_collisionLine->m_unkC >= 0) {
        result = getNextVirtual(out);
    }
    if (!result) {
        grCollisionLine* line = fn_80111B70(m_collision, m_collisionLine);
        if (line != NULL) {
            out->m_collisionLine = line;
            out->m_collisionJoint = m_collisionJoint;
            out->m_collision = m_collision;
            result = true;
        } else {
            *out = grCollisionLineWork();
        }
    }
    return result;
}

bool grCollisionLineWork::getPrev(grCollisionLineWork* out) const {
    bool result = false;
    if (m_collisionLine->m_unkA >= 0) {
        result = getPrevVirtual(out);
    }
    if (!result) {
        grCollisionLine* line = fn_80111B94(m_collision, m_collisionLine);
        if (line != NULL) {
            out->m_collisionLine = line;
            out->m_collisionJoint = m_collisionJoint;
            out->m_collision = m_collision;
            result = true;
        } else {
            *out = grCollisionLineWork();
        }
    }
    return result;
}

bool grCollisionLineWork::getPrevSameAttr(grCollisionLineWork* out) const {
    bool result = false;
    if (getPrev(out) && (m_collisionLine->m_unkE_0 & out->m_collisionLine->m_unkE_0)) {
        result = true;
    }
    return result;
}

bool grCollisionLineWork::getNextSameAttr(grCollisionLineWork* out) const {
    bool result = false;
    if (getNext(out) && (m_collisionLine->m_unkE_0 & out->m_collisionLine->m_unkE_0)) {
        result = true;
    }
    return result;
}

bool grCollisionLineWork::isLinkSameDirFlag(grCollisionLineWork* target) const {
    bool result = fn_80114670(this, target);
    if (result != true) {
        result = fn_80114868(this, target);
    }
    return result;
}

u32 grCollisionLineWork::getLineId() {
    return fn_80111C70(m_collision, m_collisionLine);
}
Vec2f grCollisionLineWork::getNearSegmetPos(const Vec2f& pos) const {
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    float l0 = (segment.m_p0Pos - pos).lengthSq();
    float l1 = (segment.m_p1Pos - pos).lengthSq();
    if (l0 < l1) {
        return segment.m_p0Pos;
    }
    return segment.m_p1Pos;
}

Vec2f grCollisionLineWork::getFarSegmetPos(const Vec2f& pos) const {
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    float l0 = (segment.m_p0Pos - pos).lengthSq();
    float l1 = (segment.m_p1Pos - pos).lengthSq();
    if (l0 > l1) {
        return segment.m_p0Pos;
    }
    return segment.m_p1Pos;
}

int grCollisionLineWork::getFarSegmetIndex(const Vec2f& pos) const {
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    float l0 = (segment.m_p0Pos - pos).lengthSq();
    float l1 = (segment.m_p1Pos - pos).lengthSq();
    return !(l0 > l1);
}

int grCollisionLineWork::getNearSegmetIndex(const Vec2f& pos) const {
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    float l0 = (segment.m_p0Pos - pos).lengthSq();
    float l1 = (segment.m_p1Pos - pos).lengthSq();
    return !(l0 < l1);
}

