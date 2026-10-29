#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 38; only passed as a pointer, the
// layout is unreconstructed (mirror of the bullet family's data role).
struct wnWolfBlasterData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1D88); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnWolfBlaster : public Weapon {
    u8 m_storage[0x1D88 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnWolfBlaster(s32 articleId, const wnArticleBuildInfo& info, const wnWolfBlasterData* data);
    // HYPOTHESIS: parameter types follow ftWolfTransactor::activeArticle1, which forwards its model and texture
    // resource ids and two flags; the founder task id comes first and the team last.
    void activate(int founderTaskId, u32 mdlResId, u32 texResId, bool unk1, bool unk2, int team);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int selector);
    virtual ~wnWolfBlaster();
};
static_assert(sizeof(wnWolfBlaster) == 0x1D88, "Wolf Blaster article extent");
