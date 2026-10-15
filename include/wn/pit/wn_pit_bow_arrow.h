#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 91; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnPitBowArrowData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1FD8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnPitBowArrow : public Weapon {
    u8 m_storage[0x1FD8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnPitBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowArrowData* data);
    virtual ~wnPitBowArrow();
};
static_assert(sizeof(wnPitBowArrow) == 0x1FD8, "Pit bow arrow article extent");
