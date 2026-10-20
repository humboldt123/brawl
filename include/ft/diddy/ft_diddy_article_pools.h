#pragma once

// The articles Diddy owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnDiddyGunData;
struct wnDiddyBarrelData;
struct wnDiddyPeanutsData;
struct wnDiddyExplosionData;

// The peanut popgun (the class is in sora_melee). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnDiddyGun : public Weapon {
    u8 m_unreconstructed[0x1CB4 - sizeof(Weapon)];
public:
    wnDiddyGun(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyGunData* data);
    virtual ~wnDiddyGun();
};
static_assert(sizeof(wnDiddyGun) == 0x1CB4, "wnDiddyGun layout is wrong!");

// The barrel rocket (the class is in the REL, wn_diddy_barrel). Opaque shell for the same reason.
class wnDiddyBarrel : public Weapon {
    u8 m_unreconstructed[0x25F0 - sizeof(Weapon)];
public:
    wnDiddyBarrel(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyBarrelData* data);
    virtual ~wnDiddyBarrel();
    void activate(int founderTaskId, int resourceId, int team, Vec3f* pos, Vec2f* speed, bool unk5, float lr, float rot);
    void shoot(Vec3f* pos, Vec2f* speed, float lr, float rot, float angle, bool unk5);
};
static_assert(sizeof(wnDiddyBarrel) == 0x25F0, "wnDiddyBarrel layout is wrong!");

// The peanuts (the class is in sora_melee). Opaque shell for the same reason.
class wnDiddyPeanuts : public Weapon {
    u8 m_unreconstructed[0x1FB4 - sizeof(Weapon)];
public:
    wnDiddyPeanuts(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyPeanutsData* data);
    virtual ~wnDiddyPeanuts();
    void activate(int founderTaskId, int resourceId, int team, float lr, Vec3f* pos, Vec2f* speed);
};
static_assert(sizeof(wnDiddyPeanuts) == 0x1FB4, "wnDiddyPeanuts layout is wrong!");

// The explosion of the peanut (the class is in the REL). Opaque shell for the same reason.
class wnDiddyExplosion : public Weapon {
    u8 m_unreconstructed[0x1F98 - sizeof(Weapon)];
public:
    wnDiddyExplosion(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyExplosionData* data);
    virtual ~wnDiddyExplosion();
    void activate(int founderTaskId, int team, Vec3f* position, float lr);
};
static_assert(sizeof(wnDiddyExplosion) == 0x1F98, "wnDiddyExplosion layout is wrong!");

typedef wnArticleHierarchy<wnDiddyExplosion, 1, soInstancePoolRoot> ftDiddyExplosionPool;
typedef wnArticleHierarchy<wnDiddyPeanuts, 10, ftDiddyExplosionPool> ftDiddyPeanutsPool;
typedef wnArticleHierarchy<wnDiddyBarrel, 2, ftDiddyPeanutsPool> ftDiddyBarrelPool;
typedef wnArticleHierarchy<wnDiddyGun, 2, ftDiddyBarrelPool> ftDiddyGunPool;

class ftDiddyArticleMediator : public soArticleMediator {
    ftDiddyGunPool m_pools;
    u8 m_autoRecycle;
public:
    ftDiddyArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftDiddyArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The lists of the module live in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 15);

typedef wnArticleManageModuleBuilder<ftDiddyArticleMediator, 15> ftDiddyArticleManageModuleBuilder;
static_assert(sizeof(ftDiddyArticleManageModuleBuilder) == 0x1E400, "Diddy article manage builder layout is wrong!");
