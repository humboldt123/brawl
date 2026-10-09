#pragma once

#include <havok/hkConvexShape.h>
#include <havok/hkMemory.h>
#include <havok/hkWorldCinfo.h>

// Triangle shape (0x40 bytes): three vertices at 0x10, 0x20 and 0x30. Layout inferred from the accessors
// (hkTriangleShapeClass.cpp does not exist yet). Overrides most hkShape/hkConvexShape virtuals.
struct hkTriangleShape : hkConvexShape {
    hkVector4 m_vertexA; // 0x10
    hkVector4 m_vertexB; // 0x20
    hkVector4 m_vertexC; // 0x30 HYPOTHESIS

    hkTriangleShape(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: only the vtable is written
    virtual ~hkTriangleShape();
    HK_DECLARE_REF_ALLOCATOR(0x25)

    static void finishLoadedObjecthkTriangleShape(void* p);
    static void cleanupLoadedObjecthkTriangleShape(void* p);
    static const void* getVtablehkTriangleShape();

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
