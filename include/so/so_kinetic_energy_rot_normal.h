#pragma once

#include <so/kinetic/so_kinetic_energy.h>

// The concrete vtable and reset/getRotation/update methods identify this
// rotational energy. Sonic's constructor places the next module at +0x44.
class soKineticEnergyRotNormal : public soKineticEnergy {
public:
#ifdef WN_WEAPON_BUILDER
    // The default vectors are default arguments: the article builders construct them before the energy.
    soKineticEnergyRotNormal(const Vec3f& rotSpeed = Vec3f(0.0f, 0.0f, 0.0f), const Vec3f& accel = Vec3f(0.0f, 0.0f, 0.0f),
                             const Vec3f& speedTarget = Vec3f(-1.0f, -1.0f, -1.0f), const Vec3f& brake = Vec3f(0.0f, 0.0f, 0.0f),
                             const Vec3f& speedLimit = Vec3f(-1.0f, -1.0f, -1.0f)) :
        m_rotSpeed(rotSpeed), m_accel(accel), m_speedTarget(speedTarget), m_brake(brake), m_speedLimit(speedLimit) { }
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
#if defined(WN_ARTICLE_NATIVE_BUILDERS) && !defined(FT_ROT_ENERGY_DTOR_OUT_OF_LINE) && !defined(FT_ROT_ENERGY_DTOR_OUTLINE)
    virtual ~soKineticEnergyRotNormal() { }
#else
    virtual ~soKineticEnergyRotNormal();
#endif
    // HYPOTHESIS: argument spelling remains unresolved for this unused empty
    // callback; its additional virtual slot is proved by the concrete table.
    virtual void projectionNormalFollow();

    // Native ft_snake helper copies a Vec3f into m_speedTarget (+0x20).
    void setStableSpeed(Vec3f* speed);

    Vec3f m_rotSpeed;     // +0x08: getRotation and clearRotSpeed
    Vec3f m_accel;        // +0x14: mulAccel and reflectAccel
    Vec3f m_speedTarget;  // +0x20: updateEnergy
    Vec3f m_brake;        // +0x2C: updateEnergy
    Vec3f m_speedLimit;   // +0x38: updateEnergy
};
static_assert(sizeof(soKineticEnergyRotNormal) == 0x44, "Rotational energy size");
