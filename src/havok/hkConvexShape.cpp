// Havok translation unit hkConvexShape.o (main.dol 0x802D0890-0x802D0A2C).
// Functions in address order:
//   0x802D0890   132  getMaximumProjection   [map: hkConvexShape__getMaximumProjection]
//   0x802D0914   272  castRayWithCollector   [map: hkConvexShape__castRayWithCollector]
//   0x802D0A24     8  getType   [map: hkConvexShape__getType]

#include <havok/hkConvexShape.h>
#include <havok/hkShapeType.h>

#pragma fp_contract on

hkReal hkConvexShape::getMaximumProjection(const hkVector4& direction) const {
    hkVector4 support;
    getSupportingVertex(direction, support);
    return m_radius + support.dot3(direction);
}

int hkConvexShape::getType() const {
    return HK_SHAPE_CONVEX;
}

// Not yet decompiled in this unit:
//   0x802D0914   272  castRayWithCollector   [map: hkConvexShape__castRayWithCollector]
