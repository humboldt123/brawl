#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 30; only passed as a pointer, the
// layout is unreconstructed (mirror of the bullet family's data role).
struct wnFoxBlasterData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1D7C); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnFoxBlaster : public Weapon {
    u8 m_storage[0x1D7C - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnFoxBlaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxBlasterData* data);
    // HYPOTHESIS: parameter types follow ftFoxTransactor::activeArticle1, which forwards its model and texture
    // resource ids and two flags; the founder task id comes first and the team last.
    void activate(int founderTaskId, u32 mdlResId, u32 texResId, bool unk1, bool unk2, int team);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int selector);
    virtual ~wnFoxBlaster();
};
static_assert(sizeof(wnFoxBlaster) == 0x1D7C, "Fox Blaster article extent");
