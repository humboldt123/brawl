// Havok translation unit hkSpatialRigidBodyDeactivator.o (main.dol 0x802E37E0-0x802E3C18).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E37E0    32  finishLoadedObjecthkSpatialRigidBodyDeactivator   [map: hkSpatialRigidBodyDeactivator__finishLoadedObjecthkSpatialRigidBodyDeactivator]
//   0x802E3800    20  cleanupLoadedObjecthkSpatialRigidBodyDeactivator   [map: hkSpatialRigidBodyDeactivator__cleanupLoadedObjecthkSpatialRigidBodyDeactivator]
//   0x802E3814    92  __dt   [map: hkSpatialRigidBodyDeactivator____dt]
//   0x802E3870    60  getVtablehkSpatialRigidBodyDeactivator   [map: hkSpatialRigidBodyDeactivator__getVtablehkSpatialRigidBodyDeactivator]
//   0x802E38AC   164  __ct   [map: hkSpatialRigidBodyDeactivator____ct]
//   0x802E3950     8  getRigidBodyDeactivatorType   [map: hkSpatialRigidBodyDeactivator__getRigidBodyDeactivatorType]
//   0x802E3958   312  shouldDeactivateHighFrequency   [map: hkSpatialRigidBodyDeactivator__shouldDeactivateHighFrequency]
//   0x802E3A90   312  shouldDeactivateLowFrequency   [map: hkSpatialRigidBodyDeactivator__shouldDeactivateLowFrequency]
//   0x802E3BC8    80  __sinit_\hkSpatialRigidBodyDeactivator_cpp   [map: hkSpatialRigidBodyDeactivatorcpp____sinit_]
#include <havok/hkSpatialRigidBodyDeactivator.h>

// The finish, getVtable and static-init functions (0x802E37E0, 0x802E3870, 0x802E3BC8) and the constructor
// (0x802E38AC) need a concrete instance. They wait for the two high/low frequency functions, see below.

void hkSpatialRigidBodyDeactivator::cleanupLoadedObjecthkSpatialRigidBodyDeactivator(void* p) {
    ((hkSpatialRigidBodyDeactivator*)p)->~hkSpatialRigidBodyDeactivator();
}

hkSpatialRigidBodyDeactivator::~hkSpatialRigidBodyDeactivator() {}

int hkSpatialRigidBodyDeactivator::getRigidBodyDeactivatorType() const {
    return DEACTIVATOR_SPATIAL;
}

// Not yet decompiled in this unit:
//   0x802E3958  shouldDeactivateHighFrequency: the argument type (fields read at +0x100..+0x150) is not known.
//   0x802E3A90  shouldDeactivateLowFrequency: same.
