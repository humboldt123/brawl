#pragma once

#include <havok/hkConvexShape.h>
#include <havok/hkMemory.h>
#include <havok/hkWorldCinfo.h>

// Box shape (0x20 bytes). Layout from hkBoxShapeClass.cpp: the half extents vector at 0x10.
// Overrides the dtor (0x08), calcStatistics (0x0C), getType (0x10), getAabb (0x14), castRay (0x1C),
// the sphere-rep slots (0x28, 0x2C) and the vertex slots (0x30 to 0x38).
struct hkBoxShape : hkConvexShape {
    hkVector4 m_halfExtents; // 0x10

    hkBoxShape(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: only the vtable is written
    virtual ~hkBoxShape();
    HK_DECLARE_REF_ALLOCATOR(0x25)

    static void finishLoadedObjecthkBoxShape(void* p);
    static void cleanupLoadedObjecthkBoxShape(void* p);
    static const void* getVtablehkBoxShape();

    virtual void calcStatistics(hkStatisticsCollector* collector) const;     // 0x0C
    virtual int getType() const;                                             // 0x10
    virtual void getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const; // 0x14
    virtual hkBool castRay(const hkShapeRayCastInput& input, hkVector4& normalOut) const; // 0x1C
    virtual void getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const; // 0x28
    virtual hkVector4* getCollisionSpheres(hkVector4* out) const;            // 0x2C
    virtual void getSupportingVertex(const hkVector4& direction, hkVector4& out) const; // 0x30
    virtual void convertVertexIdsToVertices(const void* ids, int numIds, hkVector4* out) const; // 0x34
    virtual void getFirstVertex(hkVector4& out) const;                       // 0x38
};
