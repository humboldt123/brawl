#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 99; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnLucarioAuraBallData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x2050); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnLucarioAuraBall : public Weapon {
    u8 m_storage[0x2050 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnLucarioAuraBall(s32 articleId, const wnArticleBuildInfo& info, const wnLucarioAuraBallData* data);
    virtual ~wnLucarioAuraBall();
};
static_assert(sizeof(wnLucarioAuraBall) == 0x2050, "Lucario aura ball article extent");
