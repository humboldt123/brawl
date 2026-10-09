#pragma once

#include <havok/hkConvexShape.h>
#include <havok/hkArray.h>
#include <havok/hkShapeContainer.h>
#include <havok/hkWorldCinfo.h>

// Four vertices in structure-of-arrays form (0x30 bytes): the x, y and z lanes of four vertices each.
struct hkConvexVerticesShapeFourVectors {
    hkVector4 x; // 0x00
    hkVector4 y; // 0x10
    hkVector4 z; // 0x20
};

// Convex hull given by its vertices (0x50 bytes). Layout from hkConvexVerticesShapeClass.cpp.
struct hkConvexVerticesShape : hkConvexShape {
    hkVector4 m_aabbHalfExtents;                                     // 0x10
    hkVector4 m_aabbCenter;                                          // 0x20
    hkArray<hkConvexVerticesShapeFourVectors> m_rotatedVertices;     // 0x30
    int m_numVertices;                                               // 0x3C
    hkArray<hkVector4> m_planeEquations;                             // 0x40

    hkConvexVerticesShape(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: only the vtable is written
    virtual ~hkConvexVerticesShape();

    static void finishLoadedObjecthkConvexVerticesShape(void* p);
    static void cleanupLoadedObjecthkConvexVerticesShape(void* p);
    static const void* getVtablehkConvexVerticesShape();

    virtual void calcStatistics(hkStatisticsCollector* collector) const;                 // 0x0C
    virtual int getType() const;                                                         // 0x10
    virtual void getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const;    // 0x14
    virtual hkBool castRay(const hkShapeRayCastInput& input, hkVector4& normalOut) const; // 0x1C
    virtual void getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const;            // 0x28
    virtual hkVector4* getCollisionSpheres(hkVector4* out) const;                       // 0x2C
    virtual void getSupportingVertex(const hkVector4& direction, hkVector4& out) const; // 0x30
    virtual void convertVertexIdsToVertices(const void* ids, int numIds, hkVector4* out) const; // 0x34
    virtual void getFirstVertex(hkVector4& out) const;                                  // 0x38
};
