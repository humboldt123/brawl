// Havok translation unit hkSphereMotion.o (main.dol 0x802E5AE0-0x802E60EC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802E5AE0    32  finishLoadedObjecthkSphereMotion   [map: hkSphereMotion__finishLoadedObjecthkSphereMotion]
//   0x802E5B00    20  cleanupLoadedObjecthkSphereMotion   [map: hkSphereMotion__cleanupLoadedObjecthkSphereMotion]
//   0x802E5B14    60  getVtablehkSphereMotion   [map: hkSphereMotion__getVtablehkSphereMotion]
//   0x802E5B50   108  getInertiaLocal   [map: hkSphereMotion__getInertiaLocal]
//   0x802E5BBC   108  getInertiaWorld   [map: hkSphereMotion__getInertiaWorld]
//   0x802E5C28    80  setInertiaLocal   [map: hkSphereMotion__setInertiaLocal]
//   0x802E5C78    60  setInertiaInvLocal   [map: hkSphereMotion__setInertiaInvLocal]
//   0x802E5CB4   100  getInertiaInvLocal   [map: hkSphereMotion__getInertiaInvLocal]
//   0x802E5D18   100  getInertiaInvWorld   [map: hkSphereMotion__getInertiaInvWorld]
//   0x802E5D7C   364  applyPointImpulse   [map: hkSphereMotion__applyPointImpulse]
//   0x802E5EE8    84  applyAngularImpulse   [map: hkSphereMotion__applyAngularImpulse]
//   0x802E5F3C   128  applyForce   [map: hkSphereMotion__applyForce]
//   0x802E5FBC   112  applyForce   [map: hkSphereMotion__applyForce1]
//   0x802E602C   112  applyTorque   [map: hkSphereMotion__applyTorque]
//   0x802E609C    80  __sinit_\hkSphereMotion_cpp   [map: hkSphereMotioncpp____sinit_]
#pragma fp_contract on
#include <havok/hkSphereMotion.h>

// hkMatrix3 is only forward-declared in these headers (see hkBoxMotion.cpp); elements are the 12 floats of three columns.
static float* matrixElements(hkMatrix3& m) { return (float*)&m; }
static const float* matrixElements(const hkMatrix3& m) { return (const float*)&m; }

// Inertia is the reciprocal of the isotropic inverse inertia (m_inertiaAndMassInv.x).
void hkSphereMotion::getInertiaLocal(hkMatrix3& out) const {
    float inertia = 1.0f / m_inertiaAndMassInv.x;
    float* m = matrixElements(out);
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 0.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[11] = 0.0f;
    m[0] = inertia;
    m[5] = inertia;
    m[10] = inertia;
}

void hkSphereMotion::getInertiaWorld(hkMatrix3& out) const {
    getInertiaLocal(out);
}

// The inverse inertia is the reciprocal of the largest diagonal entry (zero when that entry is not positive).
void hkSphereMotion::setInertiaLocal(const hkMatrix3& in) {
    const float* e = matrixElements(in);
    float largest = e[0];
    if (largest <= e[5]) {
        largest = e[5];
    }
    if (largest <= e[10]) {
        largest = e[10];
    }
    float invInertia = 0.0f;
    if (largest > 0.0f) {
        invInertia = 1.0f / largest;
    }
    m_inertiaAndMassInv.x = invInertia;
    m_inertiaAndMassInv.y = invInertia;
    m_inertiaAndMassInv.z = invInertia;
}

// The inverse inertia is the smallest diagonal entry.
void hkSphereMotion::setInertiaInvLocal(const hkMatrix3& in) {
    const float* e = matrixElements(in);
    float smallest = e[0];
    if (smallest >= e[5]) {
        smallest = e[5];
    }
    if (smallest >= e[10]) {
        smallest = e[10];
    }
    m_inertiaAndMassInv.x = smallest;
    m_inertiaAndMassInv.y = smallest;
    m_inertiaAndMassInv.z = smallest;
}

void hkSphereMotion::getInertiaInvLocal(hkMatrix3& out) const {
    float* m = matrixElements(out);
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 0.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[11] = 0.0f;
    m[0] = m_inertiaAndMassInv.x;
    m[5] = m_inertiaAndMassInv.x;
    m[10] = m_inertiaAndMassInv.x;
}

void hkSphereMotion::getInertiaInvWorld(hkMatrix3& out) const {
    getInertiaInvLocal(out);
}

void hkSphereMotion::applyAngularImpulse(const hkVector4& impulse) {
    m_angularVelocity.x = m_angularVelocity.x + m_inertiaAndMassInv.x * impulse.x;
    m_angularVelocity.y = m_angularVelocity.y + m_inertiaAndMassInv.y * impulse.y;
    m_angularVelocity.z = m_angularVelocity.z + m_inertiaAndMassInv.z * impulse.z;
    m_angularVelocity.w = m_angularVelocity.w + m_inertiaAndMassInv.w * impulse.w;
}

void hkSphereMotion::applyForce(hkReal timestep, const hkVector4& force) {
    hkVector4 impulse;
    impulse.setMul4(force, timestep);
    m_linearVelocity.x = m_linearVelocity.x + m_inertiaAndMassInv.w * impulse.x;
    m_linearVelocity.y = m_linearVelocity.y + m_inertiaAndMassInv.w * impulse.y;
    m_linearVelocity.z = m_linearVelocity.z + m_inertiaAndMassInv.w * impulse.z;
    m_linearVelocity.w = m_linearVelocity.w + m_inertiaAndMassInv.w * impulse.w;
}

void hkSphereMotion::applyTorque(hkReal timestep, const hkVector4& torque) {
    hkVector4 impulse;
    impulse.setMul4(torque, timestep);
    applyAngularImpulse(impulse);
}

// Not yet decompiled in this unit (they need applyPointImpulse, which is still missing, so the class is
// abstract and the loaded-object helpers, the vtable getter and the static init are not written yet):
//   0x802E5AE0  finishLoadedObjecthkSphereMotion, 0x802E5B00 cleanupLoadedObjecthkSphereMotion,
//   0x802E5B14  getVtablehkSphereMotion, 0x802E5D7C applyPointImpulse, 0x802E5FBC applyForce (point),
//   0x802E609C  __sinit.
