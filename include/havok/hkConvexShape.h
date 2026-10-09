#pragma once

#include <havok/hkSphereRepShape.h>

// Convex shape (0x10 bytes): the convex radius at 0x0C. Layout from hkConvexShapeClass.cpp.
// Adds vtable slots 0x30 getSupportingVertex, 0x34 convertVertexIdsToVertices, 0x38 getFirstVertex,
// 0x3C getNumVertices. Overrides getType, getMaximumProjection and castRayWithCollector.
struct hkConvexShape : hkSphereRepShape {
    hkReal m_radius; // 0x0C

    virtual int getType() const;                                             // 0x10
    virtual hkReal getMaximumProjection(const hkVector4& direction) const;   // 0x18
    virtual void castRayWithCollector(const hkShapeRayCastInput& input, const hkCdBody& body,
                                      hkShapeRayCastCollector& collector) const; // 0x20
    virtual void getSupportingVertex(const hkVector4& direction, hkVector4& out) const; // 0x30
    virtual void convertVertexIdsToVertices(const void* ids, int numIds, hkVector4* out) const; // 0x34 HYPOTHESIS
    virtual void getFirstVertex(hkVector4& out) const;                       // 0x38
    virtual int getNumVertices() const;                                      // 0x3C
};
