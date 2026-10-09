#pragma once

#include <havok/hkConvexShape.h>
#include <havok/hkMemory.h>
#include <havok/hkWorldCinfo.h>

// Cylinder shape (0x60 bytes). Layout from hkCylinderShapeClass.cpp: cylinder radius at 0x10, height-field
// base radius factor at 0x14, cylinder axis vertices at 0x20 and 0x30, perpendicular axes at 0x40 and 0x50.
struct hkCylinderShape : hkConvexShape {
    hkReal m_cylRadius;                    // 0x10
    hkReal m_cylBaseRadiusFactorForHeightField; // 0x14
    hkVector4 m_vertexA;                   // 0x20
    hkVector4 m_vertexB;                   // 0x30
    hkVector4 m_perpendicular1;            // 0x40
    hkVector4 m_perpendicular2;            // 0x50

    hkCylinderShape(hkFinishLoadedObjectFlag flag); // finish-loading ctor (body not recovered yet)
    virtual ~hkCylinderShape();
    HK_DECLARE_REF_ALLOCATOR(0x25)

    static void finishLoadedObjecthkCylinderShape(void* p);
    static void cleanupLoadedObjecthkCylinderShape(void* p);
    static const void* getVtablehkCylinderShape();

    hkReal getCylinderRadius() const;

    virtual void calcStatistics(hkStatisticsCollector* collector) const;     // 0x0C
    virtual int getType() const;                                             // 0x10
    virtual void getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const; // 0x14
    virtual hkBool castRay(const hkShapeRayCastInput& input, hkVector4& normalOut) const; // 0x1C
    virtual void getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const; // 0x28
    virtual hkVector4* getCollisionSpheres(hkVector4* out) const;            // 0x2C
    virtual void getSupportingVertex(const hkVector4& direction, hkVector4& out) const; // 0x30
    virtual void convertVertexIdsToVertices(const void* ids, int numIds, hkVector4* out) const; // 0x34
    virtual void getFirstVertex(hkVector4& out) const;                       // 0x38
    virtual int getNumVertices() const;                                      // 0x3C
};
