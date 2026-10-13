#pragma once
#include <wn/wn_weapon_builder.h>
#include <types.h>

// Wario's fart: the gas article the Waft (down special) leaves behind.
// Proved prefix only: the primary Weapon builder base follows the other article units; the complete allocation size
// remains HYPOTHESIS.
struct wnWarioGarlicModuleAccesserBuildConfig;
template <>
class wnWeaponBuilder<wnWarioGarlicModuleAccesserBuildConfig> : public Weapon {
    u8 unkD0[0x2160 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
class wnWarioGarlic : public wnWeaponBuilder<wnWarioGarlicModuleAccesserBuildConfig> {
public:
    // HYPOTHESIS: the names of the arguments follow what activate stores them in.
    void activate(int founderTaskId, int resourceId, int team);
};
