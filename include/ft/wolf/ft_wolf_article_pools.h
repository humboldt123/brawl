#pragma once

// The articles Wolf owns and the mediator that hands them out: the blaster, its bullets (five), the Phantasm afterimages
// (two) and the Landmaster (Final Smash). The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// Full article types let the fighter own their pools and teardown.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#include <wn/wolf/wn_wolf_illusion.h>
#include <wn/wolf/wn_wolf_blaster_bullet.h>
#include <wn/wolf/wn_wolf_blaster.h>
#include <wn/fox/wn_fox_land_master.h>

typedef wnArticleHierarchy<wnFoxLandMaster, 1, soInstancePoolRoot> ftWolfLandMasterPool;
typedef wnArticleHierarchy<wnWolfIllusion, 2, ftWolfLandMasterPool> ftWolfIllusionPool;
typedef wnArticleHierarchy<wnWolfBlasterBullet, 5, ftWolfIllusionPool> ftWolfBulletPool;
typedef wnArticleHierarchy<wnWolfBlaster, 1, ftWolfBulletPool> ftWolfBlasterPool;

class ftWolfArticleMediator : public soArticleMediator {
    ftWolfBlasterPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftWolfArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftWolfArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftWolfArticleMediator, 5> ftWolfArticleManageModuleBuilder;
static_assert(sizeof(ftWolfArticleManageModuleBuilder) == 0x14CDC, "Wolf article manage builder layout is wrong!");
