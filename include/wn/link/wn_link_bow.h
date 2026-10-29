#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 4; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnLinkBowData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x14E4); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnLinkBow : public Weapon {
    u8 m_storage[0x14E4 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnLinkBow(s32 articleId, const wnArticleBuildInfo& info, const wnLinkBowData* data);
    virtual ~wnLinkBow();
    // HYPOTHESIS: parameter names; ftLinkTransactor::activeArticle passes the founder task and the resource id.
    void activate(int founderTaskId, int resourceId);
};
static_assert(sizeof(wnLinkBow) == 0x14E4, "LinkBow article extent");
