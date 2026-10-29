#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 5; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnLinkBowArrowData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1FD8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnLinkBowArrow : public Weapon {
    u8 m_storage[0x1FD8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnLinkBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnLinkBowArrowData* data);
    virtual ~wnLinkBowArrow();
    // HYPOTHESIS: parameter names; the same activation as Toon Link's arrow (founder, resource, team, facing).
    void activate(int founderTaskId, int resourceId, int team, float lr);
    virtual void deactivate(bool force);
    const wnLinkBowArrowData* getData();
    // HYPOTHESIS: parameter names; the two floats go to work floats 0x11000002 and 0x11000001.
    void toFly(float lr, float unk2, float unk1);
    void setFlyPose();
    virtual void updateNodeSRT();
    virtual bool reflect();
};
static_assert(sizeof(wnLinkBowArrow) == 0x1FD8, "Link bow arrow article extent");
