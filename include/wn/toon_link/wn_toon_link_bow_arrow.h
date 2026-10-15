#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 140; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnToonLinkBowArrowData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1FD8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnToonLinkBowArrow : public Weapon {
    u8 m_storage[0x1FD8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnToonLinkBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkBowArrowData* data);
    virtual ~wnToonLinkBowArrow();
};
static_assert(sizeof(wnToonLinkBowArrow) == 0x1FD8, "Toon Link bow arrow article extent");
