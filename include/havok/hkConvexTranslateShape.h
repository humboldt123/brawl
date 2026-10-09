#pragma once

#include <havok/hkConvexShape.h>
#include <havok/hkShapeContainer.h>

// Convex shape with a translation applied to a child convex shape (0x30 bytes). Layout from
// hkConvexTranslateShapeClass.cpp: the child container (a hkSingleShapeContainer, vtable at 0x10,
// child pointer at 0x14) and the translation vector at 0x20.
struct hkConvexTranslateShape : hkConvexShape {
    hkSingleShapeContainer m_childShape; // 0x10
    hkVector4 m_translation;             // 0x20

    hkConvexTranslateShape(hkFinishLoadedObjectFlag flag) : m_childShape(flag) {} // finish-loading ctor
    virtual ~hkConvexTranslateShape();

    static void finishLoadedObjecthkConvexTranslateShape(void* p);
    static void cleanupLoadedObjecthkConvexTranslateShape(void* p);
    static const void* getVtablehkConvexTranslateShape();

    virtual void calcStatistics(hkStatisticsCollector* collector) const;                 // 0x0C
    virtual int getType() const;                                                         // 0x10
    virtual void getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const;    // 0x14
    virtual hkReal getMaximumProjection(const hkVector4& direction) const;               // 0x18
    virtual hkBool castRay(const hkShapeRayCastInput& input, hkVector4& normalOut) const; // 0x1C
    virtual void castRayWithCollector(const hkShapeRayCastInput& input, const hkCdBody& body,
                                      hkShapeRayCastCollector& collector) const;         // 0x20
    virtual void getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const;            // 0x28
    virtual hkVector4* getCollisionSpheres(hkVector4* out) const;                       // 0x2C
    virtual void getSupportingVertex(const hkVector4& direction, hkVector4& out) const; // 0x30
    virtual void convertVertexIdsToVertices(const void* ids, int numIds, hkVector4* out) const; // 0x34
    virtual void getFirstVertex(hkVector4& out) const;                                  // 0x38
};
