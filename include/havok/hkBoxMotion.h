#pragma once

#include <havok/hkMotion.h>

// Box-inertia motion (type 4, vtable 0x80488020). Adds no fields; the inverse diagonal
// inertia lives in m_inertiaAndMassInv (x, y, z), see hkMotion.
struct hkBoxMotion : hkMotion {
    hkBoxMotion(const hkVector4& position, const hkQuaternion& rotation);
    hkBoxMotion(hkFinishLoadedObjectFlag f) : hkMotion(f) {}
    virtual ~hkBoxMotion() {} // 0x08

    // Loaded-object support (static init registration; see src/havok/hkBoxMotion.cpp).
    static void finishLoadedObjecthkBoxMotion(void* p);
    static void cleanupLoadedObjecthkBoxMotion(void* p);
    static const void* getVtablehkBoxMotion() __attribute__((never_inline));

    virtual void setMass(hkReal mass);                                  // 0x10
    virtual void getInertiaLocal(hkMatrix3& out) const;                 // 0x18
    virtual void getInertiaWorld(hkMatrix3& out) const;                 // 0x1C
    virtual void setInertiaLocal(const hkMatrix3& in);                  // 0x20
    virtual void setInertiaInvLocal(const hkMatrix3& in);               // 0x24
    virtual void getInertiaInvLocal(hkMatrix3& out) const;              // 0x28
    virtual void getInertiaInvWorld(hkMatrix3& out) const;              // 0x2C
    virtual void applyPointImpulse(const hkVector4& impulse, const hkVector4& point); // 0x50
    virtual void applyAngularImpulse(const hkVector4& impulse);         // 0x54
    virtual void applyForce(hkReal timestep, const hkVector4& force);   // 0x58 HYPOTHESIS
    virtual void applyForce(hkReal timestep, const hkVector4& force, const hkVector4& point); // 0x5C HYPOTHESIS
    virtual void applyTorque(hkReal timestep, const hkVector4& torque); // 0x60 HYPOTHESIS
};
