#pragma once

#include <havok/hkBase.h>

struct hkShape;
struct hkMotion;

// Collision body (0x10 bytes). Layout from hkCdBodyClass.cpp.
struct hkCdBody {
    hkShape* m_shape;   // 0x00
    u32 m_shapeKey;     // 0x04
    hkMotion* m_motion; // 0x08
    hkCdBody* m_parent; // 0x0C
};
