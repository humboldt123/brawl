#pragma once

#include <havok/hkMotion.h>
#include <havok/hkThreadMemory.h>

struct hkMaxSizeMotion;

// Keyframed rigid motion (type 6, 0x110 bytes). Adds the stored motion pointer and its quality index.
// Layout from hkKeyframedRigidMotionClass.cpp. Inertia and impulses are ignored (keyframed bodies).
struct hkKeyframedRigidMotion : hkMotion {
    static void* operator new(unsigned long nbytes) {
        hkReferencedObject* p = (hkReferencedObject*)hkMemory::getInstance().allocateChunk(nbytes, 0x2c);
        p->m_memSizeAndFlags = nbytes;
        return p;
    }
    static void* operator new(unsigned long, void* where) { return where; } // placement new for loaded objects
    static void operator delete(void* p) {
        hkThreadMemory::s_instance->deallocateChunk(p, ((hkReferencedObject*)p)->m_memSizeAndFlags, 0x2c);
    }

    hkMaxSizeMotion* savedMotion; // 0x100
    int savedQualityTypeIndex;    // 0x104

    hkKeyframedRigidMotion(const hkVector4& position, const hkQuaternion& rotation);
    hkKeyframedRigidMotion(hkFinishLoadedObjectFlag f) : hkMotion(f) {}
    virtual ~hkKeyframedRigidMotion(); // 0x08 (out of line in src/havok/hkKeyframedRigidMotion.cpp)

    // Loaded-object support (static init registration; see src/havok/hkKeyframedRigidMotion.cpp).
    static void finishLoadedObjecthkKeyframedRigidMotion(void* p);
    static void cleanupLoadedObjecthkKeyframedRigidMotion(void* p);
    static const void* getVtablehkKeyframedRigidMotion() __attribute__((never_inline));
    // The max-size variant is listed under this class in the Havok TU map (see hkMaxSizeMotion below).
    static void finishLoadedObjecthkMaxSizeMotion(void* p);
    static void cleanupLoadedObjecthkMaxSizeMotion(void* p);
    static const void* getVtablehkMaxSizeMotion() __attribute__((never_inline));

    virtual void setMass(hkReal mass);                                  // 0x10
    virtual void setMassInv(hkReal massInv);                            // 0x14
    virtual void getInertiaLocal(hkMatrix3& out) const;                 // 0x18
    virtual void getInertiaWorld(hkMatrix3& out) const;                 // 0x1C
    virtual void setInertiaLocal(const hkMatrix3& in);                  // 0x20
    virtual void setInertiaInvLocal(const hkMatrix3& in);               // 0x24
    virtual void getInertiaInvLocal(hkMatrix3& out) const;              // 0x28
    virtual void getInertiaInvWorld(hkMatrix3& out) const;              // 0x2C
    virtual void applyLinearImpulse(const hkVector4& impulse);          // 0x4C
    virtual void applyPointImpulse(const hkVector4& impulse, const hkVector4& point); // 0x50
    virtual void applyAngularImpulse(const hkVector4& impulse);         // 0x54
    virtual void applyForce(hkReal timestep, const hkVector4& force);   // 0x58 HYPOTHESIS
    virtual void applyForce(hkReal timestep, const hkVector4& force, const hkVector4& point); // 0x5C HYPOTHESIS
    virtual void applyTorque(hkReal timestep, const hkVector4& torque); // 0x60 HYPOTHESIS
    virtual void setStepPosition(hkReal stepPosition);                  // HYPOTHESIS: parameter unused

    void setStoredMotion(hkMaxSizeMotion* motion);
};

// Keyframed motion with the largest storage size (same layout and dtor as hkKeyframedRigidMotion).
struct hkMaxSizeMotion : hkKeyframedRigidMotion {
    hkMaxSizeMotion(hkFinishLoadedObjectFlag f) : hkKeyframedRigidMotion(f) {}
};
