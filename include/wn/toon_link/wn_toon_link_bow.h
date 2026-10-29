#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 139; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnToonLinkBowData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x14E4); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnToonLinkBow : public Weapon {
    u8 m_storage[0x14E4 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnToonLinkBow(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkBowData* data);
    virtual ~wnToonLinkBow();
    // HYPOTHESIS: parameter names; the Toon Link transactor passes the founder task and the resource id.
    void activate(int founderTaskId, int resourceId);
};
static_assert(sizeof(wnToonLinkBow) == 0x14E4, "ToonLinkBow article extent");
