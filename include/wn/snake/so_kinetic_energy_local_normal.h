#pragma once

#include <so/so_kinetic_energy_normal.h>

// HYPOTHESIS: Snake's local-normal kinetic energy extends the normal 2D
// energy with one scalar at +0x34. Fly status reads/writes that scalar.
class soKineticEnergyLocalNormal : public soKineticEnergyNormal {
public:
    // Defined (inline) in wn_snake_nikita.cpp, the unit that builds the missile.
    soKineticEnergyLocalNormal(float angle, const Vec2f& speed, const Vec2f& accel, const Vec2f& target,
                               const Vec2f& brake, const Vec2f& limit, bool unk30);
    float unk34;
};
static_assert(sizeof(soKineticEnergyLocalNormal) == 0x38, "Local normal energy size");
