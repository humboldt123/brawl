#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 54 (the map names the accesser
// getWeaponData68); only passed as a pointer, the layout is unreconstructed (mirror of the blaster family's
// data role).
struct wnDiddyGunData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1CB4); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnDiddyGun : public Weapon {
    u8 m_storage[0x1CB4 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnDiddyGun(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyGunData* data);
    virtual ~wnDiddyGun();
};
static_assert(sizeof(wnDiddyGun) == 0x1CB4, "Diddy popgun article extent");
