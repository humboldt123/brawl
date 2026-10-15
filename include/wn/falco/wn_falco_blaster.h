#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 35; only passed as a pointer, the
// layout is unreconstructed (mirror of the bullet family's data role).
struct wnFalcoBlasterData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1D88); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnFalcoBlaster : public Weapon {
    u8 m_storage[0x1D88 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnFalcoBlaster(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoBlasterData* data);
    virtual ~wnFalcoBlaster();
};
static_assert(sizeof(wnFalcoBlaster) == 0x1D88, "Falco Blaster article extent");
