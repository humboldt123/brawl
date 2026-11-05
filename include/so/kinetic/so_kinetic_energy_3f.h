#pragma once

#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <types.h>

// HYPOTHESIS: the plain 3D speed energy (named by the RTTI object of the fighter RELs); only the speed vector that the
// weapon statuses write at +0x8 is modelled.
class soKineticEnergy3f : public soKineticEnergy {
public:
#ifdef SO_KINETIC_ENERGY_3F_FULL
    // Opt-in per unit (default off): a unit that owns an article with this energy as a member (Snake's RGB6 bullet)
    // tears it down inline.
    virtual ~soKineticEnergy3f() { }
#else
    virtual ~soKineticEnergy3f();
#endif
    void resetEnergy(int, Vec3f*, Vec3f*, soModuleAccesser*);
    Vec3f m_speed3f; // +0x08
#ifdef SO_KINETIC_ENERGY_3F_FULL
    // HYPOTHESIS: the 3D counterpart of soKineticEnergyNormal (the RGB6 bullet constructor fills four more vectors and
    // two flags); only the size is evidenced.
    Vec3f m_accel3f;       // +0x14
    Vec3f m_targetSpeed3f; // +0x20
    Vec3f m_brake3f;       // +0x2C
    Vec3f m_limit3f;       // +0x38
    bool unk44;
    bool unk45;
#ifdef SO_KINETIC_ENERGY_3F_DEFAULT_CTOR
    // Opt-in (Pit's Ikarus pool constructs the energy in its unit): the default vectors are built as default
    // arguments, last first, then copied into the members (same values as soKineticEnergyNormal's defaults).
    soKineticEnergy3f(const Vec3f& speed = Vec3f(0.0f, 0.0f, 0.0f), const Vec3f& accel = Vec3f(0.0f, 0.0f, 0.0f),
                      const Vec3f& target = Vec3f(-1.0f, -1.0f, -1.0f), const Vec3f& brake = Vec3f(0.0f, 0.0f, 0.0f),
                      const Vec3f& limit = Vec3f(-1.0f, -1.0f, -1.0f)) :
        soKineticEnergy(false), m_speed3f(speed), m_accel3f(accel), m_targetSpeed3f(target), m_brake3f(brake),
        m_limit3f(limit), unk44(false), unk45(false) { }
#endif
#endif
};
