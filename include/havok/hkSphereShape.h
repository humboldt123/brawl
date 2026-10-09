#pragma once

#include <havok/hkConvexShape.h>

#include <havok/hkWorldCinfo.h>

// Sphere shape (0x10 bytes, radius at 0x0C from the convex base). Layout from hkSphereShapeClass.cpp.
// Vtable is 0x40 bytes (16 slots; the hkTriangleShape vtable follows it).
struct hkSphereShape : hkConvexShape {
    hkSphereShape(hkFinishLoadedObjectFlag flag) {} // finish-loading ctor: base ctors set refcount and the vtable
    hkSphereShape(hkReal radius);

    static void finishLoadedObjecthkSphereShape(void* p);
    static void cleanupLoadedObjecthkSphereShape(void* p);
    static const void* getVtablehkSphereShape();

    virtual void calcStatistics(hkStatisticsCollector* collector) const;   // 0x0C
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
