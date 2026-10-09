// Havok translation unit hkCapsuleShape.o (main.dol 0x802CEB14-0x802D0144).
// Functions in address order:
//   0x802CEB14    32  finishLoadedObjecthkCapsuleShape   [map: hkCapsuleShape__finishLoadedObjecthkCapsuleShape]
//   0x802CEB34    20  cleanupLoadedObjecthkCapsuleShape   [map: hkCapsuleShape__cleanupLoadedObjecthkCapsuleShape]
//   0x802CEB48    92  __dt   [map: hkCapsuleShape____dt]
//   0x802CEBA4    60  getVtablehkCapsuleShape   [map: hkCapsuleShape__getVtablehkCapsuleShape]
//   0x802CEBE0   136  __ct   [map: hkCapsuleShape____ct]
//   0x802CEC68   200  getSupportingVertex   [map: hkCapsuleShape__getSupportingVertex]
//   0x802CED30   288  convertVertexIdsToVertices   [map: hkCapsuleShape__convertVertexIdsToVertices]
//   0x802CEE50    36  getFirstVertex   [map: hkCapsuleShape__getFirstVertex]
//   0x802CEE74     8  getNumVertices   [map: hkCapsuleShape__getNumVertices]
//   0x802CEE7C    20  getCollisionSpheresInfo   [map: hkCapsuleShape__getCollisionSpheresInfo]
//   0x802CEE90    72  getCollisionSpheres   [map: hkCapsuleShape__getCollisionSpheres]
//   0x802CEED8  1120  getAabb   [map: hkCapsuleShape__getAabb]
//   0x802CF338   668  closestInfLineSegInfLineSeg   [map: hkCapsuleShape__closestInfLineSegInfLineSeg]
//   0x802CF5D4   352  closestPointLineSeg   [map: hkCapsuleShape__closestPointLineSeg]
//   0x802CF734  2384  castRay   [map: hkCapsuleShape__castRay]
//   0x802D0084     8  getType   [map: hkCapsuleShape__getType]
//   0x802D008C   100  calcStatistics   [map: hkCapsuleShape__calcStatistics]
//   0x802D00F0    84  __sinit_\hkCapsuleShape_cpp   [map: hkCapsuleShapecpp____sinit_]

#include <havok/hkCapsuleShape.h>
#include <havok/hkShapeType.h>

#pragma fp_contract on

void hkCapsuleShape::finishLoadedObjecthkCapsuleShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkCapsuleShape(flag);
}

void hkCapsuleShape::cleanupLoadedObjecthkCapsuleShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkCapsuleShape::~hkCapsuleShape() {
}

const void* hkCapsuleShape::getVtablehkCapsuleShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x30] __attribute__((aligned(16)));
    hkCapsuleShape* p = ::new (buf) hkCapsuleShape(flag);
    return *(const void**)p;
}

void hkCapsuleShape::getFirstVertex(hkVector4& out) const {
    out.x = m_vertexB.x;
    out.y = m_vertexB.y;
    out.z = m_vertexB.z;
    out.w = m_vertexB.w;
}

int hkCapsuleShape::getNumVertices() const {
    return 2;
}

void hkCapsuleShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 2;
    out->m_flag = 1;
}

int hkCapsuleShape::getType() const {
    return HK_SHAPE_CAPSULE;
}

// Not yet decompiled in this unit:
//   0x802CEBE0   136  __ct (vertexA, vertexB, radius)   [map: hkCapsuleShape____ct]
//   0x802CEC68   200  getSupportingVertex   [map: hkCapsuleShape__getSupportingVertex]
//   0x802CED30   288  convertVertexIdsToVertices   [map: hkCapsuleShape__convertVertexIdsToVertices]
//   0x802CEE90    72  getCollisionSpheres   [map: hkCapsuleShape__getCollisionSpheres]
//   0x802CEED8  1120  getAabb   [map: hkCapsuleShape__getAabb]
//   0x802CF338   668  closestInfLineSegInfLineSeg   [map: hkCapsuleShape__closestInfLineSegInfLineSeg]
//   0x802CF5D4   352  closestPointLineSeg   [map: hkCapsuleShape__closestPointLineSeg]
//   0x802CF734  2384  castRay   [map: hkCapsuleShape__castRay]
//   0x802D008C   100  calcStatistics   [map: hkCapsuleShape__calcStatistics]
