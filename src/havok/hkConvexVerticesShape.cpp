// Havok translation unit hkConvexVerticesShape.o (main.dol 0x802D1588-0x802D256C).
// Functions in address order:
//   0x802D1588    32  finishLoadedObjecthkConvexVerticesShape   [map: hkConvexVerticesShape__finishLoadedObjecthkConvexVerticesShape]
//   0x802D15A8    20  cleanupLoadedObjecthkConvexVerticesShape   [map: hkConvexVerticesShape__cleanupLoadedObjecthkConvexVerticesShape]
//   0x802D15BC   196  __dt   [map: hkConvexVerticesShape____dt]
//   0x802D1680    60  getVtablehkConvexVerticesShape   [map: hkConvexVerticesShape__getVtablehkConvexVerticesShape]
//   0x802D16BC     8  getType   [map: hkConvexVerticesShape__getType]
//   0x802D16C4    48  getFirstVertex   [map: hkConvexVerticesShape__getFirstVertex]
//   0x802D16F4    20  getCollisionSpheresInfo   [map: hkConvexVerticesShape__getCollisionSpheresInfo]
//   0x802D1708   452  getCollisionSpheres   [map: hkConvexVerticesShape__getCollisionSpheres]
//   0x802D18CC  1004  getAabb   [map: hkConvexVerticesShape__getAabb]
//   0x802D1CB8   800  getSupportingVertex   [map: hkConvexVerticesShape__getSupportingVertex]
//   0x802D1FD8   324  convertVertexIdsToVertices   [map: hkConvexVerticesShape__convertVertexIdsToVertices]
//   0x802D211C   776  castRay   [map: hkConvexVerticesShape__castRay]
//   0x802D2424   244  calcStatistics   [map: hkConvexVerticesShape__calcStatistics]
//   0x802D2518    84  __sinit_\hkConvexVerticesShape_cpp   [map: hkConvexVerticesShapecpp____sinit_]

#include <havok/hkConvexVerticesShape.h>
#include <havok/hkShapeType.h>

#pragma fp_contract on

void hkConvexVerticesShape::finishLoadedObjecthkConvexVerticesShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkConvexVerticesShape(flag);
}

void hkConvexVerticesShape::cleanupLoadedObjecthkConvexVerticesShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkConvexVerticesShape::~hkConvexVerticesShape() {
}

const void* hkConvexVerticesShape::getVtablehkConvexVerticesShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x50] __attribute__((aligned(16)));
    hkConvexVerticesShape* p = ::new (buf) hkConvexVerticesShape(flag);
    return *(const void**)p;
}

int hkConvexVerticesShape::getType() const {
    return HK_SHAPE_CONVEX_VERTICES;
}

void hkConvexVerticesShape::getFirstVertex(hkVector4& out) const {
    const hkConvexVerticesShapeFourVectors* v = (const hkConvexVerticesShapeFourVectors*)m_rotatedVertices.m_data;
    out.x = v->x.x;
    out.y = v->y.x;
    out.z = v->z.x;
    out.w = 1.0f;
}

void hkConvexVerticesShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = m_numVertices;
    out->m_flag = 1;
}

// Not yet decompiled in this unit:
//   0x802D1708   452  getCollisionSpheres   [map: hkConvexVerticesShape__getCollisionSpheres]
//   0x802D18CC  1004  getAabb   [map: hkConvexVerticesShape__getAabb]
//   0x802D1CB8   800  getSupportingVertex   [map: hkConvexVerticesShape__getSupportingVertex]
//   0x802D1FD8   324  convertVertexIdsToVertices   [map: hkConvexVerticesShape__convertVertexIdsToVertices]
//   0x802D211C   776  castRay   [map: hkConvexVerticesShape__castRay]
//   0x802D2424   244  calcStatistics   [map: hkConvexVerticesShape__calcStatistics]
