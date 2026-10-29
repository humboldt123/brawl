// Local shadow of BrawlHeaders: adds clSegment2D::checkIntersect2 (main.dol 0x80040F74), clTriangle2D,
// and the recovered clRay2D, clLine2DParam, clQuad2D, clRhombus2D and clSegment2D members of mt_coll2d.o.
#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <revolution/GX/GXTypes.h>
#include <types.h>

class clSegment2D;

class clAABBox2D {
public:
    float m_unk1;
    float m_unk2;
    float m_unk3;
    float m_unk4;

    // HYPOTHESIS: true when other lies inside this box; parameter types from register use.
    bool checkInclude(const clAABBox2D& other) const;
};

class clCircle2D {
public:
    Vec2f m_centerPos;
    float m_radius;

    void getAABBox(clAABBox2D& aabBox) const;
};

class clRhombus2D {
public:
    Vec2f m_centerPos;
    Vec2f m_upPos;
    Vec2f m_downPos;
    Vec2f m_leftPos;
    Vec2f m_rightPos;
    clAABBox2D m_aabBox;

    clRhombus2D() { }
    // HYPOTHESIS: parameter names and order from the stores of the constructor.
    clRhombus2D(float top, float bottom, float left, float right, float x, float y);
    void move(const Vec2f& displacement);
    // HYPOTHESIS: one bit per rhombus edge the segment crosses (up-right, down-right, down-left, up-left).
    u32 getIntersectInfo(const clSegment2D& segment) const;
};

// HYPOTHESIS: three vertices, from the gfAreaTriangle copies and offsets.
class clTriangle2D {
public:
    Vec2f m_vertices[3];
};

class clSegment2D {
public:
    Vec2f m_p0Pos;
    Vec2f m_p1Pos;

    clSegment2D() { }
    // HYPOTHESIS: the rhombus edge segments are copy-constructed from two corners.
    clSegment2D(const Vec2f& p0, const Vec2f& p1) : m_p0Pos(p0), m_p1Pos(p1) { }

    // HYPOTHESIS: intersection point of two segments (written to out); the result tells whether they cross.
    static int checkIntersect2(clSegment2D* a, clSegment2D* b, Vec2f* out);
    // HYPOTHESIS: the point of the segment's line at pos.m_x (projectionX) or pos.m_y (projectionY);
    // false when the segment is parallel to that axis. Parameter types from register use.
    bool projectionX(const Vec2f& pos, Vec2f& out) const;
    bool projectionY(const Vec2f& pos, Vec2f& out) const;
    // HYPOTHESIS: debug line draw; zEnable picks the depth test. Parameter types from register use.
    void draw(GXColor color, float width, bool zEnable) const;
};

// The base of clRay2D: the map names the weak constructor that the enemy area module calls to build a ray
// clLine2DBase::__ct.
// HYPOTHESIS: start position and direction vector, from getSegment and getAABBox.
class clLine2DBase {
public:
    Vec2f m_pos;
    Vec2f m_vec;

    // MATCH-ONLY: never_inline, the area module calls the weak copy out of line from the inlined clRay2D constructor.
    clLine2DBase(const Vec2f& pos, const Vec2f& vec) __attribute__((never_inline)) : m_pos(pos), m_vec(vec) { }
};

class clRay2D : public clLine2DBase {
public:
    clRay2D(const Vec2f& pos, const Vec2f& vec) : clLine2DBase(pos, vec) { }

    // HYPOTHESIS: true when the ray crosses the segment; parameter types from register use.
    bool checkIntersect(const clSegment2D& segment) const;
    void getSegment(clSegment2D& segment) const;
};

// HYPOTHESIS: four corners, from the stores of set.
class clQuad2D {
public:
    Vec2f m_vertices[4];

    // HYPOTHESIS: the quad spanned by two segments; parameter types from register use.
    clQuad2D(const clSegment2D& a, const clSegment2D& b);
};

// HYPOTHESIS: line equation normal.x * x + normal.y * y + distance = 0.
class clLine2DParam {
public:
    Vec2f m_normal;
    float m_distance;

    // HYPOTHESIS: flip selects the normal on the other side of the ray.
    clLine2DParam(const clRay2D& ray, bool flip);
    // The signed distance of a point to the line.
    float distance(const Vec2f& point) const { return m_normal.m_x * point.m_x + m_normal.m_y * point.m_y + m_distance; }
};
