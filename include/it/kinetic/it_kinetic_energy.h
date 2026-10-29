#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <types.h>

class soModuleAccesser;

// HYPOTHESIS: member names follow the update code (the layout is the one of soKineticEnergyNormal).

class itKineticEnergyInterface : public soKineticEnergy {
public:
    virtual void updateEnergy(soModuleAccesser* moduleAccesser) = 0;
    virtual Vec2f getSpeed() = 0;
    virtual Vec3f getRotation() = 0;
    virtual void resetEnergy(int, Vec2f* speed, Vec3f*, soModuleAccesser* moduleAccesser) = 0;
    virtual void clearSpeed() = 0;
    virtual void clearRotSpeed() = 0;
    virtual void mulSpeed(Vec3f* speed) = 0;
    virtual void mulAccel(Vec3f* accel) = 0;
    virtual void reflectSpeed(Vec3f* normal) = 0;
    virtual void reflectAccel(Vec3f* normal) = 0;
    virtual ~itKineticEnergyInterface() { }
};

class itKineticEnergy : public itKineticEnergyInterface {
public:
    enum LimitFlag {
        Limit_PlusX = 0x1,
        Limit_MinusX = 0x2,
        Limit_PlusY = 0x4,
        Limit_MinusY = 0x8,
        Limit_PlusZ = 0x10,
        Limit_MinusZ = 0x20,
        Limit_Length = 0x40,
    };

    itKineticEnergy() {
        Vec2f limit(-1.0f, -1.0f);
        Vec2f brake(0.0f, 0.0f);
        Vec2f stableSpeed(0.0f, 0.0f);
        Vec2f accel(0.0f, 0.0f);
        Vec2f speed(0.0f, 0.0f);
        Vec2f::copy(m_speed, speed);
        Vec2f::copy(m_accel, accel);
        Vec2f::copy(m_stableSpeed, stableSpeed);
        Vec2f::copy(m_brake, brake);
        Vec2f::copy(m_limit, limit);
        m_isFollowGroundNormal = false;
        m_considerGroundFriction = false;
        m_limitFlags = 0x3F;
    }

    virtual void updateEnergy(soModuleAccesser* moduleAccesser);
    virtual Vec2f getSpeed() { return m_speed; }
    virtual Vec3f getRotation() { return Vec3f(0.0f, 0.0f, 0.0f); }
    virtual void resetEnergy(int, Vec2f* speed, Vec3f*, soModuleAccesser*) {
        m_speed.m_x = speed->m_x;
        m_speed.m_y = speed->m_y;
    }
    virtual void clearSpeed() { m_speed = Vec2f(0.0f, 0.0f); }
    virtual void clearRotSpeed() { }
    virtual void mulSpeed(Vec3f* speed);
    virtual void mulAccel(Vec3f* accel);
    virtual void reflectSpeed(Vec3f* normal);
    virtual void reflectAccel(Vec3f* normal);
    virtual void onConsiderGroundFriction() { m_considerGroundFriction = true; }
    virtual void offConsiderGroundFriction() { m_considerGroundFriction = false; }
    virtual ~itKineticEnergy() { }
    // HYPOTHESIS: parameter order (the map gives no parameter types).
    virtual void updateEnergy(soModuleAccesser* moduleAccesser, float rate);

    Vec2f m_speed;                 // +0x08
    Vec2f m_accel;                 // +0x10
    Vec2f m_stableSpeed;           // +0x18
    Vec2f m_brake;                 // +0x20
    Vec2f m_limit;                 // +0x28
    bool m_isFollowGroundNormal;   // +0x30
    bool m_considerGroundFriction; // +0x31
    u32 m_limitFlags;              // +0x34
};
static_assert(sizeof(itKineticEnergy) == 0x38, "Class is wrong size!");

class itKineticEnergy3f : public itKineticEnergyInterface {
public:
    virtual void updateEnergy(soModuleAccesser* moduleAccesser);
    virtual Vec2f getSpeed() { return Vec2f(m_speed.m_x, m_speed.m_y); }
    virtual Vec3f getSpeed3f() { return m_speed; }
    virtual Vec3f getRotation() { return Vec3f(0.0f, 0.0f, 0.0f); }
    virtual void resetEnergy(int, Vec2f* speed, Vec3f*, soModuleAccesser*) {
        m_speed.m_x = speed->m_x;
        m_speed.m_y = speed->m_y;
    }
    virtual void clearSpeed() { m_speed = Vec3f(0.0f, 0.0f, 0.0f); }
    virtual void clearRotSpeed() { }
    virtual void mulSpeed(Vec3f* speed);
    virtual void mulAccel(Vec3f* accel);
    virtual void reflectSpeed(Vec3f* normal);
    virtual void reflectAccel(Vec3f* normal);
    virtual void onConsiderGroundFriction() { m_considerGroundFriction = true; }
    virtual void offConsiderGroundFriction() { m_considerGroundFriction = false; }
    virtual ~itKineticEnergy3f() { }
    // HYPOTHESIS: parameter order, as itKineticEnergy::updateEnergy(soModuleAccesser*, float).
    virtual void updateEnergy(soModuleAccesser* moduleAccesser, float rate);

    Vec3f m_speed;                 // +0x08
    Vec3f m_accel;                 // +0x14
    Vec3f m_stableSpeed;           // +0x20
    Vec3f m_brake;                 // +0x2C
    Vec3f m_limit;                 // +0x38
    bool m_isFollowGroundNormal;   // +0x44
    bool m_considerGroundFriction; // +0x45
    u32 m_limitFlags;              // +0x48
};
static_assert(sizeof(itKineticEnergy3f) == 0x4C, "Class is wrong size!");

class itKineticEnergyGravity : public itKineticEnergyInterface {
public:
    virtual void updateEnergy(soModuleAccesser* moduleAccesser);
    virtual Vec2f getSpeed() { return Vec2f(0.0f, m_speed); }
    virtual Vec3f getRotation() { return Vec3f(0.0f, 0.0f, 0.0f); }
    virtual void resetEnergy(int, Vec2f* speed, Vec3f*, soModuleAccesser*) { m_speed = speed->m_y; }
    virtual void clearSpeed() { m_speed = 0.0f; }
    virtual void clearRotSpeed() { }
    virtual void mulSpeed(Vec3f* speed);
    virtual void mulAccel(Vec3f* accel);
    virtual void reflectSpeed(Vec3f* normal);
    virtual void reflectAccel(Vec3f* normal);
    virtual ~itKineticEnergyGravity() { }
    // HYPOTHESIS: the only argument is the new vertical speed (stored as is).
    virtual void resetEnergy(float speed) { m_speed = speed; }
    // HYPOTHESIS: parameter order, as itKineticEnergy::updateEnergy(soModuleAccesser*, float).
    virtual void updateEnergy(soModuleAccesser* moduleAccesser, float rate);

    float m_speed;       // +0x08
    float m_accel;       // +0x0C
    float m_stableSpeed; // +0x10
    float m_brake;       // +0x14
    float m_limit;       // +0x18
    u32 m_limitFlags;    // +0x1C
};
static_assert(sizeof(itKineticEnergyGravity) == 0x20, "Class is wrong size!");

class itKineticEnergyStop : public itKineticEnergy {
public:
    itKineticEnergyStop();
    virtual void updateEnergy(soModuleAccesser* moduleAccesser);
    virtual void resetEnergy(int mode, Vec2f* speed, Vec3f*, soModuleAccesser* moduleAccesser);
    virtual ~itKineticEnergyStop() { }
    virtual void updateEnergy(soModuleAccesser* moduleAccesser, float rate) {
        itKineticEnergy::updateEnergy(moduleAccesser, rate);
    }

    int m_mode; // +0x38
};
static_assert(sizeof(itKineticEnergyStop) == 0x3C, "Class is wrong size!");
