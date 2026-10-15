#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 77; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnLucasPKFlashData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x2054); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnLucasPKFlash : public Weapon {
    u8 m_storage[0x2054 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnLucasPKFlash(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKFlashData* data);
    virtual ~wnLucasPKFlash();
};
static_assert(sizeof(wnLucasPKFlash) == 0x2054, "Lucas PK Flash article extent");
