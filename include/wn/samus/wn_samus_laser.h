#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type of the class; only passed as a pointer here, the layout is unreconstructed.
struct wnSamusLaserData;
struct wnArticleBuildInfo;

// HYPOTHESIS: extent from the Samus article pools (0x18F4); the internal layout and the builder chain are unreconstructed.
class wnSamusLaser : public Weapon {
    u8 m_storage[0x18F4 - sizeof(Weapon)];
public:
    wnSamusLaser(s32 articleId, const wnArticleBuildInfo& info, const wnSamusLaserData* data);
    virtual ~wnSamusLaser();
    void activate(int founderTaskId, int etcResourceId, int team);
    const wnSamusLaserData* getData();
    void toShoot(float lr);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnSamusLaser) == 0x18F4, "Samus laser article extent");
