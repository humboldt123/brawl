// Havok translation unit hkBoxMotion.o (main.dol 0x802E4828-0x802E5258).
#include <new>
#include <havok/hkBoxMotion.h>
#include <havok/hkRegistry.h>

// The 12 floats of the three columns: entry (row r, column c) is [c * 4 + r].
static float* matrixElements(hkMatrix3& m) { return &m.m_col0.x; }
static const float* matrixElements(const hkMatrix3& m) { return &m.m_col0.x; }

static hkMotionTypeInfo s_hkBoxMotionTypeInfo("hkBoxMotion", hkBoxMotion::finishLoadedObjecthkBoxMotion,
                                              hkBoxMotion::cleanupLoadedObjecthkBoxMotion,
                                              hkBoxMotion::getVtablehkBoxMotion());

void hkBoxMotion::finishLoadedObjecthkBoxMotion(void* p) {
    new (p) hkBoxMotion(hkFinishLoadedObjectFlag());
}

void hkBoxMotion::cleanupLoadedObjecthkBoxMotion(void* p) {
    ((hkBoxMotion*)p)->~hkBoxMotion();
}

const void* hkBoxMotion::getVtablehkBoxMotion() {
    hkVector4 buf[16]; // 0x100 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkBoxMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

hkBoxMotion::hkBoxMotion(const hkVector4& position, const hkQuaternion& rotation) : hkMotion(position, rotation) {
    m_inertiaAndMassInv.x = 1.0f;
    m_inertiaAndMassInv.y = 1.0f;
    m_inertiaAndMassInv.z = 1.0f;
    m_inertiaAndMassInv.w = 1.0f;
    m_type = MOTION_BOX_INERTIA;
}


void hkBoxMotion::getInertiaLocal(hkMatrix3& out) const {
    hkReal ix = 1.0f / m_inertiaAndMassInv.x;
    hkReal iy = 1.0f / m_inertiaAndMassInv.y;
    hkReal iz = 1.0f / m_inertiaAndMassInv.z;
    out.setDiagonal(ix, iy, iz);
}

void hkBoxMotion::setInertiaLocal(const hkMatrix3& in) {
    m_inertiaAndMassInv.x = 1.0f / matrixElements(in)[0];
    m_inertiaAndMassInv.y = 1.0f / matrixElements(in)[5];
    m_inertiaAndMassInv.z = 1.0f / matrixElements(in)[10];
}

void hkBoxMotion::getInertiaInvLocal(hkMatrix3& out) const {
    hkReal ix = m_inertiaAndMassInv.x;
    hkReal iy = m_inertiaAndMassInv.y;
    hkReal iz = m_inertiaAndMassInv.z;
    out.setDiagonal(ix, iy, iz);
}

void hkBoxMotion::setInertiaInvLocal(const hkMatrix3& in) {
    hkReal z = matrixElements(in)[10];
    m_inertiaAndMassInv.y = matrixElements(in)[5];
    m_inertiaAndMassInv.x = matrixElements(in)[0];
    m_inertiaAndMassInv.z = z;
}

// Inertia in world space: local diagonal inertia rotated by the body basis.
void hkBoxMotion::getInertiaWorld(hkMatrix3& out) const {
    getInertiaLocal(out);
    out.changeBasis(m_motionState.m_transform.m_rotation);
}

void hkBoxMotion::setMass(hkReal mass) {
    setMassInv(1.0f / mass);
}

void hkBoxMotion::applyForce(hkReal timestep, const hkVector4& force) {
    hkVector4 impulse;
    impulse.setMul4(timestep, force);
    hkReal massInv = m_inertiaAndMassInv.w;
    m_linearVelocity.x = massInv * impulse.x + m_linearVelocity.x;
    m_linearVelocity.y = massInv * impulse.y + m_linearVelocity.y;
    m_linearVelocity.z = massInv * impulse.z + m_linearVelocity.z;
    m_linearVelocity.w = massInv * impulse.w + m_linearVelocity.w;
}

void hkBoxMotion::applyForce(hkReal timestep, const hkVector4& force, const hkVector4& point) {
    hkVector4 impulse;
    impulse.setMul4(timestep, force);
    applyPointImpulse(impulse, point);
}

// Inverse inertia in world space: R * diag(invInertia) * R^T, with the diagonal scaling folded into R's columns.
void hkBoxMotion::getInertiaInvWorld(hkMatrix3& out) const {
    hkVector4 a;
    a.setAll(m_inertiaAndMassInv.x);
    hkVector4 b;
    b.setAll(m_inertiaAndMassInv.y);
    hkVector4 c;
    c.setAll(m_inertiaAndMassInv.z);
    const hkRotation& r = getTransform().m_rotation;
    hkMatrix3 m;
    m.m_col0.setMul4(a, r.m_col0);
    m.m_col1.setMul4(b, r.m_col1);
    m.m_col2.setMul4(c, r.m_col2);
    out.setMulInverse(m, r);
}

void hkBoxMotion::applyTorque(hkReal timestep, const hkVector4& torque) {
    hkVector4 impulse;
    impulse.setMul4(timestep, torque);
    applyAngularImpulse(impulse);
}

// Not yet decompiled in this unit:
//   0x802E4BE0   804  applyPointImpulse   [map: hkBoxMotion__applyPointImpulse]
//   0x802E4F04   420  applyAngularImpulse   [map: hkBoxMotion__applyAngularImpulse]
