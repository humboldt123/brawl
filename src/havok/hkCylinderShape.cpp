// Havok translation unit hkCylinderShape.o (main.dol 0x802D2628-0x802D4688).
// Functions in address order:
//   0x802D2628    52  finishLoadedObjecthkCylinderShape   [map: hkCylinderShape__finishLoadedObjecthkCylinderShape]
//   0x802D265C    20  cleanupLoadedObjecthkCylinderShape   [map: hkCylinderShape__cleanupLoadedObjecthkCylinderShape]
//   0x802D2670    92  __dt   [map: hkCylinderShape____dt]
//   0x802D26CC    72  getVtablehkCylinderShape   [map: hkCylinderShape__getVtablehkCylinderShape]
//   0x802D2714   132  minValueRoundedUpTo1   [map: hkCylinderShape__minValueRoundedUpTo1]
//   0x802D2798     4  assertRoundUpThreshold   [map: hkCylinderShape__assertRoundUpThreshold]
//   0x802D279C    96  __ct   [map: hkCylinderShape____ct]
//   0x802D27FC     8  getCylinderRadius   [map: hkCylinderShape__getCylinderRadius]
//   0x802D2804  1080  getSupportingVertex   [map: hkCylinderShape__getSupportingVertex]
//   0x802D2C3C   740  convertVertexIdsToVertices   [map: hkCylinderShape__convertVertexIdsToVertices]
//   0x802D2F20    36  getFirstVertex   [map: hkCylinderShape__getFirstVertex]
//   0x802D2F44     8  getNumVertices   [map: hkCylinderShape__getNumVertices]
//   0x802D2F4C    20  getCollisionSpheresInfo   [map: hkCylinderShape__getCollisionSpheresInfo]
//   0x802D2F60  1304  getCollisionSpheres   [map: hkCylinderShape__getCollisionSpheres]
//   0x802D3478  1720  getAabb   [map: hkCylinderShape__getAabb]
//   0x802D3B30  2712  castRay   [map: hkCylinderShape__castRay]
//   0x802D45C8     8  getType   [map: hkCylinderShape__getType]
//   0x802D45D0   100  calcStatistics   [map: hkCylinderShape__calcStatistics]

#include <havok/hkCylinderShape.h>
#include <havok/hkShapeType.h>

#pragma fp_contract on

void hkCylinderShape::finishLoadedObjecthkCylinderShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkCylinderShape(flag);
}

void hkCylinderShape::cleanupLoadedObjecthkCylinderShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkCylinderShape::~hkCylinderShape() {
}

const void* hkCylinderShape::getVtablehkCylinderShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x60] __attribute__((aligned(16)));
    hkCylinderShape* p = ::new (buf) hkCylinderShape(flag);
    return *(const void**)p;
}

hkReal hkCylinderShape::getCylinderRadius() const {
    return m_cylRadius;
}

void hkCylinderShape::getFirstVertex(hkVector4& out) const {
    out.x = m_vertexB.x;
    out.y = m_vertexB.y;
    out.z = m_vertexB.z;
    out.w = m_vertexB.w;
}

int hkCylinderShape::getNumVertices() const {
    return -1;
}

void hkCylinderShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 18;
    out->m_flag = 1;
}

int hkCylinderShape::getType() const {
    return HK_SHAPE_CYLINDER;
}

// Not yet decompiled in this unit:
//   0x802D2714   132  minValueRoundedUpTo1   [map: hkCylinderShape__minValueRoundedUpTo1]
//   0x802D2798     4  assertRoundUpThreshold   [map: hkCylinderShape__assertRoundUpThreshold]
//   0x802D279C    96  __ct   [map: hkCylinderShape____ct]
//   0x802D2804  1080  getSupportingVertex   [map: hkCylinderShape__getSupportingVertex]
//   0x802D2C3C   740  convertVertexIdsToVertices   [map: hkCylinderShape__convertVertexIdsToVertices]
//   0x802D2F60  1304  getCollisionSpheres   [map: hkCylinderShape__getCollisionSpheres]
//   0x802D3478  1720  getAabb   [map: hkCylinderShape__getAabb]
//   0x802D3B30  2712  castRay   [map: hkCylinderShape__castRay]
//   0x802D45D0   100  calcStatistics   [map: hkCylinderShape__calcStatistics]
