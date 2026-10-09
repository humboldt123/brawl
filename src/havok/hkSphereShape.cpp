// Havok translation unit hkSphereShape.o (main.dol 0x802D5D88-0x802D6454).
// Functions in address order:
//   0x802D5D88    32  finishLoadedObjecthkSphereShape   [map: hkSphereShape__finishLoadedObjecthkSphereShape]
//   0x802D5DA8    20  cleanupLoadedObjecthkSphereShape   [map: hkSphereShape__cleanupLoadedObjecthkSphereShape]
//   0x802D5DBC    60  getVtablehkSphereShape   [map: hkSphereShape__getVtablehkSphereShape]
//   0x802D5DF8    36  __ct   [map: hkSphereShape____ct]
//   0x802D5E1C     8  getType   [map: hkSphereShape__getType]
//   0x802D5E24    24  getSupportingVertex   [map: hkSphereShape__getSupportingVertex]
//   0x802D5E3C   204  convertVertexIdsToVertices   [map: hkSphereShape__convertVertexIdsToVertices]
//   0x802D5F08    24  getFirstVertex   [map: hkSphereShape__getFirstVertex]
//   0x802D5F20     8  getNumVertices   [map: hkSphereShape__getNumVertices]
//   0x802D5F28   132  getAabb   [map: hkSphereShape__getAabb]
//   0x802D5FAC    16  getCollisionSpheresInfo   [map: hkSphereShape__getCollisionSpheresInfo]
//   0x802D5FBC    72  getCollisionSpheres   [map: hkSphereShape__getCollisionSpheres]
//   0x802D6004   920  castRay   [map: hkSphereShape__castRay]
//   0x802D639C   100  calcStatistics   [map: hkSphereShape__calcStatistics]
//   0x802D6400    84  __sinit_\hkSphereShape_cpp   [map: hkSphereShapecpp____sinit_]

#include <havok/hkSphereShape.h>
#include <havok/hkShapeType.h>
#include <havok/hkWorldCinfo.h>

#pragma fp_contract on

void hkSphereShape::finishLoadedObjecthkSphereShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkSphereShape(flag);
}

void hkSphereShape::cleanupLoadedObjecthkSphereShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

const void* hkSphereShape::getVtablehkSphereShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x10] __attribute__((aligned(16)));
    hkSphereShape* p = ::new (buf) hkSphereShape(flag);
    return *(const void**)p;
}

hkSphereShape::hkSphereShape(hkReal radius) {
    m_radius = radius;
    m_userData = 0;
}

int hkSphereShape::getType() const {
    return HK_SHAPE_SPHERE;
}

void hkSphereShape::getSupportingVertex(const hkVector4& direction, hkVector4& out) const {
    out.w = 0.0f;
    out.z = 0.0f;
    out.y = 0.0f;
    out.x = 0.0f;
}

// Every requested vertex is the sphere centre (the origin). The asm is an unrolled zero fill of numIds vectors.
void hkSphereShape::convertVertexIdsToVertices(const void* ids, int numIds, hkVector4* out) const {
    for (int i = 0; i < numIds; i++) {
        out->w = 0.0f;
        out->z = 0.0f;
        out->y = 0.0f;
        out->x = 0.0f;
        out++;
    }
}

void hkSphereShape::getFirstVertex(hkVector4& out) const {
    out.w = 0.0f;
    out.z = 0.0f;
    out.y = 0.0f;
    out.x = 0.0f;
}

int hkSphereShape::getNumVertices() const {
    return 1;
}

void hkSphereShape::getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const {
    hkReal r = expansion + m_radius;
    hkVector4 extent;
    extent.set(r, r, r, r);
    out.m_min.setSub4(xf.m_translation, extent);
    out.m_max.setAdd4(xf.m_translation, extent);
}

void hkSphereShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 1;
    out->m_flag = 1;
}

hkVector4* hkSphereShape::getCollisionSpheres(hkVector4* out) const {
    hkVector4 sphere;
    sphere.set(0.0f, 0.0f, 0.0f, m_radius);
    *out = sphere;
    return out;
}

// Not yet decompiled in this unit:
//   0x802D6004   920  castRay   [map: hkSphereShape__castRay]
//   0x802D639C   100  calcStatistics   [map: hkSphereShape__calcStatistics]
