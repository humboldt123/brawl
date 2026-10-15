#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 150; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnKirbyPikminData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x20DC); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnKirbyPikmin : public Weapon {
    u8 m_storage[0x20DC - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnKirbyPikmin(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyPikminData* data);
    virtual ~wnKirbyPikmin();
};
static_assert(sizeof(wnKirbyPikmin) == 0x20DC, "Kirby's Pikmin article extent");
