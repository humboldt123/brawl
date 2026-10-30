#pragma once

// Local header (BrawlHeaders has no grCollisionRaycheck): casts a ray against the lines of the enabled
// collisions (original object gr_collision_raycheck.o). Only the members its constructor sets are named.

#include <gr/collision/gr_collision_linework.h>
#include <mt/mt_vector.h>
#include <mt/mt_coll2d.h>
#include <types.h>

class grCollision;

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
    Vec2f m_hitNormal; // HYPOTHESIS: unit normal of the nearest line hit so far
    // 0x20
    Vec2f m_hitPos; // HYPOTHESIS: point of the nearest hit (ray start + ray vector * m_unk14)
    // 0x28
    u8 m_hit; // HYPOTHESIS: set once a line was hit
    // 0x2C
    Vec2f m_rayPos; // HYPOTHESIS: a copy of the ray (start, then vector), laid out like clRay2D
    // 0x34
    Vec2f m_rayVec;
    // 0x3C
    u32 m_unk3C; // HYPOTHESIS: line id of the nearest hit (0xFFFF when none)
    // 0x40
    u8 m_unk40;
    // 0x41
    u8 m_unk41;

    grCollisionRaycheck();
    ~grCollisionRaycheck();

    // HYPOTHESIS: signatures of the query members; the map has no parameter types.
    u8 raycheck(clRay2D* ray); // HYPOTHESIS: returns the hit flag as a byte
    u8 raycheck1(clRay2D* ray, grCollision* collision);
    void raycheckSub(grCollision* collision);
};
