#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_linework.h>
#include <gr/collision/gr_collision_manager.h>
#include <types.h>

extern grCollisionManager* lbl_805A0360;
// HYPOTHESIS: grCollision::getPreSegment (the segment of the line before the last vertex update; gr_collision.o).
extern "C" void fn_801119F8(grCollision* collision, clSegment2D* out, grCollisionLine* line);
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

// A line work whose line, joint and collision are all set, whose joint is active and whose collision is enabled.
static inline bool isActiveLineWork(const grCollisionLineWork* lineWork) {
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

void grCollisionLineWork::getPos0(Vec2f* out) const {
    *out = m_collision->m_vtxPositions[(u16)m_collisionLine->m_point0Index * 2];
}

void grCollisionLineWork::getPos1(Vec2f* out) const {
    *out = m_collision->m_vtxPositions[(u16)m_collisionLine->m_point1Index * 2];
}

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

bool grCollisionLineWork::getPrevNoVLink(grCollisionLineWork* out) const {
    bool result = false;
    grCollisionLine* line = fn_80111B94(m_collision, m_collisionLine);
    if (line != NULL) {
        result = true;
        out->m_collisionLine = line;
        out->m_collision = m_collision;
        out->m_collisionJoint = m_collisionJoint;
        out->m_unkC = 0;
        out->m_unkD = false;
    } else {
        out->m_collisionLine = NULL;
        out->m_collisionJoint = NULL;
        out->m_collision = NULL;
        out->m_unkC = 0;
    }
    return result;
}

bool grCollisionLineWork::getNextNoVLink(grCollisionLineWork* out) const {
    bool result = false;
    grCollisionLine* line = fn_80111B70(m_collision, m_collisionLine);
    if (line != NULL) {
        result = true;
        out->m_collisionLine = line;
        out->m_collision = m_collision;
        out->m_collisionJoint = m_collisionJoint;
        out->m_unkC = 0;
        out->m_unkD = false;
    } else {
        out->m_collisionLine = NULL;
        out->m_collisionJoint = NULL;
        out->m_collision = NULL;
        out->m_unkC = 0;
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

// Walks from this line along the left (prev) links, at most once around the joint, and reports whether `target` is met.
bool grCollisionLineWork::isLinkPrev(const grCollisionLineWork* target) const {
    grCollisionLineWork cur = *this;
    grCollisionLineWork next;
    bool result = false;
    if (m_collisionJoint == target->m_collisionJoint) {
        for (int i = m_collisionJoint->m_lineLen; i != 0; i--) {
            if (!cur.getPrev(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            if (valid && m_collisionJoint == next.m_collisionJoint && m_collisionLine != next.m_collisionLine) {
                if (next.m_collisionLine == target->m_collisionLine) {
                    result = true;
                    break;
                }
                cur = next;
            } else {
                break;
            }
        }
    }
    return result;
}

// Same walk along the right (next) links.
bool grCollisionLineWork::isLinkNext(const grCollisionLineWork* target) const {
    grCollisionLineWork cur = *this;
    grCollisionLineWork next;
    bool result = false;
    if (m_collisionJoint == target->m_collisionJoint) {
        for (int i = m_collisionJoint->m_lineLen; i != 0; i--) {
            if (!cur.getNext(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            if (valid && m_collisionJoint == next.m_collisionJoint && m_collisionLine != next.m_collisionLine) {
                if (next.m_collisionLine == target->m_collisionLine) {
                    result = true;
                    break;
                }
                cur = next;
            } else {
                break;
            }
        }
    }
    return result;
}

// Walks at most ten left links, staying on enabled lines that share an attribute bit with `target`.
bool grCollisionLineWork::isLinkPrevSameDirFlag(grCollisionLineWork* target) const {
    bool result = false;
    grCollisionLineWork cur = *this;
    grCollisionLineWork next;
    int i = 10;
    do {
        cur.getPrev(&next);
        bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
        bool active;
        if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        if (!active) {
            break;
        }
        bool sameDir = false;
        grCollisionLine* targetLine = target->m_collisionLine;
        if (((next.m_collisionLine->m_unkE & targetLine->m_unkE) & 0xF) != 0) {
            sameDir = true;
        }
        if (!sameDir) {
            break;
        }
        if (next.m_collisionLine == targetLine) {
            result = true;
            break;
        }
        cur = next;
    } while (--i != 0);
    return result;
}

bool grCollisionLineWork::isLinkNextSameDirFlag(grCollisionLineWork* target) const {
    bool result = false;
    grCollisionLineWork cur = *this;
    grCollisionLineWork next;
    int i = 10;
    do {
        cur.getNext(&next);
        bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
        bool active;
        if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
            active = true;
        } else {
            active = false;
        }
        if (!active) {
            break;
        }
        bool sameDir = false;
        grCollisionLine* targetLine = target->m_collisionLine;
        if (((next.m_collisionLine->m_unkE & targetLine->m_unkE) & 0xF) != 0) {
            sameDir = true;
        }
        if (!sameDir) {
            break;
        }
        if (next.m_collisionLine == targetLine) {
            result = true;
            break;
        }
        cur = next;
    } while (--i != 0);
    return result;
}

bool grCollisionLineWork::isLinkSameDirFlag(grCollisionLineWork* target) const {
    bool result = isLinkPrevSameDirFlag(target);
    if (result != true) {
        result = isLinkNextSameDirFlag(target);
    }
    return result;
}

u32 grCollisionLineWork::getLineId() {
    return fn_80111C70(m_collision, m_collisionLine);
}
// Steps to the neighbouring line at whichever end of this segment is nearer to `pos`.
bool grCollisionLineWork::getNextLineWork(const Vec2f& pos, grCollisionLineWork* out) const {
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    float l0 = (segment.m_p0Pos - pos).lengthSq();
    float l1 = (segment.m_p1Pos - pos).lengthSq();
    bool result;
    if (l0 < l1) {
        result = getPrev(out);
    } else {
        result = getNext(out);
    }
    return result;
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

Vec2f grCollisionLineWork::getNormalVec() {
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    Vec2f dir = segment.m_p1Pos - segment.m_p0Pos;
    return Vec2f(-dir.m_y, dir.m_x);
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


// HYPOTHESIS: finds, on this line's chain, the line whose x range covers `x` (a left-to-right line such as a floor): this line itself when
// it covers it, otherwise the first neighbour (same attribute, enabled collision) that does; false when the chain ends or turns back first.
bool grCollisionLineWork::getPosXDownLine(float x, grCollisionLineWork* out) const {
    clSegment2D segment;
    grCollisionLineWork next;
    m_collision->getSegment(&segment, m_collisionLine);
    if (segment.m_p0Pos.m_x <= x && x <= segment.m_p1Pos.m_x) {
        *out = *this;
        return true;
    }
    *out = *this;
    if (segment.m_p1Pos.m_x < x) {
        for (;;) {
            if (!out->getNextSameAttr(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            next.m_collision->getSegment(&segment, next.m_collisionLine);
            if (x <= segment.m_p1Pos.m_x) {
                *out = next;
                return true;
            }
            if (segment.m_p1Pos.m_x <= segment.m_p0Pos.m_x) {
                break;
            }
            *out = next;
        }
    } else if (x < segment.m_p0Pos.m_x) {
        for (;;) {
            if (!out->getPrevSameAttr(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            next.m_collision->getSegment(&segment, next.m_collisionLine);
            if (segment.m_p0Pos.m_x <= x) {
                *out = next;
                return true;
            }
            if (segment.m_p1Pos.m_x <= segment.m_p0Pos.m_x) {
                break;
            }
            *out = next;
        }
    }
    return false;
}

// Same search as getPosXDownLine, on the segments of the previous frame (getPreSegment).
bool grCollisionLineWork::getPosXDownLinePre(float x, grCollisionLineWork* out) const {
    clSegment2D segment;
    grCollisionLineWork next;
    fn_801119F8(m_collision, &segment, m_collisionLine);
    if (segment.m_p0Pos.m_x <= x && x <= segment.m_p1Pos.m_x) {
        *out = *this;
        return true;
    }
    *out = *this;
    if (segment.m_p1Pos.m_x < x) {
        for (;;) {
            if (!out->getNextSameAttr(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            fn_801119F8(next.m_collision, &segment, next.m_collisionLine);
            if (x <= segment.m_p1Pos.m_x) {
                *out = next;
                return true;
            }
            if (segment.m_p1Pos.m_x <= segment.m_p0Pos.m_x) {
                break;
            }
            *out = next;
        }
    } else if (x < segment.m_p0Pos.m_x) {
        for (;;) {
            if (!out->getPrevSameAttr(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            fn_801119F8(next.m_collision, &segment, next.m_collisionLine);
            if (segment.m_p0Pos.m_x <= x) {
                *out = next;
                return true;
            }
            if (segment.m_p1Pos.m_x <= segment.m_p0Pos.m_x) {
                break;
            }
            *out = next;
        }
    }
    return false;
}

// HYPOTHESIS: the search of getPosXDownLine for a right-to-left line such as a ceiling; neighbours need no shared attribute.
bool grCollisionLineWork::getPosXUpLine(float x, grCollisionLineWork* out) const {
    clSegment2D segment;
    grCollisionLineWork next;
    m_collision->getSegment(&segment, m_collisionLine);
    if (segment.m_p1Pos.m_x <= x && x <= segment.m_p0Pos.m_x) {
        *out = *this;
        return true;
    }
    *out = *this;
    if (segment.m_p0Pos.m_x < x) {
        for (;;) {
            out->getPrev(&next);
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            next.m_collision->getSegment(&segment, next.m_collisionLine);
            if (x <= segment.m_p0Pos.m_x) {
                *out = next;
                return true;
            }
            if (segment.m_p0Pos.m_x <= segment.m_p1Pos.m_x) {
                break;
            }
            *out = next;
        }
    } else if (x < segment.m_p1Pos.m_x) {
        for (;;) {
            out->getNext(&next);
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            next.m_collision->getSegment(&segment, next.m_collisionLine);
            if (segment.m_p1Pos.m_x <= x) {
                *out = next;
                return true;
            }
            if (segment.m_p0Pos.m_x <= segment.m_p1Pos.m_x) {
                break;
            }
            *out = next;
        }
    }
    return false;
}

// HYPOTHESIS: the same search by height `y` along a line that runs downwards (a wall on the left of the fighter).
bool grCollisionLineWork::getPosYLeftLine(float y, grCollisionLineWork* out) const {
    clSegment2D segment;
    grCollisionLineWork next;
    m_collision->getSegment(&segment, m_collisionLine);
    if (segment.m_p0Pos.m_y >= y && y >= segment.m_p1Pos.m_y) {
        *out = *this;
        return true;
    }
    *out = *this;
    if (segment.m_p0Pos.m_y < y) {
        for (;;) {
            if (!out->getNextSameAttr(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            next.m_collision->getSegment(&segment, next.m_collisionLine);
            if (segment.m_p0Pos.m_y >= y) {
                *out = next;
                return true;
            }
            if (segment.m_p0Pos.m_y <= segment.m_p1Pos.m_y) {
                break;
            }
            *out = next;
        }
    } else if (segment.m_p1Pos.m_y > y) {
        for (;;) {
            if (!out->getPrevSameAttr(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            next.m_collision->getSegment(&segment, next.m_collisionLine);
            if (y >= segment.m_p1Pos.m_y) {
                *out = next;
                return true;
            }
            if (segment.m_p0Pos.m_x <= segment.m_p1Pos.m_x) {
                break;
            }
            *out = next;
        }
    }
    return false;
}

// HYPOTHESIS: the same search by height `y` along a line that runs upwards (a wall on the right of the fighter).
bool grCollisionLineWork::getPosYRightLine(float y, grCollisionLineWork* out) const {
    clSegment2D segment;
    grCollisionLineWork next;
    m_collision->getSegment(&segment, m_collisionLine);
    if (segment.m_p0Pos.m_y <= y && y <= segment.m_p1Pos.m_y) {
        *out = *this;
        return true;
    }
    *out = *this;
    if (segment.m_p1Pos.m_y < y) {
        for (;;) {
            if (!out->getNextSameAttr(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            next.m_collision->getSegment(&segment, next.m_collisionLine);
            if (y <= segment.m_p1Pos.m_y) {
                *out = next;
                return true;
            }
            if (segment.m_p1Pos.m_y <= segment.m_p0Pos.m_y) {
                break;
            }
            *out = next;
        }
    } else if (segment.m_p0Pos.m_y > y) {
        for (;;) {
            if (!out->getPrevSameAttr(&next)) {
                break;
            }
            bool valid = next.m_collisionLine != NULL && next.m_collisionJoint != NULL && next.m_collision != NULL;
            bool active;
            if (valid && next.m_collisionJoint->m_0x54_4 && next.m_collision->m_isEnabled) {
                active = true;
            } else {
                active = false;
            }
            if (!active) {
                break;
            }
            next.m_collision->getSegment(&segment, next.m_collisionLine);
            if (segment.m_p0Pos.m_y <= y) {
                *out = next;
                return true;
            }
            if (segment.m_p1Pos.m_x <= segment.m_p0Pos.m_x) {
                break;
            }
            *out = next;
        }
    }
    return false;
}

// Follows the left (prev) links from this line for as long as the neighbour lies in an enabled collision and has its m_isUnder flag
// set; `out` ends on the last such line (this line when the first neighbour fails). Always reports true.
bool grCollisionLineWork::getLeftDownLine(grCollisionLineWork* out) const {
    // HYPOTHESIS: the segment is fetched but not used (the original discards it as well).
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    grCollisionLineWork next;
    *out = *this;
    for (;;) {
        out->getPrev(&next);
        if (isActiveLineWork(&next) && next.m_collisionLine->m_isUnder) {
            *out = next;
        } else {
            break;
        }
    }
    return true;
}


// Follows the right (next) links from this line for as long as the neighbour lies in an enabled collision and has its m_isUnder flag
// set; `out` ends on the last such line (this line when the first neighbour fails). Always reports true.
bool grCollisionLineWork::getRightDownLine(grCollisionLineWork* out) const {
    // HYPOTHESIS: the segment is fetched but not used (the original discards it as well).
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    grCollisionLineWork next;
    *out = *this;
    for (;;) {
        out->getNext(&next);
        if (isActiveLineWork(&next) && next.m_collisionLine->m_isUnder) {
            *out = next;
        } else {
            break;
        }
    }
    return true;
}


// Follows the right (next) links from this line for as long as the neighbour lies in an enabled collision and has its m_isRight flag
// set; `out` ends on the last such line (this line when the first neighbour fails). Always reports true.
bool grCollisionLineWork::getUpRightLine(grCollisionLineWork* out) const {
    // HYPOTHESIS: the segment is fetched but not used (the original discards it as well).
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    grCollisionLineWork next;
    *out = *this;
    for (;;) {
        out->getNext(&next);
        if (isActiveLineWork(&next) && next.m_collisionLine->m_isRight) {
            *out = next;
        } else {
            break;
        }
    }
    return true;
}


// Follows the left (prev) links from this line for as long as the neighbour lies in an enabled collision and has its m_isLeft flag
// set; `out` ends on the last such line (this line when the first neighbour fails). Always reports true.
bool grCollisionLineWork::getUpLeftLine(grCollisionLineWork* out) const {
    // HYPOTHESIS: the segment is fetched but not used (the original discards it as well).
    clSegment2D segment;
    m_collision->getSegment(&segment, m_collisionLine);
    grCollisionLineWork next;
    *out = *this;
    for (;;) {
        out->getPrev(&next);
        if (isActiveLineWork(&next) && next.m_collisionLine->m_isLeft) {
            *out = next;
        } else {
            break;
        }
    }
    return true;
}

