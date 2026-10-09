// Havok translation unit hkFixedRigidMotion.o (main.dol 0x802E5258-0x802E5564).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E5258    32  finishLoadedObjecthkFixedRigidMotion   [map: hkFixedRigidMotion__finishLoadedObjecthkFixedRigidMotion]
//   0x802E5278    20  cleanupLoadedObjecthkFixedRigidMotion   [map: hkFixedRigidMotion__cleanupLoadedObjecthkFixedRigidMotion]
//   0x802E528C   100  __dt   [map: hkFixedRigidMotion____dt]
//   0x802E52F0    60  getVtablehkFixedRigidMotion   [map: hkFixedRigidMotion__getVtablehkFixedRigidMotion]
//   0x802E532C    68  __ct   [map: hkFixedRigidMotion____ct]
//   0x802E5370     4  setStepPosition   [map: hkFixedRigidMotion__setStepPosition]
//   0x802E5374   416  getPositionAndVelocities   [map: hkFixedRigidMotion__getPositionAndVelocities]
//   0x802E5514    80  __sinit_\hkFixedRigidMotion_cpp   [map: hkFixedRigidMotioncpp____sinit_]
#include <new>
#include <havok/hkFixedRigidMotion.h>
#include <havok/hkRegistry.h>

static hkMotionTypeInfo s_hkFixedRigidMotionTypeInfo("hkFixedRigidMotion", hkFixedRigidMotion::finishLoadedObjecthkFixedRigidMotion,
                                                     hkFixedRigidMotion::cleanupLoadedObjecthkFixedRigidMotion,
                                                     hkFixedRigidMotion::getVtablehkFixedRigidMotion());

void hkFixedRigidMotion::finishLoadedObjecthkFixedRigidMotion(void* p) {
    new (p) hkFixedRigidMotion(hkFinishLoadedObjectFlag());
}

void hkFixedRigidMotion::cleanupLoadedObjecthkFixedRigidMotion(void* p) {
    ((hkFixedRigidMotion*)p)->~hkFixedRigidMotion();
}

hkFixedRigidMotion::~hkFixedRigidMotion() {}

const void* hkFixedRigidMotion::getVtablehkFixedRigidMotion() {
    hkVector4 buf[17]; // 0x110 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkFixedRigidMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

hkFixedRigidMotion::hkFixedRigidMotion(const hkVector4& position, const hkQuaternion& rotation)
    : hkKeyframedRigidMotion(position, rotation) {
    m_type = MOTION_FIXED;
}

void hkFixedRigidMotion::setStepPosition(hkReal stepPosition) {}

// Copies the motion state to out and clears its linear and angular velocity.
void hkFixedRigidMotion::getPositionAndVelocities(hkMotion* out) const {
    out->m_motionState = m_motionState;
    out->m_linearVelocity.setZero4();
    out->m_angularVelocity.setZero4();
}
