// Havok translation unit hkArrayAction.o (main.dol 0x802D75E0-0x802D766C).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802D75E0   140  __dt   [map: hkArray_P8hkEntity_____dt]
#include <havok/hkArray.h>
#include <havok/hkString.h>

struct hkEntity;

// Deleting destructor of hkArray<hkEntity*>. Deleting through the pointer emits the destructor
// with its operator delete path.
void hkArray_hkEntity_deletingDtor(hkArray<hkEntity*>* array) {
    delete array;
}
