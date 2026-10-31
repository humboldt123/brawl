#pragma once

// The articles Fox owns and the mediator that hands them out: the blaster, its bullets (five), the Phantasm afterimages
// (two) and the Landmaster (Final Smash). The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
#include <wn/fox/wn_fox_blaster.h>
#include <wn/fox/wn_fox_land_master.h>

class Vec2f;
class Vec3f;
struct wnFoxIllusionData;

// The Phantasm afterimage. The full class lives in wn/fox/wn_fox_illusion.h, whose builder specializations cannot coexist
// with this Fighter-based builder unit, so the fighter unit sees an opaque shell of the right size.
class wnFoxIllusion : public Weapon {
    u8 m_unreconstructed[0x1DE4 - sizeof(Weapon)];
public:
    wnFoxIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnFoxIllusionData* data);
    virtual ~wnFoxIllusion();
    void activate(s32 founderTaskId, s32 unk1, s32 team, float lr, s32 unk2, s32 unk3);
};
static_assert(sizeof(wnFoxIllusion) == 0x1DE4, "Fox Illusion layout is wrong!");

// The blaster bullet. The full class lives in wn/fox/wn_fox_blaster_bullet.h (a weapon builder specialization that cannot coexist
// with this Fighter-based builder unit), so the fighter unit sees an opaque shell of the right size.
struct wnFoxBlasterBulletData;
class wnFoxBlasterBullet : public Weapon {
    u8 m_unreconstructed[0x2178 - sizeof(Weapon)];
public:
    wnFoxBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnFoxBlasterBulletData* data);
    virtual ~wnFoxBlasterBullet();
};
static_assert(sizeof(wnFoxBlasterBullet) == 0x2178, "Fox blaster bullet layout is wrong!");

typedef wnArticleHierarchy<wnFoxLandMaster, 1, soInstancePoolRoot> ftFoxLandMasterPool;
typedef wnArticleHierarchy<wnFoxIllusion, 2, ftFoxLandMasterPool> ftFoxIllusionPool;
typedef wnArticleHierarchy<wnFoxBlasterBullet, 5, ftFoxIllusionPool> ftFoxBulletPool;
typedef wnArticleHierarchy<wnFoxBlaster, 1, ftFoxBulletPool> ftFoxBlasterPool;

class ftFoxArticleMediator : public soArticleMediator {
    ftFoxBlasterPool m_pools;
    u8 m_autoRecycle;
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftFoxArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftFoxArticleMediator();
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
FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef wnArticleManageModuleBuilder<ftFoxArticleMediator, 5> ftFoxArticleManageModuleBuilder;
static_assert(sizeof(ftFoxArticleManageModuleBuilder) == 0x14CD0, "Fox article manage builder layout is wrong!");
