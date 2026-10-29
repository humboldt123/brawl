#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <types.h>

class soModuleAccesser;

// Basic 2D speed/acceleration energy. Layout and member meanings are inferred from
// soKineticEnergyNormal::init / updateEnergy (HYPOTHESIS names).
class soKineticEnergyNormal : public soKineticEnergy {
public:
#if defined(WN_KINETIC_GENERIC_BUILDER) || defined(FT_KINETIC_STOP_IMPLEMENTATION)
#ifdef WN_KINETIC_NORMAL_EXTERN
    // MATCH-ONLY: the Pikmin Dolfin article calls the sora_melee constructor and assignment (declarations only).
    soKineticEnergyNormal(const Vec2f& speed, const Vec2f& accel,
                          const Vec2f& target, const Vec2f& brake,
                          const Vec2f& limit, bool unk30, bool friction);
    soKineticEnergyNormal& operator=(const soKineticEnergyNormal& other);
    // MATCH-ONLY: the same constructor expanded inline where the pool holder builds its energy (the native holder passes
    // five constructed vectors to the inlined constructor; the extra tag only selects this overload).
    soKineticEnergyNormal(const Vec2f& speed, const Vec2f& accel, const Vec2f& target, const Vec2f& brake, const Vec2f& limit,
                          bool unk30, bool friction, int) : soKineticEnergy(false) {
        Vec2f::copy(m_speed, speed);
        Vec2f::copy(m_accel, accel);
        Vec2f::copy(m_speedTarget, target);
        Vec2f::copy(m_brake, brake);
        Vec2f::copy(m_speedLimit, limit);
        m_unk30 = unk30;
        m_considerGroundFriction = friction;
        m_unk32 = true;
    }
#else
    // Diddy's holders call the five-vector constructor as a separate function;
    // stopping energy expands the same constructor inline.
    // HYPOTHESIS: the first byte flag retains its unknown source meaning.
    soKineticEnergyNormal(const Vec2f& speed, const Vec2f& accel,
                          const Vec2f& target, const Vec2f& brake,
                          const Vec2f& limit, bool unk30, bool friction)
#ifndef FT_KINETIC_STOP_IMPLEMENTATION
        __attribute__((never_inline))
#endif
        : soKineticEnergy(false) {
        Vec2f::copy(m_speed, speed);
        Vec2f::copy(m_accel, accel);
        Vec2f::copy(m_speedTarget, target);
        Vec2f::copy(m_brake, brake);
        Vec2f::copy(m_speedLimit, limit);
        m_unk30 = unk30;
        m_considerGroundFriction = friction;
        m_unk32 = true;
    }
    // MATCH-ONLY: the original assignment copies the vectors as word pairs after the base assignment.
    soKineticEnergyNormal& operator=(const soKineticEnergyNormal& other) {
        soKineticEnergy::operator=(other);
        Vec2f::copy(m_speed, other.m_speed);
        Vec2f::copy(m_accel, other.m_accel);
        Vec2f::copy(m_speedTarget, other.m_speedTarget);
        Vec2f::copy(m_brake, other.m_brake);
        Vec2f::copy(m_speedLimit, other.m_speedLimit);
        m_unk30 = other.m_unk30;
        m_considerGroundFriction = other.m_considerGroundFriction;
        m_unk32 = other.m_unk32;
        return *this;
    }
#endif
#endif
    soKineticEnergyNormal() {
        // MATCH-ONLY: the original builds all default vectors first and then copies them member by member.
        Vec2f limit(-1.0f, -1.0f);
        Vec2f brake(0.0f, 0.0f);
        Vec2f target(-1.0f, -1.0f);
        Vec2f accel(0.0f, 0.0f);
        Vec2f speed(0.0f, 0.0f);
        Vec2f::copy(m_speed, speed);
        Vec2f::copy(m_accel, accel);
        Vec2f::copy(m_speedTarget, target);
        Vec2f::copy(m_brake, brake);
        Vec2f::copy(m_speedLimit, limit);
        m_unk30 = false;
        m_considerGroundFriction = false;
        m_unk32 = true;
    }

    virtual void updateEnergy(soModuleAccesser* moduleAccesser);
    virtual Vec2f getSpeed();
    virtual Vec3f getRotation();
    virtual void resetEnergy(int, Vec2f*, Vec3f*, soModuleAccesser* moduleAccesser);
    virtual void clearSpeed();
    virtual void clearRotSpeed();
    virtual void mulSpeed(Vec3f* speed);
    virtual void mulAccel(Vec3f* accel);
    virtual void reflectSpeed(Vec3f* speed);
    virtual void reflectAccel(Vec3f* accel);
    virtual void onConsiderGroundFriction();
    virtual void offConsiderGroundFriction();
    virtual ~soKineticEnergyNormal() { }
    virtual void init();

    void setBrake(Vec2f* brake);
    void setStableSpeed(Vec2f* speed); // HYPOTHESIS: sets the target speed (m_speedTarget); weak in the fighter RELs
    void setSpeed(Vec2f* speed);

    Vec2f m_speed;        // +0x08
    Vec2f m_accel;        // +0x10
    Vec2f m_speedTarget;  // +0x18 (init -1,-1)
    Vec2f m_brake;        // +0x20
    Vec2f m_speedLimit;   // +0x28 (init -1,-1)
    bool m_unk30;         // +0x30
    bool m_considerGroundFriction; // +0x31
    bool m_unk32;         // +0x32
};
static_assert(sizeof(soKineticEnergyNormal) == 0x34, "Class is wrong size!");

#ifdef FT_MODULE_BUILDER
// MATCH-ONLY: in the fighter RELs these virtual functions are weak copies (the original header defined them inline);
// sora_melee has the out-of-line versions in so_kinetic_energy_normal.cpp.
inline void soKineticEnergyNormal::clearRotSpeed() { }

inline void soKineticEnergyNormal::clearSpeed() {
    m_speed = Vec2f(0.0f, 0.0f);
}

inline void soKineticEnergyNormal::init() {
    Vec2f zero(0.0f, 0.0f);
    m_speed = m_accel = m_brake = zero;
    Vec2f negOne(-1.0f, -1.0f);
    m_speedTarget = m_speedLimit = negOne;
    m_considerGroundFriction = false;
    m_unk30 = false;
    m_unk32 = true;
}

inline void soKineticEnergyNormal::offConsiderGroundFriction() {
    m_considerGroundFriction = false;
}

inline void soKineticEnergyNormal::onConsiderGroundFriction() {
    m_considerGroundFriction = true;
}

#pragma dont_inline on
inline Vec3f soKineticEnergyNormal::getRotation() {
    return Vec3f(0.0f, 0.0f, 0.0f);
}
#pragma dont_inline off

inline Vec2f soKineticEnergyNormal::getSpeed() {
    return m_speed;
}
#endif

#if defined(SO_KINETIC_WIND_IMPLEMENTATION) && !defined(FT_MODULE_BUILDER)
// This unit expands the normal-energy reset inline, as the fighter headers do.
inline void soKineticEnergyNormal::init() {
    Vec2f zero(0.0f, 0.0f);
    m_speed = m_accel = m_brake = zero;
    Vec2f negOne(-1.0f, -1.0f);
    m_speedTarget = m_speedLimit = negOne;
    m_considerGroundFriction = false;
    m_unk30 = false;
    m_unk32 = true;
}
#endif
