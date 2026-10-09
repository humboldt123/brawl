// Havok translation unit hkFastMeshShape.o (main.dol 0x802D46F4-0x802D4A94).
// Functions in address order:
//   0x802D46F4    84  finishLoadedObjecthkFastMeshShape   [map: hkFastMeshShape__finishLoadedObjecthkFastMeshShape]
//   0x802D4748   152  __dt   [map: hkMeshShape____dt]
//   0x802D47E0    20  cleanupLoadedObjecthkFastMeshShape   [map: hkFastMeshShape__cleanupLoadedObjecthkFastMeshShape]
//   0x802D47F4   156  __dt   [map: hkFastMeshShape____dt]
//   0x802D4890    92  getVtablehkFastMeshShape   [map: hkFastMeshShape__getVtablehkFastMeshShape]
//   0x802D48EC   344  getChildShape   [map: hkFastMeshShape__getChildShape]
//   0x802D4A44    80  __sinit_\hkFastMeshShape_cpp   [map: hkFastMeshShapecpp____sinit_]

#include <havok/hkFastMeshShape.h>
#include <havok/hkShapeType.h>

#pragma fp_contract on

void hkFastMeshShape::finishLoadedObjecthkFastMeshShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkFastMeshShape(flag);
}

void hkFastMeshShape::cleanupLoadedObjecthkFastMeshShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

// The hkMeshShape destructor is emitted in this translation unit (the original has it here, not in hkMeshShape.o).
hkMeshShape::~hkMeshShape() {
}

hkFastMeshShape::~hkFastMeshShape() {
}

const void* hkFastMeshShape::getVtablehkFastMeshShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x50] __attribute__((aligned(16)));
    hkFastMeshShape* p = ::new (buf) hkFastMeshShape(flag);
    return *(const void**)p;
}

// Not yet decompiled in this unit:
//   0x802D48EC   344  getChildShape   [map: hkFastMeshShape__getChildShape]
