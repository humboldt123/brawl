#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 69; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnSheikNeedleHaveData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x17F4); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnSheikNeedleHave : public Weapon {
    u8 m_storage[0x17F4 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnSheikNeedleHave(s32 articleId, const wnArticleBuildInfo& info, const wnSheikNeedleHaveData* data);
    virtual ~wnSheikNeedleHave();
    // HYPOTHESIS: parameter names; the transactor passes the founder, resource and team as for the Pit arrow.
    void activate(int founderTaskId, int resourceId, int team);
};
static_assert(sizeof(wnSheikNeedleHave) == 0x17F4, "Sheik held needle article extent");
