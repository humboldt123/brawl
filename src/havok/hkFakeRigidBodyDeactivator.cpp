// Havok translation unit hkFakeRigidBodyDeactivator.o (main.dol 0x802E2094-0x802E22C4).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E2094    32  finishLoadedObjecthkFakeRigidBodyDeactivator   [map: hkFakeRigidBodyDeactivator__finishLoadedObjecthkFakeRigidBodyDeactivator]
//   0x802E20B4    92  __dt   [map: hkEntityDeactivator____dt]
//   0x802E2110    92  __dt   [map: hkRigidBodyDeactivator____dt]
//   0x802E216C    20  cleanupLoadedObjecthkFakeRigidBodyDeactivator   [map: hkFakeRigidBodyDeactivator__cleanupLoadedObjecthkFakeRigidBodyDeactivator]
//   0x802E2180    92  __dt   [map: hkFakeRigidBodyDeactivator____dt]
//   0x802E21DC    60  getVtablehkFakeRigidBodyDeactivator   [map: hkFakeRigidBodyDeactivator__getVtablehkFakeRigidBodyDeactivator]
//   0x802E2218     8  getRigidBodyDeactivatorType   [map: hkFakeRigidBodyDeactivator__getRigidBodyDeactivatorType]
//   0x802E2220     8  shouldDeactivateHighFrequency   [map: hkFakeRigidBodyDeactivator__shouldDeactivateHighFrequency]
//   0x802E2228     8  shouldDeactivateLowFrequency   [map: hkFakeRigidBodyDeactivator__shouldDeactivateLowFrequency]
//   0x802E2230   148  __sinit_\hkFakeRigidBodyDeactivator_cpp   [map: hkFakeRigidBodyDeactivatorcpp____sinit_]
#include <new>
#include <havok/hkFakeRigidBodyDeactivator.h>
#include <havok/hkVector4.h>

static hkRigidBodyDeactivatorTypeInfo s_hkFakeRigidBodyDeactivatorTypeInfo("hkFakeRigidBodyDeactivator",
    hkFakeRigidBodyDeactivator::finishLoadedObjecthkFakeRigidBodyDeactivator,
    hkFakeRigidBodyDeactivator::cleanupLoadedObjecthkFakeRigidBodyDeactivator,
    hkFakeRigidBodyDeactivator::getVtablehkFakeRigidBodyDeactivator());

static hkFakeRigidBodyDeactivator s_hkFakeRigidBodyDeactivator;

void hkFakeRigidBodyDeactivator::finishLoadedObjecthkFakeRigidBodyDeactivator(void* p) {
    new (p) hkFakeRigidBodyDeactivator(hkFinishLoadedObjectFlag());
}

hkEntityDeactivator::~hkEntityDeactivator() {}

hkRigidBodyDeactivator::~hkRigidBodyDeactivator() {}

void hkFakeRigidBodyDeactivator::cleanupLoadedObjecthkFakeRigidBodyDeactivator(void* p) {
    ((hkFakeRigidBodyDeactivator*)p)->~hkFakeRigidBodyDeactivator();
}

hkFakeRigidBodyDeactivator::~hkFakeRigidBodyDeactivator() {}

const void* hkFakeRigidBodyDeactivator::getVtablehkFakeRigidBodyDeactivator() {
    hkVector4 buf[1]; // placed object storage (16-byte aligned)
    new (buf) hkFakeRigidBodyDeactivator(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

int hkFakeRigidBodyDeactivator::getRigidBodyDeactivatorType() const {
    return DEACTIVATOR_NEVER;
}

hkBool hkFakeRigidBodyDeactivator::shouldDeactivateHighFrequency() const {
    return hkBool(false);
}

hkBool hkFakeRigidBodyDeactivator::shouldDeactivateLowFrequency() const {
    return hkBool(false);
}
