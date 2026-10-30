#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type of the class; only passed as a pointer here, the layout is unreconstructed.
struct wnSamusGunData;
struct wnArticleBuildInfo;

// HYPOTHESIS: extent from the Samus article pools (0x1834); the internal layout and the builder chain are unreconstructed.
class wnSamusGun : public Weapon {
    u8 m_storage[0x1834 - sizeof(Weapon)];
public:
    wnSamusGun(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGunData* data);
    virtual ~wnSamusGun();
    // HYPOTHESIS: unk1..unk4 are the resource ids of the activation descriptor (+0x4/+0x8, +0xC, +0x10, +0x14).
    void activate(int founderTaskId, int unk1, int unk2, int unk3, int unk4, int team);
};
static_assert(sizeof(wnSamusGun) == 0x1834, "Samus gun article extent");
