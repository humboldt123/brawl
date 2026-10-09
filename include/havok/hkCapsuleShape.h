#pragma once

#include <havok/hkConvexShape.h>
#include <havok/hkMemory.h>
#include <havok/hkWorldCinfo.h>

// Capsule shape (0x30 bytes): segment endpoints vertexA (0x10) and vertexB (0x20), radius from the convex base.
// Layout from hkCapsuleShapeClass.cpp. Overrides most of the hkShape/hkConvexShape virtuals.
struct hkCapsuleShape : hkConvexShape {
    enum RayHitType {
        HIT_CAP0 = 0,
        HIT_CAP1 = 1,
        HIT_BODY = 2,
    };

    hkVector4 m_vertexA; // 0x10
    hkVector4 m_vertexB; // 0x20

    hkCapsuleShape(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: only the vtable is written
    virtual ~hkCapsuleShape();
    HK_DECLARE_REF_ALLOCATOR(0x25)

    static void finishLoadedObjecthkCapsuleShape(void* p);
    static void cleanupLoadedObjecthkCapsuleShape(void* p);
    static const void* getVtablehkCapsuleShape();

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
