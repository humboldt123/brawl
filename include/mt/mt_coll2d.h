// Local shadow of BrawlHeaders: adds clSegment2D::checkIntersect2 (main.dol 0x80040F74), clTriangle2D,
// and the recovered clRay2D, clLine2DParam, clQuad2D, clRhombus2D and clSegment2D members of mt_coll2d.o.
#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <revolution/GX/GXTypes.h>
#include <types.h>

class clSegment2D;
class clTriangle2D;
class clCircle2D;

class clAABBox2D {
public:
    float m_unk1;
    float m_unk2;
    float m_unk3;
    float m_unk4;

    // HYPOTHESIS: true when other lies inside this box; parameter types from register use.
    bool checkInclude(const clAABBox2D& other) const;
    // True when the point lies inside this box (edges included).
    bool checkInclude(const Vec2f& point) const;
    // HYPOTHESIS: true when the segment touches this box (end points inside, or an edge crossed).
    bool checkIntersect(const clSegment2D& segment) const;
    // True when the two boxes overlap.
    bool checkIntersect1(const clAABBox2D& other) const;
    // True when the circle overlaps this box.
    bool checkIntersect2(const clCircle2D& circle) const;
    // Writes the box enclosing this box and other to out.
    void logicalOR(const clAABBox2D& other, clAABBox2D& out) const;
    // HYPOTHESIS: debug outline draw; width and fill are not used by the shipped build. Parameter types from register use.
    void draw(GXColor color, float width, bool fill) const;
};

class clCircle2D {
public:
    Vec2f m_centerPos;
    float m_radius;

    void getAABBox(clAABBox2D& aabBox) const;
    // HYPOTHESIS: true when the circle touches the segment; parameter types from register use.
    bool checkIntersect(const clSegment2D& segment) const;
    // True when the two circles overlap.
    bool checkIntersect1(const clCircle2D& other) const;
    // True when the circle overlaps the triangle (forwards to clTriangle2D::checkIntersect1).
    bool checkIntersect2(const clTriangle2D& triangle) const;
    // True when the point lies inside the circle.
    bool checkInclude(const Vec2f& point) const;
    // HYPOTHESIS: debug outline draw; width is not used by the shipped build. Parameter types from register use.
    void draw(GXColor color, float width, bool fill) const;
    // Writes this + (target - this) * rate to out (centre and radius).
    void interpolate(const clCircle2D& target, clCircle2D& out, float rate) const;
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
    // Writes this + (target - this) * rate to out (all five points; the box is rebuilt around them).
    void interpolate(const clRhombus2D& target, clRhombus2D& out, float rate) const;
    // HYPOTHESIS: debug outline draw; the parameters are not used by the shipped build. Types from register use.
    void draw(GXColor color, float width, bool fill) const;
};

// HYPOTHESIS: three vertices, from the gfAreaTriangle copies and offsets.
class clTriangle2D {
public:
    Vec2f m_vertices[3];

    // The smallest box around the three vertices.
    void getAABBox(clAABBox2D& aabBox) const;
    // HYPOTHESIS: true when the triangle touches the box (an edge crosses the box); parameter types from register use.
    bool checkIntersect(const clAABBox2D& aabBox) const;
    // HYPOTHESIS: true when the triangle touches the circle (an edge crosses it or its centre is inside).
    bool checkIntersect1(const clCircle2D& circle) const;
    // HYPOTHESIS: true when the two triangles overlap (separating axis test on the edges).
    bool checkIntersect2(const clTriangle2D& other) const;
    // HYPOTHESIS: debug outline draw; the parameters are not used by the shipped build. Types from register use.
    void draw(GXColor color, float width, bool fill) const;
};

class clSegment2D {
public:
    Vec2f m_p0Pos;
    Vec2f m_p1Pos;

    clSegment2D() { }
    // HYPOTHESIS: the rhombus edge segments are copy-constructed from two corners.
    clSegment2D(const Vec2f& p0, const Vec2f& p1) : m_p0Pos(p0), m_p1Pos(p1) { }

    // HYPOTHESIS: static, like checkIntersect2 (callers pass the addresses of two segments); true when the segments cross.
    static bool checkIntersect(const clSegment2D* a, const clSegment2D* b);
    // HYPOTHESIS: t receives the fraction along a of the crossing; collinear overlap leaves it unwritten.
    static int checkIntersect1(clSegment2D* a, clSegment2D* b, f32* t);
    // HYPOTHESIS: intersection point of two segments (written to out); the result tells whether they cross.
    static int checkIntersect2(clSegment2D* a, clSegment2D* b, Vec2f* out);
    // HYPOTHESIS: the point of the segment's line at pos.m_x (projectionX) or pos.m_y (projectionY);
    // false when the segment is parallel to that axis. Parameter types from register use.
    // HYPOTHESIS: fraction of the segment (0 at p0, 1 at p1) at which pos projects onto its line.
    float getProjectionT(const Vec2f& pos) const;
    // HYPOTHESIS: the foot of pos on the line, the fraction limited to [tMin, tMax] (projection) or unlimited
    // with the result telling whether it lies on the segment (projection1).
    void projection(const Vec2f& pos, Vec2f& out, float tMin, float tMax) const;
    bool projection1(const Vec2f& pos, Vec2f& out) const;
    bool projectionX(const Vec2f& pos, Vec2f& out) const;
    bool projectionY(const Vec2f& pos, Vec2f& out) const;
    // HYPOTHESIS: true when pos lies to the left of the direction p0 -> p1.
    bool isLeftArea(const Vec2f& pos) const;
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
    // HYPOTHESIS: t receives the fraction along the ray (hit when 0 <= t <= 1).
    bool checkIntersect(const clSegment2D& segment, f32* t) const;
    // HYPOTHESIS: also writes the unit normal of the segment that faces against the ray.
    bool checkIntersect2(const clSegment2D& segment, f32* t, Vec2f* normal) const;
    void getAABBox(clAABBox2D& aabBox) const;
    void getSegment(clSegment2D& segment) const;
};

// HYPOTHESIS: four corners, from the stores of set.
class clQuad2D {
public:
    Vec2f m_vertices[4];

    // HYPOTHESIS: collision code builds quads uninitialised and fills them with set.
    clQuad2D() { }
    // HYPOTHESIS: the quad spanned by two segments; parameter types from register use.
    clQuad2D(const clSegment2D& a, const clSegment2D& b);
    // The quad between two segments: the end points are ordered so the outline does not cross itself.
    void set(const clSegment2D& a, const clSegment2D& b);
    // HYPOTHESIS: true when pos lies on the inner side of all four edges.
    bool checkInclude(const Vec2f& pos) const;
    // HYPOTHESIS: true when the segment overlaps the quad (separating axis test on the edges and on the segment).
    bool checkIntersect(const clSegment2D& segment) const;
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

// HYPOTHESIS: a 3D line (position and direction), from the layout read by checkIntersectXYPlane.
class clLine3D {
public:
    Vec3f m_pos;
    Vec3f m_vec;

    // HYPOTHESIS: out receives the point where the line crosses the z = 0 plane; false when parallel to it.
    bool checkIntersectXYPlane(Vec3f& out) const;
};
