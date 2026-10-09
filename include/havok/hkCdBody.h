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

    // Walks the m_parent chain up to the body without a parent (map name hkCdBody__getRootCollidable).
    hkCdBody* getRootCollidable() {
        hkCdBody* body = this;
        while (body->m_parent != 0) {
            body = body->m_parent;
        }
        return body;
    }
    // Returns the motion of the body (map name hkCdBody__getTransform; the function reads the 0x08 word).
    hkMotion* getTransform() const { return m_motion; }
};
