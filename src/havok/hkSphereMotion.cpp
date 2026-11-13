// Havok translation unit hkSphereMotion.o (main.dol 0x802E5AE0-0x802E60EC).
// Functions in address order (method names from the Havok TU map):
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
#include <new>
#include <havok/hkSphereMotion.h>
#include <havok/hkRegistry.h>

// The 12 floats of the three columns: entry (row r, column c) is [c * 4 + r].
static float* matrixElements(hkMatrix3& m) { return &m.m_col0.x; }
static const float* matrixElements(const hkMatrix3& m) { return &m.m_col0.x; }

static hkMotionTypeInfo s_hkSphereMotionTypeInfo("hkSphereMotion", hkSphereMotion::finishLoadedObjecthkSphereMotion,
                                                 hkSphereMotion::cleanupLoadedObjecthkSphereMotion,
                                                 hkSphereMotion::getVtablehkSphereMotion());

void hkSphereMotion::finishLoadedObjecthkSphereMotion(void* p) {
    new (p) hkSphereMotion(hkFinishLoadedObjectFlag());
}

void hkSphereMotion::cleanupLoadedObjecthkSphereMotion(void* p) {
    ((hkSphereMotion*)p)->~hkSphereMotion();
}

const void* hkSphereMotion::getVtablehkSphereMotion() {
    hkVector4 buf[16]; // 0x100 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkSphereMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

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

// Same body as getInertiaLocal (the world and local inertia of an isotropic sphere are equal).
void hkSphereMotion::getInertiaWorld(hkMatrix3& out) const {
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

// The inverse inertia is the reciprocal of the largest diagonal entry (zero when that entry is not positive).
void hkSphereMotion::setInertiaLocal(const hkMatrix3& in) {
    const float* e = matrixElements(in);
    float y = e[5];
    float z;
    float largest = e[0];
    z = e[10];
    largest = largest > y ? largest : y;
    largest = largest > z ? largest : z;
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
    float diagY = e[5];
    float diagZ = e[10];
    float smallest = e[0];
    smallest = smallest < diagY ? smallest : diagY;
    smallest = smallest < diagZ ? smallest : diagZ;
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

// The torque about the center of mass (point - centerOfMass) x impulse is applied to the angular velocity.
void hkSphereMotion::applyPointImpulse(const hkVector4& impulse, const hkVector4& point) {
    hkVector4 r;
    r.setSub4(point, m_motionState.m_sweptTransform.m_centerOfMass1);
    hkVector4 torque;
    torque.setCross(r, impulse);
    m_linearVelocity.x = m_linearVelocity.x + m_inertiaAndMassInv.w * impulse.x;
    m_linearVelocity.y = m_linearVelocity.y + m_inertiaAndMassInv.w * impulse.y;
    m_linearVelocity.z = m_linearVelocity.z + m_inertiaAndMassInv.w * impulse.z;
    m_linearVelocity.w = m_linearVelocity.w + m_inertiaAndMassInv.w * impulse.w;
    m_angularVelocity.x = m_angularVelocity.x + m_inertiaAndMassInv.x * torque.x;
    m_angularVelocity.y = m_angularVelocity.y + m_inertiaAndMassInv.y * torque.y;
    m_angularVelocity.z = m_angularVelocity.z + m_inertiaAndMassInv.z * torque.z;
    m_angularVelocity.w = m_angularVelocity.w + m_inertiaAndMassInv.w * torque.w;
}

void hkSphereMotion::applyAngularImpulse(const hkVector4& impulse) {
    m_angularVelocity.x = m_angularVelocity.x + m_inertiaAndMassInv.x * impulse.x;
    m_angularVelocity.y = m_angularVelocity.y + m_inertiaAndMassInv.y * impulse.y;
    m_angularVelocity.z = m_angularVelocity.z + m_inertiaAndMassInv.z * impulse.z;
    m_angularVelocity.w = m_angularVelocity.w + m_inertiaAndMassInv.w * impulse.w;
}

void hkSphereMotion::applyForce(hkReal timestep, const hkVector4& force) {
    hkVector4 impulse;
    impulse.setMul4(timestep, force);
    hkReal massInv = m_inertiaAndMassInv.w;
    m_linearVelocity.x = massInv * impulse.x + m_linearVelocity.x;
    m_linearVelocity.y = massInv * impulse.y + m_linearVelocity.y;
    m_linearVelocity.z = massInv * impulse.z + m_linearVelocity.z;
    m_linearVelocity.w = massInv * impulse.w + m_linearVelocity.w;
}

void hkSphereMotion::applyForce(hkReal timestep, const hkVector4& force, const hkVector4& point) {
    hkVector4 impulse;
    impulse.setMul4(timestep, force);
    applyPointImpulse(impulse, point);
}

void hkSphereMotion::applyTorque(hkReal timestep, const hkVector4& torque) {
    hkVector4 impulse;
    impulse.setMul4(timestep, torque);
    applyAngularImpulse(impulse);
}
