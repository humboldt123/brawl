#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 53; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnDededeStarmissileData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1FB8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnDededeStarmissile : public Weapon {
    u8 m_storage[0x1FB8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnDededeStarmissile(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarmissileData* data);
    virtual ~wnDededeStarmissile();
};
static_assert(sizeof(wnDededeStarmissile) == 0x1FB8, "Dedede star missile article extent");
