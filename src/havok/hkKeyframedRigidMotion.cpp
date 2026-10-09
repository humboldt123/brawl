// Havok translation unit hkKeyframedRigidMotion.o (main.dol 0x802E5564-0x802E5A28).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E5564    32  finishLoadedObjecthkKeyframedRigidMotion   [map: hkKeyframedRigidMotion__finishLoadedObjecthkKeyframedRigidMotion]
//   0x802E5584    20  cleanupLoadedObjecthkKeyframedRigidMotion   [map: hkKeyframedRigidMotion__cleanupLoadedObjecthkKeyframedRigidMotion]
//   0x802E5598    60  getVtablehkKeyframedRigidMotion   [map: hkKeyframedRigidMotion__getVtablehkKeyframedRigidMotion]
//   0x802E55D4    32  finishLoadedObjecthkMaxSizeMotion   [map: hkKeyframedRigidMotion__finishLoadedObjecthkMaxSizeMotion]
//   0x802E55F4    20  cleanupLoadedObjecthkMaxSizeMotion   [map: hkKeyframedRigidMotion__cleanupLoadedObjecthkMaxSizeMotion]
//   0x802E5608    60  getVtablehkMaxSizeMotion   [map: hkKeyframedRigidMotion__getVtablehkMaxSizeMotion]
//   0x802E5644   100  __ct   [map: hkKeyframedRigidMotion____ct]
//   0x802E56A8   176  __dt   [map: hkKeyframedRigidMotion____dt]
//   0x802E5758     4  setMass   [map: hkKeyframedRigidMotion__setMass]
//   0x802E575C     4  setMassInv   [map: hkKeyframedRigidMotion__setMassInv]
//   0x802E5760    96  getInertiaLocal   [map: hkKeyframedRigidMotion__getInertiaLocal]
//   0x802E57C0    96  getInertiaWorld   [map: hkKeyframedRigidMotion__getInertiaWorld]
//   0x802E5820     4  setInertiaLocal   [map: hkKeyframedRigidMotion__setInertiaLocal]
//   0x802E5824     4  setInertiaInvLocal   [map: hkKeyframedRigidMotion__setInertiaInvLocal]
//   0x802E5828    96  getInertiaInvLocal   [map: hkKeyframedRigidMotion__getInertiaInvLocal]
//   0x802E5888    96  getInertiaInvWorld   [map: hkKeyframedRigidMotion__getInertiaInvWorld]
//   0x802E58E8     4  applyLinearImpulse   [map: hkKeyframedRigidMotion__applyLinearImpulse]
//   0x802E58EC     4  applyPointImpulse   [map: hkKeyframedRigidMotion__applyPointImpulse]
//   0x802E58F0     4  applyAngularImpulse   [map: hkKeyframedRigidMotion__applyAngularImpulse]
//   0x802E58F4     4  applyForce   [map: hkKeyframedRigidMotion__applyForce]
//   0x802E58F8     4  applyForce   [map: hkKeyframedRigidMotion__applyForce1]
//   0x802E58FC     4  applyTorque   [map: hkKeyframedRigidMotion__applyTorque]
//   0x802E5900     4  setStepPosition   [map: hkKeyframedRigidMotion__setStepPosition]
//   0x802E5904   156  setStoredMotion   [map: hkKeyframedRigidMotion__setStoredMotion]
//   0x802E59A0   136  __sinit_\hkKeyframedRigidMotion_cpp   [map: hkKeyframedRigidMotioncpp____sinit_]
#include <new>
#include <havok/hkKeyframedRigidMotion.h>
#include <havok/hkRegistry.h>

static hkMotionTypeInfo s_hkKeyframedRigidMotionTypeInfo("hkKeyframedRigidMotion",
    hkKeyframedRigidMotion::finishLoadedObjecthkKeyframedRigidMotion,
    hkKeyframedRigidMotion::cleanupLoadedObjecthkKeyframedRigidMotion,
    hkKeyframedRigidMotion::getVtablehkKeyframedRigidMotion());

static hkMotionTypeInfo s_hkMaxSizeMotionTypeInfo("hkMaxSizeMotion",
    hkKeyframedRigidMotion::finishLoadedObjecthkMaxSizeMotion,
    hkKeyframedRigidMotion::cleanupLoadedObjecthkMaxSizeMotion,
    hkKeyframedRigidMotion::getVtablehkMaxSizeMotion());

void hkKeyframedRigidMotion::finishLoadedObjecthkKeyframedRigidMotion(void* p) {
    new (p) hkKeyframedRigidMotion(hkFinishLoadedObjectFlag());
}

void hkKeyframedRigidMotion::cleanupLoadedObjecthkKeyframedRigidMotion(void* p) {
    ((hkKeyframedRigidMotion*)p)->~hkKeyframedRigidMotion();
}

const void* hkKeyframedRigidMotion::getVtablehkKeyframedRigidMotion() {
    hkVector4 buf[17]; // 0x110 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkKeyframedRigidMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

void hkKeyframedRigidMotion::finishLoadedObjecthkMaxSizeMotion(void* p) {
    new (p) hkMaxSizeMotion(hkFinishLoadedObjectFlag());
}

void hkKeyframedRigidMotion::cleanupLoadedObjecthkMaxSizeMotion(void* p) {
    ((hkMaxSizeMotion*)p)->~hkMaxSizeMotion();
}

const void* hkKeyframedRigidMotion::getVtablehkMaxSizeMotion() {
    hkVector4 buf[17]; // 0x110 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkMaxSizeMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

hkKeyframedRigidMotion::hkKeyframedRigidMotion(const hkVector4& position, const hkQuaternion& rotation)
    : hkMotion(position, rotation) {
    savedMotion = 0;
    savedQualityTypeIndex = 0;
    m_inertiaAndMassInv.setZero4();
    m_type = MOTION_KEYFRAMED;
}

hkKeyframedRigidMotion::~hkKeyframedRigidMotion() {
    if (savedMotion) {
        savedMotion->removeReference();
    }
}

void hkKeyframedRigidMotion::setMass(hkReal mass) {}

void hkKeyframedRigidMotion::setMassInv(hkReal massInv) {}

// Writes the 12 floats of a three-column matrix as zero (columns x, y, z, w in order).
// Inertia is not tracked for keyframed bodies: the matrices are returned as zero.
void hkKeyframedRigidMotion::getInertiaLocal(hkMatrix3& out) const {
    hkVector4* cols = (hkVector4*)&out;
    cols[0].x = 0.0f;
    cols[0].y = 0.0f;
    cols[0].z = 0.0f;
    cols[0].w = 0.0f;
    cols[1].x = 0.0f;
    cols[1].y = 0.0f;
    cols[1].z = 0.0f;
    cols[1].w = 0.0f;
    cols[2].x = 0.0f;
    cols[2].y = 0.0f;
    cols[2].z = 0.0f;
    cols[2].w = 0.0f;
    // MATCH-ONLY: the original also leaves a zeroed stack vector behind (stored after the columns).
    hkVector4 zero;
    zero.w = 0.0f;
    zero.z = 0.0f;
    zero.y = 0.0f;
    zero.x = 0.0f;
}

void hkKeyframedRigidMotion::getInertiaWorld(hkMatrix3& out) const {
    hkVector4* cols = (hkVector4*)&out;
    cols[0].x = 0.0f;
    cols[0].y = 0.0f;
    cols[0].z = 0.0f;
    cols[0].w = 0.0f;
    cols[1].x = 0.0f;
    cols[1].y = 0.0f;
    cols[1].z = 0.0f;
    cols[1].w = 0.0f;
    cols[2].x = 0.0f;
    cols[2].y = 0.0f;
    cols[2].z = 0.0f;
    cols[2].w = 0.0f;
    // MATCH-ONLY: the original also leaves a zeroed stack vector behind (stored after the columns).
    hkVector4 zero;
    zero.w = 0.0f;
    zero.z = 0.0f;
    zero.y = 0.0f;
    zero.x = 0.0f;
}

void hkKeyframedRigidMotion::setInertiaLocal(const hkMatrix3& in) {}

void hkKeyframedRigidMotion::setInertiaInvLocal(const hkMatrix3& in) {}

void hkKeyframedRigidMotion::getInertiaInvLocal(hkMatrix3& out) const {
    hkVector4* cols = (hkVector4*)&out;
    cols[0].x = 0.0f;
    cols[0].y = 0.0f;
    cols[0].z = 0.0f;
    cols[0].w = 0.0f;
    cols[1].x = 0.0f;
    cols[1].y = 0.0f;
    cols[1].z = 0.0f;
    cols[1].w = 0.0f;
    cols[2].x = 0.0f;
    cols[2].y = 0.0f;
    cols[2].z = 0.0f;
    cols[2].w = 0.0f;
    // MATCH-ONLY: the original also leaves a zeroed stack vector behind (stored after the columns).
    hkVector4 zero;
    zero.w = 0.0f;
    zero.z = 0.0f;
    zero.y = 0.0f;
    zero.x = 0.0f;
}

void hkKeyframedRigidMotion::getInertiaInvWorld(hkMatrix3& out) const {
    hkVector4* cols = (hkVector4*)&out;
    cols[0].x = 0.0f;
    cols[0].y = 0.0f;
    cols[0].z = 0.0f;
    cols[0].w = 0.0f;
    cols[1].x = 0.0f;
    cols[1].y = 0.0f;
    cols[1].z = 0.0f;
    cols[1].w = 0.0f;
    cols[2].x = 0.0f;
    cols[2].y = 0.0f;
    cols[2].z = 0.0f;
    cols[2].w = 0.0f;
    // MATCH-ONLY: the original also leaves a zeroed stack vector behind (stored after the columns).
    hkVector4 zero;
    zero.w = 0.0f;
    zero.z = 0.0f;
    zero.y = 0.0f;
    zero.x = 0.0f;
}

void hkKeyframedRigidMotion::applyLinearImpulse(const hkVector4& impulse) {}

void hkKeyframedRigidMotion::applyPointImpulse(const hkVector4& impulse, const hkVector4& point) {}

void hkKeyframedRigidMotion::applyAngularImpulse(const hkVector4& impulse) {}

void hkKeyframedRigidMotion::applyForce(hkReal timestep, const hkVector4& force) {}

void hkKeyframedRigidMotion::applyForce(hkReal timestep, const hkVector4& force, const hkVector4& point) {}

void hkKeyframedRigidMotion::applyTorque(hkReal timestep, const hkVector4& torque) {}

void hkKeyframedRigidMotion::setStepPosition(hkReal stepPosition) {}

// Replaces the stored motion: the new one gains a reference, the old one loses one.
void hkKeyframedRigidMotion::setStoredMotion(hkMaxSizeMotion* motion) {
    if (motion) {
        motion->addReference();
    }
    if (savedMotion) {
        savedMotion->removeReference();
    }
    savedMotion = motion;
}
