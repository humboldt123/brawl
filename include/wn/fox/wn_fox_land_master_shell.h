#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 34; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnFoxLandMasterShellData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1F28); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnFoxLandMasterShell : public Weapon {
    u8 m_storage[0x1F28 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnFoxLandMasterShell(s32 articleId, const wnArticleBuildInfo& info, const wnFoxLandMasterShellData* data);
    virtual ~wnFoxLandMasterShell();
};
static_assert(sizeof(wnFoxLandMasterShell) == 0x1F28, "Fox Land Master shell article extent");
