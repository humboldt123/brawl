#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <types.h>

u8 grCollisionJoint::getCollisionId() {
    return m_collision->m_index;
}
