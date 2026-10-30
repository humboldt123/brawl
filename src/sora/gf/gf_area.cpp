// MATCH-ONLY: this unit copies whole vectors with the aggregate word copy.
#define MT_VEC2F_PLAIN_BASE
#include <gf/gf_area.h>
#include <gf/gf_area_manager.h>
#include <mt/mt_coll2d.h>
#include <types.h>

int lbl_8059C428 = 1;
gfAreaEventReciverNull lbl_8059FF88;
nw4r::ut::Color lbl_8059FF8C(0x80, 0x80, 0x80, 0x60);

gfArea::gfArea(const Vec2f& pos, bool isRegist)
    : m_0x0(0), m_0x4(0), m_checkTargetGroup(-1), m_checkTargetLocalGroup(0), m_isDrawFill(true), m_unk1C_6(true),
      m_isNoDraw(false), m_isRegist(false), m_group(Group_None), m_pos(pos), m_eventReciver(&lbl_8059FF88), m_ownerId(-1) {
    m_debugColor.r = 0x80;
    m_debugColor.g = 0xC0;
    m_debugColor.b = 0x40;
    m_debugColor.a = 0x80;
    m_teamId = m_id = lbl_8059C428++;
    m_aabBox.m_unk1 = -10.0f;
    m_aabBox.m_unk2 = -10.0f;
    m_aabBox.m_unk3 = 10.0f;
    m_aabBox.m_unk4 = 10.0f;
    if (isRegist == true) {
        regist();
    }
}

gfArea::~gfArea() {
    unRegist();
}

void gfArea::regist() {
    if (!m_isRegist) {
        lbl_8059FF90->registArea(this);
        m_isRegist = true;
    }
}

void gfArea::unRegist() {
    if (m_isRegist == true) {
        lbl_8059FF90->removeArea(this);
        m_isRegist = false;
    }
}

void gfArea::setEventReciver(gfAreaEventReciver* reciver) {
    m_eventReciver = reciver;
}

void gfArea::test(gfArea* other) {
    bool isHit;
    bool isOtherHit;
    if (m_teamId != other->m_teamId) {
        isHit = ((1 << other->m_group) & m_checkTargetGroup) != 0;
        isOtherHit = ((1 << m_group) & other->m_checkTargetGroup) != 0;
    } else {
        isHit = ((1 << other->m_group) & m_checkTargetLocalGroup) != 0;
        isOtherHit = ((1 << m_group) & other->m_checkTargetLocalGroup) != 0;
    }
    if (isHit || isOtherHit) {
        if (m_aabBox.checkIntersect1(other->m_aabBox) && testIntersectShape(other)) {
            if (isHit) {
                notifyIntersect(other);
            }
            if (isOtherHit) {
                other->notifyIntersect(this);
            }
        }
    }
}

bool gfArea::testIntersect(gfArea* other) {
    bool result = false;
    if (m_aabBox.checkIntersect1(other->m_aabBox) && testIntersectShape(other)) {
        result = true;
    }
    return result;
}

bool gfArea::testIntersectShape(gfArea* other) {
    return true;
}

void gfArea::notifyIntersect(gfArea* other) {
    m_eventReciver->reciveEvent(this, other);
}

void gfArea::debugDrawImp() {
    if (!m_isNoDraw) {
        m_aabBox.draw(m_debugColor, 1.0f, false);
    }
}

void gfAreaRectangle::update() {
    Vec2f v0 = m_vertices[0] + m_pos;
    Vec2f v1 = m_vertices[1] + m_pos;
    Vec2f v2 = m_vertices[2] + m_pos;
    Vec2f v3 = m_vertices[3] + m_pos;
    Vec2f min = v0;
    Vec2f max = v0;
    if (v1.m_x < min.m_x) min.m_x = v1.m_x;
    m_aabBox.m_unk1 = min.m_x;
    m_aabBox.m_unk2 = min.m_y;
    m_aabBox.m_unk3 = max.m_x;
    m_aabBox.m_unk4 = max.m_y;
}

void gfAreaRectangle::setShape(const Vec2f& center, const Vec2f& size) {
    m_isNoDraw = true;
    m_vertices[0] = Vec2f(center.m_x - size.m_x * 0.5f, center.m_y - size.m_y * 0.5f);
    m_vertices[1] = Vec2f(center.m_x + size.m_x * 0.5f, center.m_y - size.m_y * 0.5f);
    m_vertices[2] = Vec2f(center.m_x + size.m_x * 0.5f, center.m_y + size.m_y * 0.5f);
    m_vertices[3] = Vec2f(center.m_x - size.m_x * 0.5f, center.m_y + size.m_y * 0.5f);
}

bool gfAreaRectangle::testIntersectShape(gfArea* other) {
    bool result = false;
    switch (other->m_shapeType) {
    case Shape_Rectangle:
        result = true;
        break;
    case Shape_Circle: {
        gfAreaCircle* circle = static_cast<gfAreaCircle*>(other);
        clCircle2D shape;
        shape.m_centerPos.m_x = circle->m_circle.m_centerPos.m_x + circle->m_pos.m_x;
        shape.m_centerPos.m_y = circle->m_circle.m_centerPos.m_y + circle->m_pos.m_y;
        shape.m_radius = circle->m_circle.m_radius;
        result = m_aabBox.checkIntersect2(shape);
        break;
    }
    }
    return result;
}

bool gfAreaRectangle::testIncludePos(Vec2f* pos) {
    return m_aabBox.checkInclude(*pos);
}

void gfAreaRectangle::debugDrawImp() {
    if (!m_isNoDraw) {
        if (m_isDrawFill) {
            m_aabBox.draw(m_debugColor, 1.0f, true);
        } else {
            m_aabBox.draw(lbl_8059FF8C, 1.0f, false);
        }
    }
}

void gfAreaCircle::update() {
    clCircle2D shape;
    shape = m_circle;
    shape.m_centerPos.m_x = shape.m_centerPos.m_x + m_pos.m_x;
    shape.m_centerPos.m_y = shape.m_centerPos.m_y + m_pos.m_y;
    shape.getAABBox(m_aabBox);
}

void gfAreaCircle::setShape(const Vec2f& center, float radius) {
    m_circle.m_centerPos = center;
    m_circle.m_radius = radius;
    m_isNoDraw = true;
}

bool gfAreaCircle::testIntersectShape(gfArea* other) {
    bool result = false;
    clCircle2D shape;
    shape.m_centerPos.m_x = m_circle.m_centerPos.m_x + m_pos.m_x;
    shape.m_centerPos.m_y = m_circle.m_centerPos.m_y + m_pos.m_y;
    shape.m_radius = m_circle.m_radius;
    switch (other->m_shapeType) {
    case Shape_Rectangle:
        result = dynamic_cast<gfAreaRectangle*>(other)->m_aabBox.checkIntersect2(shape);
        break;
    case Shape_Circle: {
        gfAreaCircle* circle = static_cast<gfAreaCircle*>(other);
        clCircle2D otherShape;
        otherShape.m_centerPos.m_x = circle->m_circle.m_centerPos.m_x + circle->m_pos.m_x;
        otherShape.m_centerPos.m_y = circle->m_circle.m_centerPos.m_y + circle->m_pos.m_y;
        otherShape.m_radius = circle->m_circle.m_radius;
        result = shape.checkIntersect1(otherShape);
        break;
    }
    case Shape_Triangle: {
        gfAreaTriangle* triangle = static_cast<gfAreaTriangle*>(other);
        clTriangle2D otherShape = triangle->m_triangle;
        otherShape.m_vertices[0].m_x = otherShape.m_vertices[0].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[0].m_y = otherShape.m_vertices[0].m_y + triangle->m_pos.m_y;
        otherShape.m_vertices[1].m_x = otherShape.m_vertices[1].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[1].m_y = otherShape.m_vertices[1].m_y + triangle->m_pos.m_y;
        otherShape.m_vertices[2].m_x = otherShape.m_vertices[2].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[2].m_y = otherShape.m_vertices[2].m_y + triangle->m_pos.m_y;
        result = shape.checkIntersect2(otherShape);
        break;
    }
    }
    return result;
}

bool gfAreaCircle::testIncludePos(Vec2f* pos) {
    bool result = false;
    if (m_aabBox.checkInclude(*pos)) {
        clCircle2D shape;
        shape.m_centerPos.m_x = m_circle.m_centerPos.m_x + m_pos.m_x;
        shape.m_centerPos.m_y = m_circle.m_centerPos.m_y + m_pos.m_y;
        shape.m_radius = m_circle.m_radius;
        result = shape.checkInclude(*pos);
    }
    return result;
}

void gfAreaCircle::debugDrawImp() {
    clCircle2D shape;
    shape.m_centerPos.m_x = m_circle.m_centerPos.m_x + m_pos.m_x;
    shape.m_centerPos.m_y = m_circle.m_centerPos.m_y + m_pos.m_y;
    shape.m_radius = m_circle.m_radius;
    if (m_isDrawFill) {
        shape.draw(m_debugColor, 1.0f, true);
    } else {
        shape.draw(lbl_8059FF8C, 1.0f, false);
    }
}

void gfAreaTriangle::update() {
    clTriangle2D shape = m_triangle;
    shape.m_vertices[0].m_x = shape.m_vertices[0].m_x + m_pos.m_x;
    shape.m_vertices[0].m_y = shape.m_vertices[0].m_y + m_pos.m_y;
    shape.m_vertices[1].m_x = shape.m_vertices[1].m_x + m_pos.m_x;
    shape.m_vertices[1].m_y = shape.m_vertices[1].m_y + m_pos.m_y;
    shape.m_vertices[2].m_x = shape.m_vertices[2].m_x + m_pos.m_x;
    shape.m_vertices[2].m_y = shape.m_vertices[2].m_y + m_pos.m_y;
    shape.getAABBox(m_aabBox);
}

bool gfAreaTriangle::testIntersectShape(gfArea* other) {
    bool result = false;
    clTriangle2D shape = m_triangle;
    shape.m_vertices[0].m_x = shape.m_vertices[0].m_x + m_pos.m_x;
    shape.m_vertices[0].m_y = shape.m_vertices[0].m_y + m_pos.m_y;
    shape.m_vertices[1].m_x = shape.m_vertices[1].m_x + m_pos.m_x;
    shape.m_vertices[1].m_y = shape.m_vertices[1].m_y + m_pos.m_y;
    shape.m_vertices[2].m_x = shape.m_vertices[2].m_x + m_pos.m_x;
    shape.m_vertices[2].m_y = shape.m_vertices[2].m_y + m_pos.m_y;
    switch (other->m_shapeType) {
    case Shape_Rectangle:
        result = shape.checkIntersect(dynamic_cast<gfAreaRectangle*>(other)->m_aabBox);
        break;
    case Shape_Circle: {
        gfAreaCircle* circle = static_cast<gfAreaCircle*>(other);
        clCircle2D otherShape;
        otherShape.m_centerPos.m_x = circle->m_circle.m_centerPos.m_x + circle->m_pos.m_x;
        otherShape.m_centerPos.m_y = circle->m_circle.m_centerPos.m_y + circle->m_pos.m_y;
        otherShape.m_radius = circle->m_circle.m_radius;
        result = shape.checkIntersect1(otherShape);
        break;
    }
    case Shape_Triangle: {
        gfAreaTriangle* triangle = static_cast<gfAreaTriangle*>(other);
        clTriangle2D otherShape = triangle->m_triangle;
        otherShape.m_vertices[0].m_x = otherShape.m_vertices[0].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[0].m_y = otherShape.m_vertices[0].m_y + triangle->m_pos.m_y;
        otherShape.m_vertices[1].m_x = otherShape.m_vertices[1].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[1].m_y = otherShape.m_vertices[1].m_y + triangle->m_pos.m_y;
        otherShape.m_vertices[2].m_x = otherShape.m_vertices[2].m_x + triangle->m_pos.m_x;
        otherShape.m_vertices[2].m_y = otherShape.m_vertices[2].m_y + triangle->m_pos.m_y;
        result = shape.checkIntersect2(otherShape);
        break;
    }
    }
    return result;
}

bool gfAreaTriangle::testIncludePos(Vec2f* pos) {
    if (m_aabBox.checkInclude(*pos)) {
        clTriangle2D shape = m_triangle;
        shape.m_vertices[0].m_x = shape.m_vertices[0].m_x + m_pos.m_x;
        shape.m_vertices[0].m_y = shape.m_vertices[0].m_y + m_pos.m_y;
        shape.m_vertices[1].m_x = shape.m_vertices[1].m_x + m_pos.m_x;
        shape.m_vertices[1].m_y = shape.m_vertices[1].m_y + m_pos.m_y;
        shape.m_vertices[2].m_x = shape.m_vertices[2].m_x + m_pos.m_x;
        shape.m_vertices[2].m_y = shape.m_vertices[2].m_y + m_pos.m_y;
    }
    return false;
}

void gfAreaTriangle::debugDrawImp() {
    clTriangle2D shape = m_triangle;
    shape.m_vertices[0].m_x = shape.m_vertices[0].m_x + m_pos.m_x;
    shape.m_vertices[0].m_y = shape.m_vertices[0].m_y + m_pos.m_y;
    shape.m_vertices[1].m_x = shape.m_vertices[1].m_x + m_pos.m_x;
    shape.m_vertices[1].m_y = shape.m_vertices[1].m_y + m_pos.m_y;
    shape.m_vertices[2].m_x = shape.m_vertices[2].m_x + m_pos.m_x;
    shape.m_vertices[2].m_y = shape.m_vertices[2].m_y + m_pos.m_y;
    if (m_isDrawFill) {
        shape.draw(m_debugColor, 1.0f, true);
    } else {
        shape.draw(lbl_8059FF8C, 1.0f, false);
    }
}
