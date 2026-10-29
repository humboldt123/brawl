// MATCH-ONLY: this unit copies whole vectors with the aggregate word copy.
#define MT_VEC2F_PLAIN_BASE
#include <gf/gf_camera.h>
#include <gf/gf_draw.h>
#include <mt/mt_coll2d.h>
#include <revolution/GX.h>
#include <types.h>

// HYPOTHESIS: unnamed mt_coll2d entry points (map: clRay2D/clSegment2D checkIntersect, clQuad2D set); parameter types from register use.
extern "C" bool fn_800401A4(const clRay2D* ray, const clSegment2D* segment, f32* t);
extern "C" bool fn_800409D4(const clSegment2D* segment, const clSegment2D* other);
extern "C" int fn_80040C00(clSegment2D* a, clSegment2D* b, f32* t);
extern "C" void fn_800429C8(clQuad2D* quad, const clSegment2D* a, const clSegment2D* b);

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

bool clRay2D::checkIntersect(const clSegment2D& segment) const {
    f32 t;
    return fn_800401A4(this, &segment, &t);
}

void clRay2D::getSegment(clSegment2D& segment) const {
    segment.m_p0Pos = m_pos;
    segment.m_p1Pos = m_pos + m_vec;
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

int clSegment2D::checkIntersect2(clSegment2D* a, clSegment2D* b, Vec2f* out) {
    f32 t;
    int hit = fn_80040C00(a, b, &t);

    if (hit != 0) {
        *out = a->m_p0Pos + (a->m_p1Pos - a->m_p0Pos) * t;
    }
    return hit;
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

bool clAABBox2D::checkInclude(const clAABBox2D& other) const {
    bool result = false;
    if (!(m_unk3 < other.m_unk3) && !(m_unk1 > other.m_unk1) && !(m_unk4 < other.m_unk4) && !(m_unk2 > other.m_unk2)) {
        result = true;
    }
    return result;
}

clQuad2D::clQuad2D(const clSegment2D& a, const clSegment2D& b) {
    fn_800429C8(this, &a, &b);
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
    if (fn_800409D4(&upRight, &segment)) {
        flags |= 1;
    }
    if (fn_800409D4(&downRight, &segment)) {
        flags |= 2;
    }
    if (fn_800409D4(&downLeft, &segment)) {
        flags |= 4;
    }
    if (fn_800409D4(&upLeft, &segment)) {
        flags |= 8;
    }
    return flags;
}
