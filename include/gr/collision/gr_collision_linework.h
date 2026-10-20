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

    u32 getLineId();
    bool getNextSameAttr(grCollisionLineWork* out);
    bool getPrevSameAttr(grCollisionLineWork* out);
    Vec2f getNormalVec();
};
static_assert(sizeof(grCollisionLineWork) == 16, "Class is wrong size!");
