#pragma once

// Local shadow of BrawlHeaders/gr/collision/gr_collision_linework.h: the zero-initialising constructor, the byte at +0xC / flag bit at +0xD,
// and the line-work queries that soGroundModuleImpl calls (names from the symbol map; HYPOTHESIS signatures).
#include <StaticAssert.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_line.h>
#include <mt/mt_vector.h>
#include <types.h>

class grCollision;

class grCollisionLineWork {
public:
    // 0
    grCollisionLine* m_collisionLine;
    // 4
    grCollisionJoint* m_collisionJoint;
    // 8
    grCollision* m_collision;
    // 12
    u8 m_unkC;
    // 13
    bool m_unkD : 1;

    grCollisionLineWork() {
        m_collisionLine = NULL;
        m_collisionJoint = NULL;
        m_collision = NULL;
        m_unkC = 0;
        m_unkD = false;
    }

    // HYPOTHESIS: the line work of one line (grCollision::getLineWork builds it with this).
    grCollisionLineWork(grCollisionLine* line, grCollisionJoint* joint, grCollision* collision) {
        m_collisionLine = line;
        m_collisionJoint = joint;
        m_collision = collision;
        m_unkC = 0;
        m_unkD = false;
    }

    // HYPOTHESIS signatures for the members below: the map has no parameter types.
    void getPos0(Vec2f* out) const;
    void getPos1(Vec2f* out) const;
    bool getPrevVirtual(grCollisionLineWork* out) const;
    bool getNextVirtual(grCollisionLineWork* out) const;
    bool getNext(grCollisionLineWork* out) const;
    bool getPrev(grCollisionLineWork* out) const;
    bool getPrevNoVLink(grCollisionLineWork* out) const;
    bool getNextNoVLink(grCollisionLineWork* out) const;
    bool getPrevSameAttr(grCollisionLineWork* out) const;
    bool getNextSameAttr(grCollisionLineWork* out) const;
    bool isLinkPrev(const grCollisionLineWork* target) const;
    bool isLinkNext(const grCollisionLineWork* target) const;
    bool isLinkPrevSameDirFlag(grCollisionLineWork* target) const;
    bool isLinkNextSameDirFlag(grCollisionLineWork* target) const;
    bool isLinkSameDirFlag(grCollisionLineWork* target) const;
    u32 getLineId();
    bool getNextLineWork(const Vec2f& pos, grCollisionLineWork* out) const;
    Vec2f getNearSegmetPos(const Vec2f& pos) const;
    Vec2f getFarSegmetPos(const Vec2f& pos) const;
    Vec2f getNormalVec();
    bool getPosXDownLine(float x, grCollisionLineWork* out) const;
    bool getPosXDownLinePre(float x, grCollisionLineWork* out) const;
    bool getPosXUpLine(float x, grCollisionLineWork* out) const;
    bool getPosYLeftLine(float y, grCollisionLineWork* out) const;
    bool getPosYRightLine(float y, grCollisionLineWork* out) const;
    bool getLeftDownLine(grCollisionLineWork* out) const;
    bool getRightDownLine(grCollisionLineWork* out) const;
    bool getUpRightLine(grCollisionLineWork* out) const;
    bool getUpLeftLine(grCollisionLineWork* out) const;
    int getFarSegmetIndex(const Vec2f& pos) const;
    int getNearSegmetIndex(const Vec2f& pos) const;
};
static_assert(sizeof(grCollisionLineWork) == 16, "Class is wrong size!");
