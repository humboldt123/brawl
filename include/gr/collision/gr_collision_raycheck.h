#pragma once

// Local header (BrawlHeaders has no grCollisionRaycheck): casts a ray against the lines of the enabled
// collisions (original object gr_collision_raycheck.o). Only the members its constructor sets are named.

#include <gr/collision/gr_collision_linework.h>
#include <mt/mt_coll2d.h>
#include <types.h>

class grCollisionRaycheck {
public:
    // 0x0
    clAABBox2D m_aabb; // HYPOTHESIS: box of the current ray (filled by clRay2D::getAABBox)
    // 0x10
    u16 m_unk10;
    // 0x12
    u8 m_unk12;
    // 0x14
    float m_unk14; // HYPOTHESIS: nearest hit rate along the ray
    // 0x18
    char _spacer18[0x3C - 0x18];
    // 0x3C
    u32 m_unk3C;
    // 0x40
    u8 m_unk40;
    // 0x41
    u8 m_unk41;

    grCollisionRaycheck();
    ~grCollisionRaycheck();
};
