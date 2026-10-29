#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 109; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnPopoIceShotData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x23D0); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnPopoIceShot : public Weapon {
    u8 m_storage[0x23D0 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnPopoIceShot(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceShotData* data);
    virtual ~wnPopoIceShot();
    // HYPOTHESIS: the parameter order and types follow the Popo transactor's call (founder task, resource, team,
    // position, facing, extra id) and the Blizzard activation; the map gives none.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, float lr, int unk40);
    void shoot();
    virtual void processUpdate();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void updatePosture(bool);
};
static_assert(sizeof(wnPopoIceShot) == 0x23D0, "Popo ice shot article extent");
