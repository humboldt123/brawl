#pragma once

#include <havok/hkMotion.h>

// Sphere-inertia motion (type 2, 0x100 bytes). Adds no fields: the inverse inertia is isotropic and kept in
// m_inertiaAndMassInv (x for the inertia, w for the inverse mass), see hkMotion.
struct hkSphereMotion : hkMotion {
    hkSphereMotion(hkFinishLoadedObjectFlag f) : hkMotion(f) {}
    virtual ~hkSphereMotion() {} // 0x08

    // Loaded-object support (static init registration; see src/havok/hkSphereMotion.cpp).
    static void finishLoadedObjecthkSphereMotion(void* p);
    static void cleanupLoadedObjecthkSphereMotion(void* p);
    static const void* getVtablehkSphereMotion() __attribute__((never_inline));

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
