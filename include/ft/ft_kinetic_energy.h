#pragma once

#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <types.h>

// Kinetic energies used by fighters (instantiated in the soKineticMediatorImpl type list).
// Only the members that the REL code touches directly are modelled; the rest of the
// classes live in sora_melee.

// Converts animation translation into planar fighter velocity.
class ftKineticEnergyMotion : public soKineticEnergyNormal {
public:
    ftKineticEnergyMotion(); // sora_melee
#ifdef FT_KINETIC_MOTION_IMPLEMENTATION
    virtual ~ftKineticEnergyMotion();
#elif defined(FT_MODULE_BUILDER)
    virtual ~ftKineticEnergyMotion() { }
#endif
    virtual void resetEnergy(int mode, Vec2f* speed, Vec3f*, soModuleAccesser*);
    virtual void updateEnergy(soModuleAccesser*);

    int m_motionMode; // +0x34
    float m_facing; // +0x38; reset snapshots facing, selected modes read it live.
    float m_rotation; // +0x3C; radians, also written by launch status processes.
    float m_speedMul; // +0x40; scales all three animation-translation components.
    Vec2f m_prevMotionSpeed; // +0x44; used to apply changes in animation velocity.
    bool m_hasPrevMotionSpeed; // +0x4C
};
static_assert(sizeof(ftKineticEnergyMotion) == 0x50, "Motion energy extent");

// MATCH-ONLY: declaration order keeps shared controller RTTI before motion RTTI.
#include <ft/ft_kinetic_energy_controller.h>

// Fighter braking modes share the normal energy's speed, brake and limit vectors.
class ftKineticEnergyStop : public soKineticEnergyNormal {
public:
    ftKineticEnergyStop(); // sora_melee
#ifdef FT_KINETIC_STOP_IMPLEMENTATION
    virtual ~ftKineticEnergyStop();
#elif defined(FT_MODULE_BUILDER)
    virtual ~ftKineticEnergyStop() { }
#endif
    virtual void resetEnergy(int type, Vec2f* speed, Vec3f*, soModuleAccesser*);
    virtual void updateEnergy(soModuleAccesser*);

    s32 m_resetType; // +0x34; constructor/reset use -1 as the inactive mode.
    // HYPOTHESIS: caller-facing meanings of these delayed speed-scale flags.
    bool unk38;
    bool unk39;
};
static_assert(sizeof(ftKineticEnergyStop) == 0x3C, "Stopping energy extent");

class ftKineticEnergyDamage : public ftKineticEnergyStop {
public:
#ifdef FT_MODULE_BUILDER
    ftKineticEnergyDamage() { }
#endif
#ifdef FT_MODULE_BUILDER
    virtual ~ftKineticEnergyDamage() { }
#endif
};

class ftKineticEnergyGravity : public soKineticEnergy {
public:
    ftKineticEnergyGravity(); // sora_melee
    virtual ~ftKineticEnergyGravity() { }
    virtual void resetEnergy(int, Vec2f*, Vec3f*, soModuleAccesser*);
    virtual void clearSpeed();
    virtual void updateEnergy(soModuleAccesser*);
    virtual void mulSpeed(Vec3f*);
    virtual void mulAccel(Vec3f*);
    virtual void reflectSpeed(Vec3f*);
    virtual void reflectAccel(Vec3f*);
    virtual void clearRotSpeed();
    virtual Vec3f getRotation();
    virtual Vec2f getSpeed();

    s32 m_resetType;
    // Verified by ftKineticEnergyGravity::getSpeed: vertical speed at 0xC.
    float m_speedY;
    float m_gravity;
    float m_fallSpeedMax;
    // Yoshi kinetic mode 0x68 writes this offset as a scalar float.
    float unk18;
    // Vertical speed limit used when m_limitMode selects the current direction.
    float unk1C;
    // Reset snapshots the owner's gravity multiplier; update applies it to speed and acceleration.
    float m_gravityMul;
    // -1 disables the limit; 0 limits falling, 1 rising, 2 both directions.
    s32 m_limitMode;
};
static_assert(sizeof(ftKineticEnergyGravity) == 0x28, "Gravity energy extent");

class soKineticEnergyWindNormal : public soKineticEnergyNormal {
public:
    soKineticEnergyWindNormal();
#ifdef SO_KINETIC_WIND_IMPLEMENTATION
    virtual ~soKineticEnergyWindNormal();
#elif defined(FT_MODULE_BUILDER)
    virtual ~soKineticEnergyWindNormal() { }
#endif
    virtual void updateEnergy(soModuleAccesser*);
    virtual void init();

    Vec2f m_windSpeed; // 0x34, accumulated external wind, consumed each update.
    // HYPOTHESIS: original name. The stronger-wind blend retains this fraction of the old velocity.
    float m_responseRetention; // 0x3C
};
static_assert(sizeof(soKineticEnergyWindNormal) == 0x40, "Wind energy extent");

class soKineticEnergyGroundMovement : public soKineticEnergy {
public:
    soKineticEnergyGroundMovement();
#ifdef SO_KINETIC_CONTACT_IMPLEMENTATION
    virtual ~soKineticEnergyGroundMovement();
#elif defined(FT_MODULE_BUILDER)
    virtual ~soKineticEnergyGroundMovement() { }
#endif
    virtual void updateEnergy(soModuleAccesser*);
    virtual Vec2f getSpeed();
    virtual Vec3f getRotation();
    virtual void resetEnergy(int, Vec2f*, Vec3f*, soModuleAccesser*);
    virtual void clearSpeed();
    virtual void clearRotSpeed();
    virtual void mulSpeed(Vec3f*);
    virtual void mulAccel(Vec3f*);
    virtual void reflectSpeed(Vec3f*);
    virtual void reflectAccel(Vec3f*);
    void enableRot(Vec2f* normal);

    // HYPOTHESIS: names of the per-frame external movement inputs, cleared after update.
    Vec2f m_outsideSpeed; // 0x08
    Vec2f m_speed; // 0x10
    float m_brake; // 0x18
    float m_brakeThreshold; // 0x1C
    float m_outsideRotation; // 0x20
    float m_rotation; // 0x24, degrees this frame
    float m_prevGroundAngle; // 0x28, radians
    bool m_brakeEnabled; // 0x2C
    bool m_rotationEnabled; // 0x2D
};
static_assert(sizeof(soKineticEnergyGroundMovement) == 0x30, "Ground movement energy extent");

class soKineticEnergyJostle : public soKineticEnergy {
public:
    soKineticEnergyJostle();
#ifdef SO_KINETIC_CONTACT_IMPLEMENTATION
    virtual ~soKineticEnergyJostle();
#elif defined(FT_MODULE_BUILDER)
    virtual ~soKineticEnergyJostle() { }
#endif
    virtual void updateEnergy(soModuleAccesser*);
    virtual Vec2f getSpeed();
    virtual Vec3f getSpeed3f();
    virtual Vec3f getRotation();
    virtual void resetEnergy(int, Vec2f*, Vec3f*, soModuleAccesser*);
    virtual void clearSpeed();
    virtual void clearRotSpeed();
    virtual void mulSpeed(Vec3f*);
    virtual void mulAccel(Vec3f*);
    virtual void reflectSpeed(Vec3f*);
    virtual void reflectAccel(Vec3f*);

    Vec3f m_speed; // 0x08: planar push plus correction to the jostle module's depth.
};
static_assert(sizeof(soKineticEnergyJostle) == 0x14, "Jostle energy extent");

#include <so/so_module_accesser.h>

// Sum speed helper (the real wrapper is probably an inline function of a util class)
// MWCC only inlines these when each has a single call site in the TU, so the
// wrapper is stamped out once per user (FT_DEFINE_GET_SUM_SPEED(name)).
#define FT_DEFINE_GET_SUM_SPEED(name)     inline Vec2f name(soModuleAccesser* a) {         soKineticEnergy::AttributeFlag flag(1);         return a->getKineticModule().getSumSpeed(flag);     }
