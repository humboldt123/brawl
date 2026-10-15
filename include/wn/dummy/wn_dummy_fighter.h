#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 152; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnDummyFighterData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x9EC); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnDummyFighter : public Weapon {
    u8 m_storage[0x9EC - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnDummyFighter(s32 articleId, const wnArticleBuildInfo& info, const wnDummyFighterData* data);
    virtual ~wnDummyFighter();
};
static_assert(sizeof(wnDummyFighter) == 0x9EC, "dummy fighter article extent");
