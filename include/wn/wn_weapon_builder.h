#pragma once

// Local BrawlHeaders shadow: Weapon ends at 0xCC; keep the unresolved default
// builder's established total extent until its configuration is reconstructed.
#include <StaticAssert.h>
#include <types.h>
#include <wn/weapon.h>

template <typename T>
class wnWeaponBuilder : public Weapon {
public:
    char unkCC[0x2160 - sizeof(Weapon)];
    // TODO: virtual functions
};
static_assert(sizeof(wnWeaponBuilder<void>) == 0x2160, "Class is wrong size!");
