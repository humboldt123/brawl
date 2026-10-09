#pragma once

#include <havok/hkEntity.h>

// Rigid body (0x1F0 bytes). Adds no fields to hkEntity; layout from hkRigidBodyClass.cpp.
// The motion (hkFixedRigidMotion / hkKeyframedRigidMotion style) is reached through hkEntity::getMotion().
struct hkRigidBody : hkEntity {
    hkRigidBody(hkFinishLoadedObjectFlag flag) : hkEntity(flag) {}

    // Loaded-object support (static init registration).
    static void finishLoadedObjecthkRigidBody(void* p);
    static void cleanupLoadedObjecthkRigidBody(void* p);
    static const void* getVtablehkRigidBody();

    virtual hkMotionState* getMotionState();

    void setDeactivator(hkEntityDeactivator* deactivator);
    void setMotionType(u8 motionType, u8 a, u8 b);
    void setMass(hkReal mass);
    void setPosition(const hkVector4& position);
    void setPositionAndRotation(const hkVector4& position, const hkQuaternion& rotation);
    void setTransform(const hkTransform& transform);
    // HYPOTHESIS: refreshes the broadphase entry and collision information after the body is warped.
    void updateBroadphaseAndResetCollisionInformationOfWarpedBody();
};
