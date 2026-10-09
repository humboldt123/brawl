#pragma once

#include <havok/hkBase.h>
#include <havok/hkVector4.h>
#include <havok/hkQuaternion.h>
#include <havok/hkMotionState.h>
#include <havok/hkRegistry.h>
#include <havok/hkWorldCinfo.h> // hkFinishLoadedObjectFlag (int m_finishing)

struct hkTransform;
struct hkMatrix3;

// Static type registration record (hkTypeInfo) whose constructor fills the fields in order.
// HYPOTHESIS: the original static is built by a constructor; the hkTypeInfo declaration is in hkRegistry.h.
struct hkMotionTypeInfo : hkTypeInfo {
    hkMotionTypeInfo(const char* name, void (*finish)(void*), void (*cleanup)(void*), const void* vtable) {
        m_name = name;
        m_finish = finish;
        m_cleanup = cleanup;
        m_vtable = vtable;
    }
};

// Base simulation motion (0x100 bytes). Derived from hkReferencedObject; the
// motion type is at 0x08 and the per-body motion state at 0x10.
// Layout from hkMotionClass.cpp and the hkMotion TU.
struct hkMotion : hkReferencedObject {
    enum MotionType {
        MOTION_INVALID = 0,
        MOTION_DYNAMIC = 1,
        MOTION_SPHERE_INERTIA = 2,
        MOTION_STABILIZED_SPHERE_INERTIA = 3,
        MOTION_BOX_INERTIA = 4,
        MOTION_STABILIZED_BOX_INERTIA = 5,
        MOTION_KEYFRAMED = 6,
        MOTION_FIXED = 7,
        MOTION_THIN_BOX_INERTIA = 8,
        MOTION_MAX_ID = 9,
    };

    u8 m_type;                     // 0x08 (MotionType)
    hkMotionState m_motionState;   // 0x10
    hkVector4 m_inertiaAndMassInv; // 0xD0 (x, y, z: inverse inertia diagonal, w: inverse mass)
    hkVector4 m_linearVelocity;    // 0xE0
    hkVector4 m_angularVelocity;   // 0xF0

    hkMotion(const hkVector4& position, const hkQuaternion& rotation);
    hkMotion(hkFinishLoadedObjectFlag f) {}
    virtual ~hkMotion() {}                                              // 0x08
    // calcStatistics                                                   // 0x0C (inherited)
    virtual void setMass(hkReal mass);                                  // 0x10
    virtual void setMassInv(hkReal massInv);                            // 0x14
    virtual void getInertiaLocal(hkMatrix3& out) const = 0;             // 0x18
    virtual void getInertiaWorld(hkMatrix3& out) const = 0;             // 0x1C
    virtual void setInertiaLocal(const hkMatrix3& in) = 0;              // 0x20
    virtual void setInertiaInvLocal(const hkMatrix3& in) = 0;           // 0x24
    virtual void getInertiaInvLocal(hkMatrix3& out) const = 0;          // 0x28
    virtual void getInertiaInvWorld(hkMatrix3& out) const = 0;          // 0x2C
    virtual void setCenterOfMassInLocal(const hkVector4& com);          // 0x30
    virtual void setPosition(const hkVector4& position);                // 0x34
    virtual void setRotation(const hkQuaternion& rotation);             // 0x38
    virtual void setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation); // 0x3C
    virtual void setTransform(const hkTransform& transform);            // 0x40
    virtual void setLinearVelocity(const hkVector4& v);                 // 0x44
    virtual void setAngularVelocity(const hkVector4& v);                // 0x48
    virtual void applyLinearImpulse(const hkVector4& impulse);          // 0x4C
    virtual void applyPointImpulse(const hkVector4& impulse, const hkVector4& point) = 0; // 0x50
    virtual void applyAngularImpulse(const hkVector4& impulse) = 0;     // 0x54
    virtual void applyForce(hkReal timestep, const hkVector4& force) = 0; // 0x58 HYPOTHESIS: arguments
    virtual void applyForce(hkReal timestep, const hkVector4& force, const hkVector4& point) = 0; // 0x5C HYPOTHESIS
    virtual void applyTorque(hkReal timestep, const hkVector4& torque) = 0; // 0x60 HYPOTHESIS
    virtual void getMotionStateAndVelocities(hkMotion* out) const;      // 0x64 HYPOTHESIS: out type

    hkReal getMass() const;
    void setDeactivationClass(u16 deactivationClass);
    void setDeactivationCounter(u16 deactivationCounter); // defined in src/havok/hkRigidMotionUtil.cpp
};
