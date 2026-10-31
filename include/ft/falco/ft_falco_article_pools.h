#pragma once

// The articles Falco owns and the mediator that hands them out: the blaster, its bullets (five), the Phantasm afterimages
// (two) and the Landmaster (Final Smash). The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
#include <wn/falco/wn_falco_blaster.h>
#include <wn/falco/wn_falco_blaster_bullet.h>
#include <wn/fox/wn_fox_land_master.h>

class Vec2f;
class Vec3f;
struct wnFalcoIllusionData;

// The Phantasm afterimage. The full class lives in wn/falco/wn_falco_illusion.h, whose builder specializations cannot coexist
// with this Fighter-based builder unit, so the fighter unit sees an opaque shell of the right size.
class wnFalcoIllusion : public Weapon {
    u8 m_unreconstructed[0x1FA0 - sizeof(Weapon)];
public:
    wnFalcoIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoIllusionData* data);
    virtual ~wnFalcoIllusion();
    void activate(s32 founderTaskId, s32 unk1, s32 team, float lr, s32 unk2, s32 unk3);
};
static_assert(sizeof(wnFalcoIllusion) == 0x1FA0, "Falco Illusion layout is wrong!");

typedef wnArticleHierarchy<wnFoxLandMaster, 1, soInstancePoolRoot> ftFalcoLandMasterPool;
typedef wnArticleHierarchy<wnFalcoIllusion, 2, ftFalcoLandMasterPool> ftFalcoIllusionPool;
typedef wnArticleHierarchy<wnFalcoBlasterBullet, 5, ftFalcoIllusionPool> ftFalcoBulletPool;
typedef wnArticleHierarchy<wnFalcoBlaster, 1, ftFalcoBulletPool> ftFalcoBlasterPool;

class ftFalcoArticleMediator : public soArticleMediator {
    ftFalcoBlasterPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftFalcoArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftFalcoArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftFalcoArticleMediator, 5> ftFalcoArticleManageModuleBuilder;
static_assert(sizeof(ftFalcoArticleManageModuleBuilder) == 0x15040, "Falco article manage builder layout is wrong!");
