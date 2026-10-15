#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 68; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnSheikNeedleData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1BD0); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnSheikNeedle : public Weapon {
    u8 m_storage[0x1BD0 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnSheikNeedle(s32 articleId, const wnArticleBuildInfo& info, const wnSheikNeedleData* data);
    virtual ~wnSheikNeedle();
};
static_assert(sizeof(wnSheikNeedle) == 0x1BD0, "Sheik needle article extent");
