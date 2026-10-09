// Havok translation unit hkBvShape.o (main.dol 0x802CE5F0-0x802CEB14).
// Functions in address order:
//   0x802CE5F0    44  finishLoadedObjecthkBvShape   [map: hkBvShape__finishLoadedObjecthkBvShape]
//   0x802CE61C   132  __dt   [map: hkSingleShapeContainer____dt]
//   0x802CE6A0    20  cleanupLoadedObjecthkBvShape   [map: hkBvShape__cleanupLoadedObjecthkBvShape]
//   0x802CE6B4   268  __dt   [map: hkBvShape____dt]
//   0x802CE7C0    72  getVtablehkBvShape   [map: hkBvShape__getVtablehkBvShape]
//   0x802CE808    20  getAabb   [map: hkBvShape__getAabb]
//   0x802CE81C     8  getType   [map: hkBvShape__getType]
//   0x802CE824   260  castRay   [map: hkBvShape__castRay]
//   0x802CE928   216  castRayWithCollector   [map: hkBvShape__castRayWithCollector]
//   0x802CEA00   152  calcStatistics   [map: hkBvShape__calcStatistics]
//   0x802CEA98     8  getContainer   [map: hkBvShape__getContainer]
//   0x802CEAA0     8  getChildShape   [map: hkSingleShapeContainer__getChildShape]
//   0x802CEAA8     8  getNextKey   [map: hkSingleShapeContainer__getNextKey]
//   0x802CEAB0     8  getFirstKey   [map: hkSingleShapeContainer__getFirstKey]
//   0x802CEAB8     8  getNumChildShapes   [map: hkSingleShapeContainer__getNumChildShapes]
//   0x802CEAC0    84  __sinit_\hkBvShape_cpp   [map: hkBvShapecpp____sinit_]

#include <havok/hkBvShape.h>
#include <havok/hkShapeType.h>

#pragma fp_contract on

void hkBvShape::finishLoadedObjecthkBvShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkBvShape(flag);
}

void hkBvShape::cleanupLoadedObjecthkBvShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkBvShape::~hkBvShape() {
    if (m_boundingVolumeShape) {
        m_boundingVolumeShape->removeReference();
    }
}

const void* hkBvShape::getVtablehkBvShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x18] __attribute__((aligned(16)));
    hkBvShape* p = ::new (buf) hkBvShape(flag);
    return *(const void**)p;
}

void hkBvShape::getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const {
    m_boundingVolumeShape->getAabb(xf, expansion, out);
}

int hkBvShape::getType() const {
    return HK_SHAPE_BV;
}

hkShapeContainer* hkBvShape::getContainer() const {
    return const_cast<hkSingleShapeContainer*>(&m_childContainer);
}

hkShape* hkSingleShapeContainer::getChildShape() const {
    return m_childShape;
}

int hkSingleShapeContainer::getNextKey(int key) const {
    return -1;
}

int hkSingleShapeContainer::getFirstKey() const {
    return 0;
}

int hkSingleShapeContainer::getNumChildShapes() const {
    return 1;
}

// Not yet decompiled in this unit:
//   0x802CE61C   132  __dt   [map: hkSingleShapeContainer____dt]
//   0x802CE824   260  castRay   [map: hkBvShape__castRay]
//   0x802CE928   216  castRayWithCollector   [map: hkBvShape__castRayWithCollector]
//   0x802CEA00   152  calcStatistics   [map: hkBvShape__calcStatistics]
