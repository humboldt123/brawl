// Havok translation unit hkMeshShape.o (main.dol 0x802D51F4-0x802D5CAC).
// Functions in address order:
//   0x802D51F4    52  finishLoadedObjecthkMeshShape   [map: hkMeshShape__finishLoadedObjecthkMeshShape]
//   0x802D5228    20  cleanupLoadedObjecthkMeshShape   [map: hkMeshShape__cleanupLoadedObjecthkMeshShape]
//   0x802D523C    72  getVtablehkMeshShape   [map: hkMeshShape__getVtablehkMeshShape]
//   0x802D5284   112  __ct   [map: hkMeshShape____ct]
//   0x802D52F4     8  getType   [map: hkMeshShape__getType]
//   0x802D52FC   168  getFirstKey   [map: hkMeshShape__getFirstKey]
//   0x802D53A4   252  getNextKey   [map: hkMeshShape__getNextKey]
//   0x802D54A0   508  getChildShape   [map: hkMeshShape__getChildShape]
//   0x802D569C   152  getCollisionFilterInfo   [map: hkMeshShape__getCollisionFilterInfo]
//   0x802D5734   348  addToAabb   [map: hkAabb__addToAabb]
//   0x802D5890   544  getAabb   [map: hkMeshShape__getAabb]
//   0x802D5AB0   244  addSubpart   [map: hkMeshShape__addSubpart]
//   0x802D5BA4   180  calcStatistics   [map: hkMeshShape__calcStatistics]
//   0x802D5C58    84  __sinit_\hkMeshShape_cpp   [map: hkMeshShapecpp____sinit_]

#include <havok/hkMeshShape.h>
#include <havok/hkShapeType.h>

#pragma fp_contract on

void hkMeshShape::finishLoadedObjecthkMeshShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkMeshShape(flag);
}

void hkMeshShape::cleanupLoadedObjecthkMeshShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkMeshShape::~hkMeshShape() {
}

const void* hkMeshShape::getVtablehkMeshShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x50] __attribute__((aligned(16)));
    hkMeshShape* p = ::new (buf) hkMeshShape(flag);
    return *(const void**)p;
}

#pragma dont_inline on
hkMeshShape::hkMeshShape(const hkFinishLoadedObjectFlag& flag) {
    if (flag.m_finishing == 0) {
        return;
    }
    // Subparts loaded without a material index striding type get the default (int8).
    hkMeshShapeSubpart* subparts = (hkMeshShapeSubpart*)m_subparts.m_data;
    for (int i = 0; i < m_subparts.m_size; i++) {
        if (subparts[i].m_materialIndexStridingType == 0) {
            subparts[i].m_materialIndexStridingType = 1;
        }
    }
}

#pragma dont_inline reset

int hkMeshShape::getType() const {
    return HK_SHAPE_TRIANGLE_COLLECTION;
}

// Not yet decompiled in this unit:
//   0x802D5284   112  __ct   [map: hkMeshShape____ct]
//   0x802D52FC   168  getFirstKey   [map: hkMeshShape__getFirstKey]
//   0x802D53A4   252  getNextKey   [map: hkMeshShape__getNextKey]
//   0x802D54A0   508  getChildShape   [map: hkMeshShape__getChildShape]
//   0x802D569C   152  getCollisionFilterInfo   [map: hkMeshShape__getCollisionFilterInfo]
//   0x802D5734   348  addToAabb   [map: hkAabb__addToAabb]
//   0x802D5890   544  getAabb   [map: hkMeshShape__getAabb]
//   0x802D5AB0   244  addSubpart   [map: hkMeshShape__addSubpart]
//   0x802D5BA4   180  calcStatistics   [map: hkMeshShape__calcStatistics]
