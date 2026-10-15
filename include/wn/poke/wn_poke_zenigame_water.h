#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 95; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnPokeZenigameWaterData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1690); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnPokeZenigameWater : public Weapon {
    u8 m_storage[0x1690 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnPokeZenigameWater(s32 articleId, const wnArticleBuildInfo& info, const wnPokeZenigameWaterData* data);
    virtual ~wnPokeZenigameWater();
};
static_assert(sizeof(wnPokeZenigameWater) == 0x1690, "Pokemon water gun article extent");
