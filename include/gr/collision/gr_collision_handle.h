#pragma once

// Local shadow of BrawlHeaders/gr/collision/gr_collision_handle.h: adds the inline default constructor that the CPU AI's
// tag_AiClJointList constructor expands (no collision yet, the id invalid).

#include <types.h>

class grCollision;

class grCollisionHandle {
    u16* m_idPtr;
    u16 m_id;
    static const u32 InvalidID = 0x0000FFFFu;
    // TODO more fields?
public:
    grCollisionHandle() {
        m_id = InvalidID;
        m_idPtr = NULL;
    }
    grCollisionHandle(u16* idPtr);
    bool isValid();
    // HYPOTHESIS: the handle points at grCollision::m_index, the first member, so it gives the collision itself (the CPU AI
    // reads the handle like this after isValid()).
    grCollision* getCollision() const { return reinterpret_cast<grCollision*>(m_idPtr); }
};
// TODO size assertion
