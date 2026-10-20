// Local shadow of BrawlHeaders: adds clSegment2D::checkIntersect2 (main.dol 0x80040F74).
#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <types.h>

class clAABBox2D {
public:
    float m_unk1;
    float m_unk2;
    float m_unk3;
    float m_unk4;
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

    void move(const Vec2f& displacement);
};

class clSegment2D {
public:
    Vec2f m_p0Pos;
    Vec2f m_p1Pos;

    // HYPOTHESIS: intersection point of two segments (written to out); the result tells whether they cross.
    static int checkIntersect2(clSegment2D* a, clSegment2D* b, Vec2f* out);
};
