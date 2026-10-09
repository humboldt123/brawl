// Havok translation unit hkBoxShape.o (main.dol 0x802CD884-0x802CE588).
// Functions in address order:
//   0x802CD884    32  finishLoadedObjecthkBoxShape   [map: hkBoxShape__finishLoadedObjecthkBoxShape]
//   0x802CD8A4    20  cleanupLoadedObjecthkBoxShape   [map: hkBoxShape__cleanupLoadedObjecthkBoxShape]
//   0x802CD8B8    92  __dt   [map: hkBoxShape____dt]
//   0x802CD914    60  getVtablehkBoxShape   [map: hkBoxShape__getVtablehkBoxShape]
//   0x802CD950    96  calcStatistics   [map: hkBoxShape__calcStatistics]
//   0x802CD9B0     8  getType   [map: hkBoxShape__getType]
//   0x802CD9B8    36  getFirstVertex   [map: hkBoxShape__getFirstVertex]
//   0x802CD9DC   748  getAabb   [map: hkBoxShape__getAabb]
//   0x802CDCC8   204  getSupportingVertex   [map: hkBoxShape__getSupportingVertex]
//   0x802CDD94   300  convertVertexIdsToVertices   [map: hkBoxShape__convertVertexIdsToVertices]
//   0x802CDEC0    20  getCollisionSpheresInfo   [map: hkBoxShape__getCollisionSpheresInfo]
//   0x802CDED4   448  getCollisionSpheres   [map: hkBoxShape__getCollisionSpheres]
//   0x802CE094  1184  castRay   [map: hkBoxShape__castRay]
//   0x802CE534    84  __sinit_\hkBoxShape_cpp   [map: hkBoxShapecpp____sinit_]

#include <havok/hkBoxShape.h>
#include <havok/hkShapeType.h>

#pragma fp_contract on

void hkBoxShape::finishLoadedObjecthkBoxShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkBoxShape(flag);
}

void hkBoxShape::cleanupLoadedObjecthkBoxShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkBoxShape::~hkBoxShape() {
}

const void* hkBoxShape::getVtablehkBoxShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x20] __attribute__((aligned(16)));
    hkBoxShape* p = ::new (buf) hkBoxShape(flag);
    return *(const void**)p;
}

int hkBoxShape::getType() const {
    return HK_SHAPE_BOX;
}

void hkBoxShape::getFirstVertex(hkVector4& out) const {
    out.x = m_halfExtents.x;
    out.y = m_halfExtents.y;
    out.z = m_halfExtents.z;
    out.w = m_halfExtents.w;
}

void hkBoxShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 8;
    out->m_flag = 1;
}

// Not yet decompiled in this unit:
//   0x802CD950    96  calcStatistics   [map: hkBoxShape__calcStatistics]
//   0x802CD9DC   748  getAabb   [map: hkBoxShape__getAabb]
//   0x802CDCC8   204  getSupportingVertex   [map: hkBoxShape__getSupportingVertex]
//   0x802CDD94   300  convertVertexIdsToVertices   [map: hkBoxShape__convertVertexIdsToVertices]
//   0x802CDED4   448  getCollisionSpheres   [map: hkBoxShape__getCollisionSpheres]
//   0x802CE094  1184  castRay   [map: hkBoxShape__castRay]
