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
#endif
};
