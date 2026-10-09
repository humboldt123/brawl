// Havok translation unit hkShapeContainer.o (main.dol 0x802D4CE4-0x802D4E0C).
// Functions in address order:
//   0x802D4CE4    24  finishLoadedObjecthkSingleShapeContainer   [map: hkShapeContainer__finishLoadedObjecthkSingleShapeContainer]
//   0x802D4CFC    20  cleanupLoadedObjecthkSingleShapeContainer   [map: hkShapeContainer__cleanupLoadedObjecthkSingleShapeContainer]
//   0x802D4D10    52  getVtablehkSingleShapeContainer   [map: hkShapeContainer__getVtablehkSingleShapeContainer]
//   0x802D4D44   120  getNumChildShapes   [map: hkShapeContainer__getNumChildShapes]
//   0x802D4DBC    80  __sinit_\hkShapeContainer_cpp   [map: hkShapeContainercpp____sinit_]

#include <havok/hkShapeContainer.h>

#pragma fp_contract on

void hkSingleShapeContainer::finishLoadedObjecthkSingleShapeContainer(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkSingleShapeContainer(flag);
}

void hkSingleShapeContainer::cleanupLoadedObjecthkSingleShapeContainer(void* p) {
    ((hkShapeContainer*)p)->~hkShapeContainer();
}

const void* hkSingleShapeContainer::getVtablehkSingleShapeContainer() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[8] __attribute__((aligned(16)));
    hkSingleShapeContainer* p = ::new (buf) hkSingleShapeContainer(flag);
    return *(const void**)p;
}

int hkShapeContainer::getNumChildShapes() const {
    int count = 0;
    for (int key = getFirstKey(); key != -1; key = getNextKey(key)) {
        count++;
    }
    return count;
}
