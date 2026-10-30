#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type of the class; only passed as a pointer here, the layout is unreconstructed.
struct wnSamusLaser2Data;
struct wnArticleBuildInfo;

// HYPOTHESIS: extent from the Samus article pools (0x17C8); the internal layout and the builder chain are unreconstructed.
class wnSamusLaser2 : public Weapon {
    u8 m_storage[0x17C8 - sizeof(Weapon)];
public:
    wnSamusLaser2(s32 articleId, const wnArticleBuildInfo& info, const wnSamusLaser2Data* data);
    virtual ~wnSamusLaser2();
    // HYPOTHESIS: the float is the height offset kept in work float 0x11000001.
    void activate(int founderTaskId, int etcResourceId, int team, float height);
    const wnSamusLaser2Data* getData();
    void toShoot(float lr);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnSamusLaser2) == 0x17C8, "Samus second laser article extent");
