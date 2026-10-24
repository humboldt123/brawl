#pragma once

#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <types.h>

// HYPOTHESIS: the plain 3D speed energy (named by the RTTI object of the fighter RELs); only the speed vector that the
// weapon statuses write at +0x8 is modelled.
class soKineticEnergy3f : public soKineticEnergy {
public:
    Vec3f m_speed3f; // +0x08
};
