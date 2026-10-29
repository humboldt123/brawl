#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 36; only passed as a pointer, the
// layout is unreconstructed (mirror of the bullet family's data role).
struct wnFalcoBlasterBulletData;
struct wnArticleBuildInfo;

// HYPOTHESIS: only the first word, which activate copies into the lifetime, is known.
struct wnFalcoBlasterBulletParam {
    s32 unk0;
};

// Extent known from the class info createArray allocation (0x2174); the builder chain stays blob-provided
// until the constructor is reconstructed.
// HYPOTHESIS: as in the Fox bullet, the parameter pointer (+0x213C) and the parameter accesser (+0x2140)
// follow a 0x213C-byte weapon builder.
class wnFalcoBlasterBullet : public Weapon {
    u8 m_storage[0x213C - sizeof(Weapon)];
    const wnFalcoBlasterBulletParam* m_param;
    u8 m_paramAccesser[0x2174 - 0x2140];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnFalcoBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoBlasterBulletData* data);
    virtual ~wnFalcoBlasterBullet();
    // HYPOTHESIS: r5 is unused; the remaining arguments follow the Fox and Wolf bullet activate.
    void activate(int founderTaskId, int unk5, int team, Vec3f* pos, int unkMode, EfID effectId, float lr,
                  float speed, float angle);
    virtual bool reflect();
    virtual float getCollisionLr(soModuleAccesser* moduleAccesser);
    virtual void processUpdate();
    virtual void updateNodeSRT();
};
static_assert(sizeof(wnFalcoBlasterBullet) == 0x2174, "Falco Blaster Bullet article extent");
