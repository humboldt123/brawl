// MATCH-ONLY: this unit copies whole vectors with the aggregate word copy.
#define MT_VEC2F_PLAIN_BASE
#define MT_VEC3F_MUL_INLINE
#include <gf/gf_camera.h>
#include <gf/gf_draw.h>
#include <mt/mt_coll2d.h>
#include <revolution/GX.h>
#include <types.h>
#include <mt/mt_common.h>

// ===== part A =====

// HYPOTHESIS: unnamed mt_coll2d entry points (map: clRay2D/clSegment2D checkIntersect, clQuad2D set); parameter types from register use.

// Geometric helpers of the 2D library (inlined by the compiler into every user below).
// The dot product of two vectors.
static inline float vecDot(const Vec2f& a, const Vec2f& b) {
    return a.m_x * b.m_x + a.m_y * b.m_y;
}

// The normal of a segment (its direction turned 90 degrees to the left).
static inline Vec2f segNormal(const clSegment2D& segment) {
    Vec2f res;
    res.m_x = -(segment.m_p1Pos - segment.m_p0Pos).m_y;
    res.m_y = (segment.m_p1Pos - segment.m_p0Pos).m_x;
    return res;
}

// The vector pointing the other way.
static inline Vec2f vecNeg(const Vec2f& v) {
    Vec2f res;
    res.m_x = -v.m_x;
    res.m_y = -v.m_y;
    return res;
}

clLine2DParam::clLine2DParam(const clRay2D& ray, bool flip) {
    Vec2f normal;
    if (!flip) {
        normal.m_y = ray.m_vec.m_x;
        normal.m_x = -ray.m_vec.m_y;
    } else {
        normal.m_x = ray.m_vec.m_y;
        normal.m_y = -ray.m_vec.m_x;
    }
    normal.normalize();
    float dot = normal.m_x * ray.m_pos.m_x + normal.m_y * ray.m_pos.m_y;
    m_normal = normal;
    m_distance = -dot;
}

// Ray (pos + t * vec) against a segment; t receives the parameter of the crossing along the ray (a hit needs 0 <= t <= 1).
bool clRay2D::checkIntersect(const clSegment2D& segment, f32* t) const {
    bool hit = false;
    Vec2f a;
    Vec2f b;
    Vec2f n;
    a = segment.m_p0Pos - m_pos;
    b = segment.m_p1Pos - m_pos;
    // the normal of the ray; both segment ends on the same side of the ray's line: no crossing
    n.m_x = -m_vec.m_y;
    n.m_y = m_vec.m_x;
    if (!(vecDot(n, a) * vecDot(n, b) > 0.0f)) {
        // the normal of the segment
        Vec2f m;
        m.m_x = -(segment.m_p1Pos - segment.m_p0Pos).m_y;
        m.m_y = (segment.m_p1Pos - segment.m_p0Pos).m_x;
        float denom = vecDot(m, m_vec);
        Vec2f w = vecNeg(a);
        if ((float)__fabs(denom) < 1.1920929e-7f) {
            // parallel: hit when the segment lies in front of the ray start
            if (!(vecDot(w, m_vec) < 0.0f)) {
                if (!(vecDot(segment.m_p1Pos - m_pos, m_vec) < 0.0f)) {
                    hit = true;
                }
            }
        } else {
            float s = -vecDot(w, m) / denom;
            *t = s;
            if (!(s < 0.0f) && !(s > 1.0f)) {
                hit = true;
            }
        }
    }
    return hit;
}

bool clRay2D::checkIntersect(const clSegment2D& segment) const {
    f32 t;
    return checkIntersect(segment, &t);
}

// Like checkIntersect; on a hit normal receives the unit normal of the segment, facing against the ray.
bool clRay2D::checkIntersect2(const clSegment2D& segment, f32* t, Vec2f* normal) const {
    bool hit = checkIntersect(segment, t);
    if (hit) {
        *normal = segNormal(segment);
        if (vecDot(*normal, m_vec) > 0.0f) {
            *normal = vecNeg(*normal);
        }
        normal->normalize();
    }
    return hit;
}

// MATCH-ONLY: the original takes the componentwise minimum and maximum of two floats with a paired-single
// subtract and select (the compiler schedules the four subtracts and selects of getAABBox together).
static inline float psMin(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, b, a
    }
    // clang-format on
    return result;
}

static inline float psMax(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, a, b
    }
    // clang-format on
    return result;
}

// The smallest box that contains two points.
static inline void boxOf(const Vec2f& a, const Vec2f& b, clAABBox2D& box) {
    box.m_unk1 = psMin(a.m_x, b.m_x);
    box.m_unk2 = psMin(a.m_y, b.m_y);
    box.m_unk3 = psMax(a.m_x, b.m_x);
    box.m_unk4 = psMax(a.m_y, b.m_y);
}

// The smallest box that contains the ray's start and end.
void clRay2D::getAABBox(clAABBox2D& aabBox) const {
    boxOf(m_pos, m_pos + m_vec, aabBox);
}

void clRay2D::getSegment(clSegment2D& segment) const {
    segment.m_p0Pos = m_pos;
    segment.m_p1Pos = m_pos + m_vec;
}

// The position of pos projected onto the segment's line, as a fraction of the segment (0 at p0, 1 at p1).
float clSegment2D::getProjectionT(const Vec2f& pos) const {
    Vec2f v;
    Vec2f d;
    v = pos - m_p0Pos;
    d = m_p1Pos - m_p0Pos;
    return vecDot(v, d) / vecDot(d, d);
}

// The point of the segment's line nearest to pos, with the fraction limited to [tMin, tMax].
void clSegment2D::projection(const Vec2f& pos, Vec2f& out, float tMin, float tMax) const {
    Vec2f v;
    Vec2f d;
    v = pos - m_p0Pos;
    d = m_p1Pos - m_p0Pos;
    float t = vecDot(v, d) / vecDot(d, d);
    if (t > tMax) {
        t = tMax;
    }
    if (t < tMin) {
        t = tMin;
    }
    out = m_p0Pos + d * t;
}

// The nearest point of the segment's line to pos; true when it lies on the segment (0 <= t <= 1).
bool clSegment2D::projection1(const Vec2f& pos, Vec2f& out) const {
    bool onSegment = false;
    Vec2f v;
    Vec2f d;
    v = pos - m_p0Pos;
    d = m_p1Pos - m_p0Pos;
    float t = vecDot(v, d) / vecDot(d, d);
    out = m_p0Pos + d * t;
    if (!(t > 1.0f) && !(t < 0.0f)) {
        onSegment = true;
    }
    return onSegment;
}

bool clSegment2D::projectionX(const Vec2f& pos, Vec2f& out) const {
    Vec2f d = m_p1Pos - m_p0Pos;
    float dx = d.m_x;
    if ((float)__fabs(dx) > 1.1920929e-7f) {
        out.m_x = pos.m_x;
        out.m_y = m_p0Pos.m_y + d.m_y * ((pos.m_x - m_p0Pos.m_x) / dx);
        return true;
    }
    return false;
}

bool clSegment2D::projectionY(const Vec2f& pos, Vec2f& out) const {
    Vec2f d = m_p1Pos - m_p0Pos;
    float dy = d.m_y;
    if ((float)__fabs(dy) > 1.1920929e-7f) {
        out.m_y = pos.m_y;
        out.m_x = m_p0Pos.m_x + d.m_x * ((pos.m_y - m_p0Pos.m_y) / dy);
        return true;
    }
    return false;
}

// True when the two segments cross.
bool clSegment2D::checkIntersect(const clSegment2D* seg, const clSegment2D* other) {
    bool hit = false;
    Vec2f a;
    Vec2f b;
    Vec2f n;
    a = other->m_p0Pos - seg->m_p0Pos;
    b = other->m_p1Pos - seg->m_p0Pos;
    n.m_x = -(seg->m_p1Pos - seg->m_p0Pos).m_y;
    n.m_y = (seg->m_p1Pos - seg->m_p0Pos).m_x;
    // the ends of the other segment lie on opposite sides (or on the line) of this one ...
    if (!(vecDot(n, a) * vecDot(n, b) > 0.0f)) {
        Vec2f m;
        m.m_x = -(other->m_p1Pos - other->m_p0Pos).m_y;
        m.m_y = (other->m_p1Pos - other->m_p0Pos).m_x;
        Vec2f c = vecNeg(a);
        Vec2f f;
        f = seg->m_p1Pos - other->m_p0Pos;
        // ... and the ends of this one lie on opposite sides of the other
        if (!(vecDot(m, c) * vecDot(m, f) > 0.0f)) {
            hit = true;
        }
    }
    return hit;
}

// Crossing of two segments; t receives the fraction along the first one. Collinear segments hit when an end of
// the other one lies inside the first one (t is not written then).
int clSegment2D::checkIntersect1(clSegment2D* seg, clSegment2D* other, f32* t) {
    int hit = 0;
    Vec2f a;
    Vec2f b;
    Vec2f n;
    a = other->m_p0Pos - seg->m_p0Pos;
    b = other->m_p1Pos - seg->m_p0Pos;
    n.m_x = -(seg->m_p1Pos - seg->m_p0Pos).m_y;
    n.m_y = (seg->m_p1Pos - seg->m_p0Pos).m_x;
    float na = vecDot(n, a);
    float nb = vecDot(n, b);
    if (!(na * nb > 0.0f)) {
        Vec2f d = seg->m_p1Pos - seg->m_p0Pos;
        if ((float)__fabs(na) < 1.1920929e-7f && (float)__fabs(nb) < 1.1920929e-7f) {
            // collinear
            hit = 1;
            float len = vecDot(d, d);
            float pa = vecDot(d, a);
            if (pa > 0.0f && pa < len) {
                return hit;
            }
            float pb = vecDot(d, other->m_p1Pos - seg->m_p0Pos);
            if (pb > 0.0f && pb < len) {
                return hit;
            }
            hit = 0;
        } else {
            Vec2f e = other->m_p1Pos - other->m_p0Pos;
            float u = -vecDot(n, other->m_p0Pos - seg->m_p0Pos) / vecDot(n, e);
            if (!(u < 0.0f) && !(u > 1.0f)) {
                Vec2f m;
                m.m_x = -e.m_y;
                m.m_y = e.m_x;
                Vec2f c = seg->m_p0Pos - other->m_p0Pos;
                float s = -vecDot(m, c) / vecDot(m, d);
                *t = s;
                if (!(s < 0.0f) && !(s > 1.0f)) {
                    hit = 1;
                }
            }
        }
    }
    return hit;
}

int clSegment2D::checkIntersect2(clSegment2D* a, clSegment2D* b, Vec2f* out) {
    f32 t;
    int hit = checkIntersect1(a, b, &t);

    if (hit != 0) {
        *out = a->m_p0Pos + (a->m_p1Pos - a->m_p0Pos) * t;
    }
    return hit;
}

// True when pos lies on the left side of the segment (looking from p0 to p1).
bool clSegment2D::isLeftArea(const Vec2f& pos) const {
    Vec2f n;
    n.m_x = -(m_p1Pos - m_p0Pos).m_y;
    n.m_y = (m_p1Pos - m_p0Pos).m_x;
    return vecDot(n, pos - m_p0Pos) > 0.00001f;
}

void clSegment2D::draw(GXColor color, float width, bool zEnable) const {
    gfCameraManager::getManager()->m_cameras[0].setGX();
    u32 rgba = *reinterpret_cast<u32*>(&color);
    gfDrawSetVtxPosColorPrimEnvironment();
    if (zEnable) {
        GXSetZMode(true, GX_LEQUAL, false);
    } else {
        GXSetZMode(false, GX_LEQUAL, false);
    }
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetLineWidth(6.0f * width, 2);
    GXBegin(GX_LINES, GX_VTXFMT1, 2);
    GXPosition3f32(m_p0Pos.m_x, m_p0Pos.m_y, 0.0f);
    GXColor1u32(rgba);
    GXPosition3f32(m_p1Pos.m_x, m_p1Pos.m_y, 0.0f);
    GXColor1u32(rgba);
    GXEnd();
}



// ===== part B =====

// HYPOTHESIS: clTriangle2D::checkIntersect1 (mt_coll2d.o, main.dol 0x8004231C): true when the circle overlaps the triangle.
// HYPOTHESIS: clSegment2D::checkIntersect (mt_coll2d.o, main.dol 0x800409D4): true when the two segments cross.

// MATCH-ONLY: the original takes the componentwise minimum and maximum of the two segment end points with paired singles.
static inline void segmentBounds(register const clSegment2D* segment, register Vec2f* lo, register Vec2f* hi) {
    register float p0, p1, diff, minPair, maxPair;
    // clang-format off
    asm {
        psq_l p0, 0(segment), 0, 0
        psq_l p1, 8(segment), 0, 0
        ps_sub diff, p0, p1
        ps_sel minPair, diff, p1, p0
        ps_sel maxPair, diff, p0, p1
        psq_st minPair, 0(lo), 0, 0
        psq_st maxPair, 0(hi), 0, 0
    }
    // clang-format on
}

// The segment (p0, p1) as the box spanned by its end points.
static inline void segmentBox(register const clSegment2D* segment, register clAABBox2D* box) {
    register float p0, p1, diffMin, diffMax, minPair, maxPair;
    // clang-format off
    asm {
        psq_l p0, 0(segment), 0, 0
        psq_l p1, 8(segment), 0, 0
        ps_sub diffMin, p0, p1
        ps_sub diffMax, p0, p1
        ps_sel minPair, diffMin, p1, p0
        ps_sel maxPair, diffMax, p0, p1
        psq_st minPair, 0(box), 0, 0
        psq_st maxPair, 8(box), 0, 0
    }
    // clang-format on
}

// True when the circle touches the segment: the circle's box overlaps the segment's box, the segment's line
// passes within the radius of the centre, and the line crosses the circle between p0 and p1.
bool clCircle2D::checkIntersect(const clSegment2D& segment) const {
    bool result = false;
    Vec2f lo;
    Vec2f hi;
    segmentBounds(&segment, &lo, &hi);

    Vec2f center(m_centerPos.m_x, m_centerPos.m_y);
    bool overlap = false;
    if (!(center.m_x + m_radius < lo.m_x) && !(center.m_x - m_radius > hi.m_x) && !(center.m_y + m_radius < lo.m_y) &&
        !(center.m_y - m_radius > hi.m_y)) {
        overlap = true;
    }
    if (overlap) {
        Vec2f dir = Vec2f(segment.m_p1Pos.m_x - segment.m_p0Pos.m_x, segment.m_p1Pos.m_y - segment.m_p0Pos.m_y);
        Vec2f normal(-dir.m_y, dir.m_x);
        normal.normalize();
        center -= segment.m_p0Pos;

        // distance of the centre from the segment's line
        float dist = normal.m_x * center.m_x + normal.m_y * center.m_y;
        double wide = __fabs(dist);
        float absDist = wide;
        if (!(absDist > m_radius)) {
            // |p0 + t * dir - centre|^2 = r^2 is a * t^2 + b * t + c = 0
            float a = dir.lengthSq();
            float b = -2.0f * (center.m_x * dir.m_x + center.m_y * dir.m_y);
            float c = center.lengthSq() - m_radius * m_radius;
            float discriminant = b * b - 4.0f * a * c;
            if (!(discriminant < 0.0f)) {
                float t1 = (-b - mtSqrtf(discriminant)) / (2.0f * a);
                float t2 = (-b + mtSqrtf(discriminant)) / (2.0f * a);
                if (!(t1 > 1.0f) && !(t2 < 0.0f)) {
                    result = true;
                }
            }
        }
    }
    return result;
}

// True when the distance between the centres does not exceed the sum of the radii.
bool clCircle2D::checkIntersect1(const clCircle2D& other) const {
    Vec2f a(m_centerPos.m_x, m_centerPos.m_y);
    Vec2f b(other.m_centerPos.m_x, other.m_centerPos.m_y);
    Vec2f diff(a.m_x - b.m_x, a.m_y - b.m_y);
    float radii = m_radius + other.m_radius;
    return diff.lengthSq() <= radii * radii;
}

bool clCircle2D::checkIntersect2(const clTriangle2D& triangle) const {
    return triangle.checkIntersect1(*this);
}

bool clCircle2D::checkInclude(const Vec2f& point) const {
    bool result = false;
    Vec2f center(m_centerPos.m_x, m_centerPos.m_y);
    Vec2f diff = center - point;
    if (diff.lengthSq() <= m_radius * m_radius) {
        result = true;
    }
    return result;
}

void clCircle2D::getAABBox(clAABBox2D& aabBox) const {
    float x;
    float r;
    float y;

    y = m_centerPos.m_y;
    r = m_radius;
    x = m_centerPos.m_x;

    float xPlusR;
    float yPlusR;
    float yMinusR;
    float xMinusR;

    yPlusR = y + r;
    xPlusR = x + r;
    yMinusR = y - r;
    xMinusR = x - r;

    aabBox.m_unk4 = yPlusR;
    aabBox.m_unk3 = xPlusR;
    aabBox.m_unk1 = xMinusR;
    aabBox.m_unk2 = yMinusR;
}

// MATCH-ONLY: the shipped build keeps only the vertex set-up of the debug draw (the fill pass is gone).
void clCircle2D::draw(GXColor color, float width, bool fill) const {
    gfCameraManager::getManager()->m_cameras[0].setGX();

    u32 rgba = *reinterpret_cast<u32*>(&color);
    Vec3f vtx[18];
    vtx[0] = Vec3f(m_centerPos.m_x, m_centerPos.m_y, 0.0f);
    for (int i = 0; i < 16; i++) {
        float angle = 3.14159274f * (2.0f * (0.0625f * i));
        float y = m_centerPos.m_y + m_radius * (float)sin(angle);
        float x = m_centerPos.m_x + m_radius * (float)cos(angle);
        vtx[i + 1] = Vec3f(x, y, 0.0f);
    }
    vtx[17] = vtx[1];

    gfDrawSetVtxPosColorPrimEnvironment();
    GXSetZMode(true, GX_LEQUAL, false);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXBegin(GX_TRIANGLEFAN, GX_VTXFMT1, 18);
    for (int i = 0; i < 18; i++) {
        GXPosition3f32(vtx[i].m_x, vtx[i].m_y, vtx[i].m_z);
        GXColor1u32(rgba);
    }
    GXEnd();
    if (fill == true) {
        // HYPOTHESIS: the native code stores this fill colour (255, 255, 255, 128) and then makes a call
        // whose effect is gone from the shipped build; the dead stores are not reproduced yet.
        gfCameraManager::getManager();
    }
}

void clCircle2D::interpolate(const clCircle2D& target, clCircle2D& out, float rate) const {
    out.m_centerPos.m_x = m_centerPos.m_x + rate * (target.m_centerPos.m_x - m_centerPos.m_x);
    out.m_centerPos.m_y = m_centerPos.m_y + rate * (target.m_centerPos.m_y - m_centerPos.m_y);
    out.m_radius = m_radius + rate * (target.m_radius - m_radius);
}

bool clAABBox2D::checkInclude(const Vec2f& point) const {
    bool result = false;
    if (!(m_unk3 < point.m_x) && !(m_unk1 > point.m_x) && !(m_unk4 < point.m_y) && !(m_unk2 > point.m_y)) {
        result = true;
    }
    return result;
}

bool clAABBox2D::checkInclude(const clAABBox2D& other) const {
    bool result = false;
    if (!(m_unk3 < other.m_unk3) && !(m_unk1 > other.m_unk1) && !(m_unk4 < other.m_unk4) && !(m_unk2 > other.m_unk2)) {
        result = true;
    }
    return result;
}

// True when the segment touches the box: an end point lies inside, or the segment crosses an edge. The
// corners are first tested against the segment's line (all on one side: no crossing possible).
bool clAABBox2D::checkIntersect(const clSegment2D& segment) const {
    bool result = false;
    clAABBox2D segBox;
    segmentBox(&segment, &segBox);

    bool overlap = false;
    if (!(m_unk3 < segBox.m_unk1) && !(m_unk1 > segBox.m_unk3) && !(m_unk4 < segBox.m_unk2) && !(m_unk2 > segBox.m_unk4)) {
        overlap = true;
    }
    if (overlap) {
        if (checkInclude(segment.m_p0Pos) || checkInclude(segment.m_p1Pos)) {
            result = true;
        } else {
            Vec2f dir = segment.m_p1Pos - segment.m_p0Pos;
            Vec2f toCorner = Vec2f(m_unk1, m_unk2) - segment.m_p0Pos;
            Vec2f normal(-dir.m_y, dir.m_x);
            float side = normal.m_x * toCorner.m_x + normal.m_y * toCorner.m_y;

            Vec2f corners[4];
            corners[3] = Vec2f(m_unk1, m_unk2);
            corners[2] = Vec2f(m_unk1, m_unk4);
            corners[1] = Vec2f(m_unk3, m_unk2);
            corners[0] = Vec2f(m_unk3, m_unk4);
            if (side < 0.0f) {
                normal = Vec2f(-normal.m_x, -normal.m_y);
                side = -side;
            }
            Vec2f w2 = corners[2] - segment.m_p0Pos;
            Vec2f w1 = corners[1] - segment.m_p0Pos;
            Vec2f w0 = corners[0] - segment.m_p0Pos;
            float side2 = normal.m_x * w2.m_x + normal.m_y * w2.m_y;
            float side1 = normal.m_x * w1.m_x + normal.m_y * w1.m_y;
            float side0 = normal.m_x * w0.m_x + normal.m_y * w0.m_y;
            if (!(side > 0.0f && side2 > 0.0f && side1 > 0.0f && side0 > 0.0f)) {
                result = true;
                clSegment2D edge;
                edge.m_p0Pos = corners[3];
                edge.m_p1Pos = corners[2];
                if (!clSegment2D::checkIntersect(&segment, &edge)) {
                    edge.m_p1Pos = corners[1];
                    if (!clSegment2D::checkIntersect(&segment, &edge)) {
                        edge.m_p0Pos = corners[0];
                        if (!clSegment2D::checkIntersect(&segment, &edge)) {
                            edge.m_p1Pos = corners[2];
                            if (!clSegment2D::checkIntersect(&segment, &edge)) {
                                result = false;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

bool clAABBox2D::checkIntersect1(const clAABBox2D& other) const {
    if (m_unk3 < other.m_unk1) {
        return false;
    }
    if (m_unk4 < other.m_unk2) {
        return false;
    }
    if (m_unk1 > other.m_unk3) {
        return false;
    }
    return !(m_unk2 > other.m_unk4);
}

// True when the circle overlaps the box: the centre lies in the box's span on one axis, or the box corner nearest
// the centre is closer than the radius.
bool clAABBox2D::checkIntersect2(const clCircle2D& circle) const {
    float cx = circle.m_centerPos.m_x;
    float cy = circle.m_centerPos.m_y;
    float r = circle.m_radius;
    bool result = false;
    if (!(m_unk3 < cx - r) && !(m_unk4 < cy - r) && !(m_unk1 > cx + r) && !(m_unk2 > cy + r)) {
        result = true;
        if (m_unk3 >= cx && m_unk1 <= cx) {
            goto done;
        }
        if (m_unk4 >= cy && m_unk2 <= cy) {
            goto done;
        }
        {
            float rr = r * r;
            if (m_unk1 > cx) {
                float dx = cx - m_unk1;
                if (m_unk4 < cy) {
                    Vec2f d(dx, cy - m_unk4);
                    if (d.lengthSq() < rr) {
                        goto done;
                    }
                }
                if (cy < m_unk2) {
                    Vec2f d(dx, cy - m_unk2);
                    if (d.lengthSq() < rr) {
                        goto done;
                    }
                }
            } else if (m_unk3 < cx) {
                float dx = cx - m_unk3;
                if (m_unk4 < cy) {
                    Vec2f d(dx, cy - m_unk4);
                    if (d.lengthSq() < rr) {
                        goto done;
                    }
                }
                if (cy < m_unk2) {
                    Vec2f d(dx, cy - m_unk2);
                    if (d.lengthSq() < rr) {
                        goto done;
                    }
                }
            }
        }
        result = false;
    }
done:
    return result;
}

// MATCH-ONLY: the original takes the componentwise minimum of the low corners and maximum of the high corners with paired singles.
void clAABBox2D::logicalOR(const clAABBox2D& other, clAABBox2D& out) const {
    register const clAABBox2D* a = this;
    register const clAABBox2D* b = &other;
    register clAABBox2D* dst = &out;
    register float diffLo, bLo, aLo, diffHi, bHi, aHi;
    // clang-format off
    asm {
        psq_l aLo, 0(a), 0, 0
        psq_l bLo, 0(b), 0, 0
        psq_l aHi, 8(a), 0, 0
        psq_l bHi, 8(b), 0, 0
        ps_sub diffLo, aLo, bLo
        ps_sub diffHi, aHi, bHi
        ps_sel diffLo, diffLo, bLo, aLo
        ps_sel bLo, diffHi, aHi, bHi
        psq_st diffLo, 0(dst), 0, 0
        psq_st bLo, 8(dst), 0, 0
    }
    // clang-format on
}

// MATCH-ONLY: the shipped build keeps only the vertex set-up of the debug draw.
void clAABBox2D::draw(GXColor color, float width, bool fill) const {
    gfCameraManager::getManager()->m_cameras[0].setGX();
    gfDrawSetVtxPosColorPrimEnvironment();
    Vec3f vtx[5];
    vtx[0] = Vec3f(m_unk1, m_unk4, 0.0f);
    vtx[1] = Vec3f(m_unk1, m_unk2, 0.0f);
    vtx[2] = Vec3f(m_unk3, m_unk2, 0.0f);
    vtx[3] = Vec3f(m_unk3, m_unk4, 0.0f);
    vtx[4] = Vec3f(m_unk1, m_unk4, 0.0f);
    gfCameraManager::getManager();
}


// ===== part C =====
// MATCH-ONLY: the scale of a 3D vector with paired singles (see mt_vector.h).

// HYPOTHESIS: unnamed mt_coll2d entry points (map: clSegment2D checkIntersect / checkIntersect1); parameter types from register use.

// Geometric helpers of the 2D library (inlined by the compiler into every user below; the same helpers are
// defined in the other clusters of this file).
// The dot product of two vectors.
static inline float dot(const Vec2f& a, const Vec2f& b) {
    return a.m_x * b.m_x + a.m_y * b.m_y;
}

// The vector turned 90 degrees to the left: (-y, x).
static inline Vec2f perp(const Vec2f& v) {
    return Vec2f(-v.m_y, v.m_x);
}

// The vector turned 90 degrees to the right: (y, -x).
static inline Vec2f perpRight(const Vec2f& v) {
    return Vec2f(v.m_y, -v.m_x);
}

// HYPOTHESIS: stand-ins for clAABBox2D::checkIntersect1 / checkInclude, which the original unit inlines
// into the triangle tests (the out-of-line copies live in the box cluster).
static inline bool boxesOverlap(const clAABBox2D& a, const clAABBox2D& b) {
    if (a.m_unk3 < b.m_unk1) {
        return false;
    }
    if (a.m_unk4 < b.m_unk2) {
        return false;
    }
    if (a.m_unk1 > b.m_unk3) {
        return false;
    }
    return !(a.m_unk2 > b.m_unk4);
}

static inline bool boxIncludes(const clAABBox2D& box, const Vec2f& point) {
    bool result = false;
    if (!(box.m_unk3 < point.m_x) && !(box.m_unk1 > point.m_x) && !(box.m_unk4 < point.m_y) && !(box.m_unk2 > point.m_y)) {
        result = true;
    }
    return result;
}

// MATCH-ONLY: the original widens the box with paired-single max/min (ps_sub, ps_sel); the box holds the first vertex.
static inline void widenByTriangle(register const clTriangle2D* tri, register clAABBox2D* box) {
    register float hi1, hi2, diff3, lo1, v2, lo2, diff2, diff1, lo, hi, v1, diff0;
    // clang-format off
    asm {
        psq_l  v1, 8(tri), 0, 0
        psq_l  v2, 16(tri), 0, 0
        psq_l  hi, 8(box), 0, 0
        ps_sub diff0, hi, v1
        ps_sel hi1, diff0, hi, v1
        psq_l  lo, 0(box), 0, 0
        ps_sub diff1, lo, v1
        ps_sub diff2, hi1, v2
        ps_sel lo1, diff1, v1, lo
        ps_sel hi2, diff2, hi1, v2
        ps_sub diff3, lo1, v2
        psq_st hi2, 8(box), 0, 0
        ps_sel lo2, diff3, v2, lo1
        psq_st lo2, 0(box), 0, 0
    }
    // clang-format on
}

// The first vertex is both corners of the box before the others widen it.
static inline void startBox(const clTriangle2D* tri, clAABBox2D* box) {
    box->m_unk3 = tri->m_vertices[0].m_x;
    box->m_unk4 = tri->m_vertices[0].m_y;
    box->m_unk1 = tri->m_vertices[0].m_x;
    box->m_unk2 = tri->m_vertices[0].m_y;
}

// aabBox is (min x, min y, max x, max y).
void clTriangle2D::getAABBox(clAABBox2D& aabBox) const {
    startBox(this, &aabBox);
    widenByTriangle(this, &aabBox);
}

bool clTriangle2D::checkIntersect(const clAABBox2D& aabBox) const {
    bool result = false;
    clAABBox2D bounds;
    getAABBox(bounds);
    if (boxesOverlap(bounds, aabBox)) {
        result = true;
        clSegment2D edge0(m_vertices[0], m_vertices[1]);
        if (!aabBox.checkIntersect(edge0)) {
            clSegment2D edge1(m_vertices[1], m_vertices[2]);
            if (!aabBox.checkIntersect(edge1)) {
                clSegment2D edge2(m_vertices[2], m_vertices[0]);
                if (!aabBox.checkIntersect(edge2)) {
                    for (int i = 0; i < 3; i++) {
                        if (boxIncludes(aabBox, m_vertices[i])) {
                            break;
                        }
                    }
                    result = false;
                }
            }
        }
    }
    return result;
}

// True when pos is on the right of the segment (looking from p0 to p1).
static inline bool isRightArea(const clSegment2D& seg, const Vec2f& pos) {
    Vec2f d = seg.m_p1Pos - seg.m_p0Pos;
    Vec2f v = pos - seg.m_p0Pos;
    Vec2f n = perpRight(d);
    return dot(n, v) > 0.0f;
}

bool clTriangle2D::checkIntersect1(const clCircle2D& circle) const {
    bool result = false;
    clAABBox2D bounds;
    getAABBox(bounds);
    if (bounds.checkIntersect2(circle)) {
        result = true;
        clSegment2D edge0(m_vertices[0], m_vertices[1]);
        if (!circle.checkIntersect(edge0)) {
            clSegment2D edge1(m_vertices[1], m_vertices[2]);
            if (!circle.checkIntersect(edge1)) {
                clSegment2D edge2(m_vertices[2], m_vertices[0]);
                if (!circle.checkIntersect(edge2)) {
                    // no edge touches the circle: it is inside when its centre is on the inner side of all three edges
                    float y = circle.m_centerPos.m_y;
                    float x = circle.m_centerPos.m_x;
                    Vec2f center;
                    center.m_x = x;
                    center.m_y = y;
                    u32 inside = 0;
                    for (u32 i = 0; i < 3; i++) {
                        u32 next = i + 1;
                        if (next == 3) {
                            next = 0;
                        }
                        clSegment2D edge;
                        edge.m_p0Pos = m_vertices[i];
                        edge.m_p1Pos = m_vertices[next];
                        if (isRightArea(edge, center)) {
                            inside++;
                        }
                    }
                    if (inside != 3) {
                        result = false;
                    }
                }
            }
        }
    }
    return result;
}

// True when all three vertices of other are on the outer (right) side of one edge of tri: that edge separates them.
static inline bool hasSeparatingEdge(const clTriangle2D& tri, const clTriangle2D& other) {
    for (u32 i = 0; i < 3; i++) {
        u32 next = i + 1;
        if (next == 3) {
            next = 0;
        }
        clSegment2D edge;
        edge.m_p0Pos = tri.m_vertices[i];
        edge.m_p1Pos = tri.m_vertices[next];
        u32 outside = 0;
        for (u32 j = 0; j < 3; j++) {
            Vec2f e = edge.m_p1Pos - edge.m_p0Pos;
            Vec2f d = other.m_vertices[j] - edge.m_p0Pos;
            Vec2f n = perpRight(e);
            if (dot(n, d) > 0.0f) {
                outside++;
            }
        }
        if (outside == 3) {
            return true;
        }
    }
    return false;
}

bool clTriangle2D::checkIntersect2(const clTriangle2D& other) const {
    bool result = false;
    clAABBox2D bounds;
    clAABBox2D otherBounds;
    getAABBox(bounds);
    other.getAABBox(otherBounds);
    if (boxesOverlap(bounds, otherBounds)) {
        result = true;
        if (hasSeparatingEdge(*this, other)) {
            result = false;
        } else if (hasSeparatingEdge(other, *this)) {
            result = false;
        }
    }
    return result;
}

// HYPOTHESIS: the GX outline draw was compiled out of the shipped build; what is left is the camera setup and dead
// stack stores of the four outline vertices (a closed line strip), which are not reproduced here yet (13% of the bytes).
void clTriangle2D::draw(GXColor color, float width, bool fill) const {
    gfCameraManager::getManager()->m_cameras[0].setGX();
    gfCameraManager::getManager();
}

clQuad2D::clQuad2D(const clSegment2D& a, const clSegment2D& b) {
    set(a, b);
}

void clQuad2D::set(const clSegment2D& a, const clSegment2D& b) {
    clSegment2D sa;
    if (a.m_p0Pos.m_x < a.m_p1Pos.m_x) {
        sa = a;
    } else {
        sa.m_p0Pos = a.m_p1Pos;
        sa.m_p1Pos = a.m_p0Pos;
    }
    clSegment2D sb;
    if (b.m_p0Pos.m_x < b.m_p1Pos.m_x) {
        sb = b;
    } else {
        sb.m_p0Pos = b.m_p1Pos;
        sb.m_p1Pos = b.m_p0Pos;
    }
    // the line through the first segment, with b's end points measured against it
    Vec2f d = sa.m_p1Pos - sa.m_p0Pos;
    Vec2f n = perp(d);
    n.normalize();
    float dist = -dot(n, sa.m_p0Pos);
    float side0 = dist + dot(n, sb.m_p0Pos);
    float side1 = dist + dot(n, sb.m_p1Pos);
    if (side0 < 0.0f && side1 < 0.0f) {
        m_vertices[0] = sa.m_p0Pos;
        m_vertices[1] = sb.m_p0Pos;
        m_vertices[2] = sb.m_p1Pos;
        m_vertices[3] = sa.m_p1Pos;
    } else if (side0 > 0.0f && side1 > 0.0f) {
        m_vertices[0] = sb.m_p0Pos;
        m_vertices[1] = sa.m_p0Pos;
        m_vertices[2] = sa.m_p1Pos;
        m_vertices[3] = sb.m_p1Pos;
    } else if (side0 < 0.0f && side1 > 0.0f) {
        m_vertices[0] = sa.m_p0Pos;
        m_vertices[1] = sb.m_p0Pos;
        m_vertices[2] = sa.m_p1Pos;
        m_vertices[3] = sb.m_p1Pos;
    } else {
        m_vertices[0] = sb.m_p0Pos;
        m_vertices[1] = sa.m_p0Pos;
        m_vertices[2] = sb.m_p1Pos;
        m_vertices[3] = sa.m_p1Pos;
    }
}


// HYPOTHESIS: stand-in for the clLine2DParam constructor, which the original unit inlines into the quad tests
// (the out-of-line copy lives in the ray cluster). The line runs through from and to; the distance of a point is
// positive on the left of the direction from -> to.
struct EdgeLine {
    Vec2f m_normal;
    float m_distance;
    float distance(const Vec2f& point) const { return m_normal.m_x * point.m_x + m_normal.m_y * point.m_y + m_distance; }
};
struct EdgeRay {
    Vec2f m_pos;
    Vec2f m_vec;
};
static inline void makeEdgeLine(EdgeLine& line, const Vec2f& from, const Vec2f& to) {
    Vec2f d = to - from;
    EdgeRay ray;
    ray.m_pos = from;
    ray.m_vec.normalize(&d);
    Vec2f normal;
    normal.m_y = ray.m_vec.m_x;
    normal.m_x = -ray.m_vec.m_y;
    normal.normalize();
    float dotv = normal.m_x * ray.m_pos.m_x + normal.m_y * ray.m_pos.m_y;
    line.m_normal = normal;
    line.m_distance = -dotv;
}

bool clQuad2D::checkInclude(const Vec2f& pos) const {
    bool result = false;
    EdgeLine line0;
    makeEdgeLine(line0, m_vertices[0], m_vertices[1]);
    if (!(line0.distance(pos) < 0.0f)) {
        EdgeLine line1;
        makeEdgeLine(line1, m_vertices[1], m_vertices[2]);
        if (!(line1.distance(pos) < 0.0f)) {
            EdgeLine line2;
            makeEdgeLine(line2, m_vertices[2], m_vertices[3]);
            if (!(line2.distance(pos) < 0.0f)) {
                EdgeLine line3;
                makeEdgeLine(line3, m_vertices[3], m_vertices[0]);
                if (!(line3.distance(pos) < 0.0f)) {
                    result = true;
                }
            }
        }
    }
    return result;
}

bool clQuad2D::checkIntersect(const clSegment2D& segment) const {
    bool result = false;
    EdgeLine line0;
    makeEdgeLine(line0, m_vertices[0], m_vertices[1]);
    if (!(line0.distance(segment.m_p0Pos) < 0.0f && line0.distance(segment.m_p1Pos) < 0.0f)) {
        EdgeLine line1;
        makeEdgeLine(line1, m_vertices[1], m_vertices[2]);
        if (!(line1.distance(segment.m_p0Pos) < 0.0f && line1.distance(segment.m_p1Pos) < 0.0f)) {
            EdgeLine line2;
            makeEdgeLine(line2, m_vertices[2], m_vertices[3]);
            if (!(line2.distance(segment.m_p0Pos) < 0.0f && line2.distance(segment.m_p1Pos) < 0.0f)) {
                EdgeLine line3;
                makeEdgeLine(line3, m_vertices[3], m_vertices[0]);
                if (!(line3.distance(segment.m_p0Pos) < 0.0f && line3.distance(segment.m_p1Pos) < 0.0f)) {
                    // the segment's own line: when all four corners are on one side of it, there is no overlap
                    EdgeLine segLine;
                    makeEdgeLine(segLine, segment.m_p0Pos, segment.m_p1Pos);
                    int side = 0;
                    side += segLine.distance(m_vertices[0]) > 0.0f ? 1 : -1;
                    side += segLine.distance(m_vertices[1]) > 0.0f ? 1 : -1;
                    side += segLine.distance(m_vertices[2]) > 0.0f ? 1 : -1;
                    side += segLine.distance(m_vertices[3]) > 0.0f ? 1 : -1;
                    if (side != 4 && side != -4) {
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

clRhombus2D::clRhombus2D(float top, float bottom, float left, float right, float x, float y) {
    m_centerPos.m_x = x;
    m_centerPos.m_y = y;
    m_upPos.m_x = x;
    m_upPos.m_y = top;
    m_downPos.m_x = x;
    m_downPos.m_y = bottom;
    m_leftPos.m_x = left;
    m_leftPos.m_y = y;
    m_rightPos.m_x = right;
    m_rightPos.m_y = y;
    m_aabBox.m_unk1 = left;
    m_aabBox.m_unk2 = bottom;
    m_aabBox.m_unk3 = top;
    m_aabBox.m_unk4 = right;
}

void clRhombus2D::move(const Vec2f& displacement) {
    m_centerPos.m_x += displacement.m_x;
    m_centerPos.m_y += displacement.m_y;
    m_upPos.m_x += displacement.m_x;
    m_upPos.m_y += displacement.m_y;
    m_downPos.m_x += displacement.m_x;
    m_downPos.m_y += displacement.m_y;
    m_leftPos.m_x += displacement.m_x;
    m_leftPos.m_y += displacement.m_y;
    m_rightPos.m_x += displacement.m_x;
    m_rightPos.m_y += displacement.m_y;
    m_aabBox.m_unk1 += displacement.m_x;
    m_aabBox.m_unk2 += displacement.m_y;
    m_aabBox.m_unk3 += displacement.m_x;
    m_aabBox.m_unk4 += displacement.m_y;
}

u32 clRhombus2D::getIntersectInfo(const clSegment2D& segment) const {
    u32 flags = 0;
    clSegment2D upRight(m_upPos, m_rightPos);
    clSegment2D downRight(m_downPos, m_rightPos);
    clSegment2D downLeft(m_downPos, m_leftPos);
    clSegment2D upLeft(m_upPos, m_leftPos);
    if (clSegment2D::checkIntersect(&upRight, &segment)) {
        flags |= 1;
    }
    if (clSegment2D::checkIntersect(&downRight, &segment)) {
        flags |= 2;
    }
    if (clSegment2D::checkIntersect(&downLeft, &segment)) {
        flags |= 4;
    }
    if (clSegment2D::checkIntersect(&upLeft, &segment)) {
        flags |= 8;
    }
    return flags;
}

// MATCH-ONLY: the original interpolates all five points with paired singles and rebuilds the box with ps_sel.
void clRhombus2D::interpolate(const clRhombus2D& target, clRhombus2D& out, float rate) const {
    register const clRhombus2D* from = this;
    register const clRhombus2D* to = &target;
    register clRhombus2D* dst = &out;
    register float t = rate;
    register float inv = 1.0f - rate;
    register float cenA, cen, cenTo, up, upTo, downS, downTo, down, leftA, leftS, leftTo, left, rightA, rightS, rightTo, right;
    // out = from * (1 - rate) + to * rate for every point
    // clang-format off
    asm {
        psq_l    cenA, 0(from), 0, 0
        psq_l    up, 8(from), 0, 0
        psq_l    downS, 16(from), 0, 0
        ps_muls0 cen, cenA, inv
        psq_l    cenTo, 0(to), 0, 0
        ps_muls0 up, up, inv
        psq_l    upTo, 8(to), 0, 0
        ps_muls0 downS, downS, inv
        psq_l    leftA, 24(from), 0, 0
        ps_madds0 cen, cenTo, t, cen
        psq_l    downTo, 16(to), 0, 0
        ps_madds0 up, upTo, t, up
        psq_l    rightA, 32(from), 0, 0
        ps_madds0 down, downTo, t, downS
        psq_l    leftTo, 24(to), 0, 0
        psq_st   cen, 0(dst), 0, 0
        ps_muls0 leftS, leftA, inv
        ps_muls0 rightS, rightA, inv
        psq_l    rightTo, 32(to), 0, 0
        ps_madds0 left, leftTo, t, leftS
        ps_madds0 right, rightTo, t, rightS
    }
    // clang-format on
    // the box starts as the centre and grows to the four corners
    Vec2f lo;
    Vec2f hi;
    hi = out.m_centerPos;
    lo = out.m_centerPos;
    register Vec2f* hiPtr = &hi;
    register Vec2f* loPtr = &lo;
    register float hiV, loV, d0, d1, d2, d3, d4, d5, d6, d7, hi1, lo1, hi2, lo2, hi3, lo3, hi4, lo4;
    // clang-format off
    asm {
        psq_l  hiV, 0(hiPtr), 0, 0
        ps_sub d0, up, hiV
        psq_l  loV, 0(loPtr), 0, 0
        ps_sel hi1, d0, up, hiV
        ps_sub d1, up, loV
        psq_st up, 8(dst), 0, 0
        ps_sub d2, down, hi1
        ps_sel lo1, d1, loV, up
        psq_st down, 16(dst), 0, 0
        ps_sel hi2, d2, down, hi1
        ps_sub d3, down, lo1
        psq_st left, 24(dst), 0, 0
        ps_sub d4, left, hi2
        ps_sel lo2, d3, lo1, down
        psq_st right, 32(dst), 0, 0
        ps_sel hi3, d4, left, hi2
        ps_sub d5, left, lo2
        ps_sub d6, right, hi3
        ps_sel lo3, d5, lo2, left
        ps_sel hi4, d6, right, hi3
        ps_sub d7, right, lo3
        psq_st hi4, 0(hiPtr), 0, 0
        ps_sel lo4, d7, lo3, right
        psq_st lo4, 0(loPtr), 0, 0
    }
    // clang-format on
    out.m_aabBox.m_unk3 = hi.m_x;
    out.m_aabBox.m_unk1 = lo.m_x;
    out.m_aabBox.m_unk2 = lo.m_y;
    out.m_aabBox.m_unk4 = hi.m_y;
}

// HYPOTHESIS: like clTriangle2D::draw, the GX outline draw was compiled out; only dead stack stores of the five outline
// vertices (up, right, down, left, up) and the camera manager access remain, which are not reproduced here yet.
void clRhombus2D::draw(GXColor color, float width, bool fill) const {
    gfCameraManager::getManager();
}

// MATCH-ONLY: the same paired-single scale as Vec3fScale, with the registers of this unit's native allocation.
static inline void scaleVec3(register Vec3f* out, register const Vec3f* v, register float c) {
    register f32 frZ, frXY;
    // clang-format off
    asm {
        psq_l    frXY, Vec3f.m_x(v), 0, 0
        psq_l    frZ, Vec3f.m_z(v), 1, 0
        ps_muls0 frXY, frXY, c
        ps_muls0 frZ, frZ, c
        psq_st   frXY, Vec3f.m_x(out), 0, 0
        psq_st   frZ, Vec3f.m_z(out), 1, 0
    }
    // clang-format on
}

bool clLine3D::checkIntersectXYPlane(Vec3f& out) const {
    bool hit;
    float t;
    double a = __fabs(m_vec.m_z);
    float absZ = a;
    if (absZ <= 1.1920929e-7f) {
        hit = false;
    } else {
        t = -m_pos.m_z / m_vec.m_z;
        hit = true;
    }
    if (hit) {
        Vec3f point;
        Vec3f scaled;
        scaleVec3(&scaled, &m_vec, t);
        Vec3fAdd(&point, &m_pos, &scaled);
        out = point;
    }
    return hit;
}

