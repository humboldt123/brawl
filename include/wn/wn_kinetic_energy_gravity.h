#pragma once
#include <so/kinetic/so_kinetic_energy.h>
// Layout and vertical-motion roles are established by reset/update/accessors,
// the concrete vtable and RTTI, and the Super Sonic builder constructor.
class wnKineticEnergyGravity : public soKineticEnergy {
public:
    wnKineticEnergyGravity() : m_speedY(0.0f), m_gravity(0.0f), m_speedLimit(-1.0f) {}
#ifdef WN_KINETIC_GENERIC_BUILDER
    // BYTES: the Pikmin transactor calls this constructor out of line.
    wnKineticEnergyGravity(float speedY, float gravity, float speedLimit) __attribute__((never_inline)) : m_speedY(speedY), m_gravity(gravity), m_speedLimit(speedLimit) {}
#else
    wnKineticEnergyGravity(float speedY, float gravity, float speedLimit) : m_speedY(speedY), m_gravity(gravity), m_speedLimit(speedLimit) {}
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
#ifdef WN_KINETIC_GRAVITY_INLINE_DTOR
    // BYTES: the Pikmin transactor's temporary calls the base destructor directly and assigns out of line.
    wnKineticEnergyGravity& operator=(const wnKineticEnergyGravity& other) __attribute__((never_inline)) {
        soKineticEnergy::operator=(other);
        m_speedY = other.m_speedY;
        m_gravity = other.m_gravity;
        m_speedLimit = other.m_speedLimit;
        return *this;
    }
    virtual ~wnKineticEnergyGravity() { }
#else
    virtual ~wnKineticEnergyGravity();
#endif
    float m_speedY; // +8: reset/getSpeed/clear/mul use vertical speed.
    float m_gravity; // +C: update adds acceleration, reflect/mulAccel use it.
    float m_speedLimit; // +10: positive bound applied to descending speed.
};
static_assert(sizeof(wnKineticEnergyGravity) == 0x14, "Weapon gravity layout");
