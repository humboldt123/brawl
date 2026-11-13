#pragma once

#include <havok/hkShapeCollection.h>
#include <havok/hkArray.h>

// Shape collection holding a list of child shapes (map name hkListShape). Layout from the list and convex list agents:
// the child array at 0x14, 8 bytes per child.
// HYPOTHESIS: the second word of a child entry (Havok's collision filter info; not read by the agents).
struct hkListShape : hkShapeCollection {
    struct ChildInfo {
        const hkShape* m_shape;      // 0x00
        u32 m_collisionFilterInfo;   // 0x04
    };
    hkArray<ChildInfo> m_childInfo; // 0x14
};
