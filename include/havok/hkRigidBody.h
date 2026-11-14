#pragma once

#include <havok/hkEntity.h>
#include <havok/hkRigidBodyCinfo.h>

// Rigid body (0x1F0 bytes). Adds no fields to hkEntity; layout from hkRigidBodyClass.cpp.
// The motion (hkFixedRigidMotion / hkKeyframedRigidMotion style) is reached through hkEntity::getMotion().
struct hkRigidBody : hkEntity {
    hkRigidBody(hkFinishLoadedObjectFlag flag) : hkEntity(flag) {}
    hkRigidBody(const hkRigidBodyCinfo& info);
    // phShape's view of the same constructor (it hands its construction info over as an int).
    hkRigidBody(int flag);
    // Fills info with the body's current construction values.
    void getCinfo(hkRigidBodyCinfo& info) const;

    // Loaded-object support (static init registration).
    static void finishLoadedObjecthkRigidBody(void* p);
    static void cleanupLoadedObjecthkRigidBody(void* p);
    static const void* getVtablehkRigidBody();

    virtual hkMotionState* getMotionState();
    // Replaces the shape: the body leaves the broad phase while its cached shape data is rebuilt.
    virtual bool setShape(hkShape* shape);
    // Accessors into the body's motion block (out of line in src/havok/hkSimulation.cpp).
    hkMotion* getRigidMotion();
    hkTransform& getTransform();
    hkVector4& getCenterOfMassInWorld();

    void setDeactivator(hkEntityDeactivator* deactivator);
    // Installs a deactivator of the given hkRigidBodyDeactivator::DeactivatorType (out of line).
    void setDeactivator(int type);
    void setLinearVelocity(const hkVector4& v) {
        activate();
        getMotion()->setLinearVelocity(v);
    }
    void setAngularVelocity(const hkVector4& v) {
        activate();
        getMotion()->setAngularVelocity(v);
    }
    void setMotionType(u8 motionType, u8 a, u8 b);
    void setMass(hkReal mass);
    void setCenterOfMassLocal(const hkVector4& com);
    // HYPOTHESIS: computes the shape bounds and a normalized direction into out (0x10 bytes).
    // Out of line at 0x802E23CC (hkRigidBody__updateCachedShapeInfo), not written yet.
    void updateCachedShapeInfo(hkShape* shape, hkVector4* out);
    // Builds the motion of the given type in motionBuffer; dynamic motions pick sphere, box or thin box inertia
    // from the spread of the inertia diagonal.
    static void createDynamicRigidMotion(hkMotion::MotionType motionType, const hkVector4& position,
                                         const hkQuaternion& rotation, hkReal mass, hkReal maxLinearVelocity,
                                         hkReal maxAngularVelocity, const hkMatrix3& inertiaLocal,
                                         const hkVector4& centerOfMassLocal, hkMotion* motionBuffer);
    void setPosition(const hkVector4& position);
    void setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation);
    void setTransform(const hkTransform& transform);
    // Turns the body to the new rotation about its centre of mass (the centre of mass stays where it is).
    void setRotationAroundCentreOfMass(const hkQuaternion& newRotation);
    // HYPOTHESIS: refreshes the broadphase entry and collision information after the body is warped.
    void updateBroadphaseAndResetCollisionInformationOfWarpedBody();
};
