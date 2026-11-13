// Havok translation unit hkNullCollisionFilter.o (main.dol 0x802CD66C-0x802CD7E8).
// Functions in address order (method names from the Havok TU map):
//   0x802CD66C    92  __dt   [map: hkNullCollisionFilter____dt]
//   0x802CD6C8    64  finishLoadedObjecthkNullCollisionFilter   [map: hkNullCollisionFilter__finishLoadedObjecthkNullCollisionFilter]
//   0x802CD708    20  cleanupLoadedObjecthkNullCollisionFilter   [map: hkNullCollisionFilter__cleanupLoadedObjecthkNullCollisionFilter]
//   0x802CD71C    92  getVtablehkNullCollisionFilter   [map: hkNullCollisionFilter__getVtablehkNullCollisionFilter]
//   0x802CD778     8  isCollisionEnabled   [map: hkNullCollisionFilter__isCollisionEnabled]
//   0x802CD780     8  isCollisionEnabled   [map: hkNullCollisionFilter__isCollisionEnabled1]
//   0x802CD788     8  isCollisionEnabled   [map: hkNullCollisionFilter__isCollisionEnabled2]
//   0x802CD790     8  isCollisionEnabled   [map: hkNullCollisionFilter__isCollisionEnabled3]
//   0x802CD798    80  __sinit_\hkNullCollisionFilter_cpp   [map: hkNullCollisionFiltercpp____sinit_]

#include <havok/hkNullCollisionFilter.h>
#include <havok/hkRegistry.h>
#include <types.h>

hkNullCollisionFilter::~hkNullCollisionFilter() {}

void hkNullCollisionFilter::finishLoadedObjecthkNullCollisionFilter(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkNullCollisionFilter(flag);
}

void hkNullCollisionFilter::cleanupLoadedObjecthkNullCollisionFilter(void* p) {
    ((hkNullCollisionFilter*)p)->~hkNullCollisionFilter();
}

#pragma auto_inline off
const void* hkNullCollisionFilter::getVtablehkNullCollisionFilter() {
    // Temporary object on the stack; only its vtable pointer is read back.
    __attribute__((aligned(16))) char buf[0x18];
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    ::new (buf) hkNullCollisionFilter(flag);
    return *(const void**)buf;
}
#pragma auto_inline reset

// Defined in reverse vtable order (the thunk targets give the order).
hkBool hkNullCollisionFilter::isCollisionEnabled(const hkWorldRayCastInput& a, const hkCollidable& collidableB) const {
    return true;
}

hkBool hkNullCollisionFilter::isCollisionEnabled(const hkShapeRayCastInput& aInput, const hkShapeCollection& bContainer,
                                                 u32 bKey) const {
    return true;
}

hkBool hkNullCollisionFilter::isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b,
                                                 const hkShapeCollection& bContainer, u32 bKey) const {
    return true;
}

hkBool hkNullCollisionFilter::isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const {
    return true;
}

static hkStaticTypeInfo hkNullCollisionFilterTypeInfo("hkNullCollisionFilter", hkNullCollisionFilter::finishLoadedObjecthkNullCollisionFilter,
                                                    hkNullCollisionFilter::cleanupLoadedObjecthkNullCollisionFilter,
                                                    hkNullCollisionFilter::getVtablehkNullCollisionFilter());
