#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 90; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnPitBowData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1534, as in include/ft/pit/ft_pit_article_pools.h);
// the internal layout and the builder chain stay blob-provided until the constructor is reconstructed.
class wnPitBow : public Weapon {
    u8 m_storage[0x1534 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnPitBow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowData* data);
    virtual ~wnPitBow();
    // HYPOTHESIS: parameter names and types; ftPitTransactor::activeArticle passes the founder task, the model
    // and texture resource ids and its two flags.
    void activate(int founderTaskId, int resourceId, int texResId, bool unk1, bool unk2);
};
static_assert(sizeof(wnPitBow) == 0x1534, "Pit bow article extent");
