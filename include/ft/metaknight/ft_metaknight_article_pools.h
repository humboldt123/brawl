#pragma once

// The article Meta Knight owns and the mediator that hands it out: the Mantle (one, the cape of the Final Smash). The pool is
// chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

struct wnMetaknightMantleData;

// The Mantle. The full class lives in wn/metaknight/wn_metaknight_mantle.h (a weapon builder specialization that cannot
// coexist with this Fighter-based builder unit), so the fighter unit sees an opaque shell of the right size.
class wnMetaknightMantle : public Weapon {
    u8 m_unreconstructed[0x20A4 - sizeof(Weapon)];
public:
    wnMetaknightMantle(s32 articleId, const wnArticleBuildInfo& info, const wnMetaknightMantleData* data);
    virtual ~wnMetaknightMantle();
    void activate(int founderTaskId, int resourceId, int unkC, int team);
};
static_assert(sizeof(wnMetaknightMantle) == 0x20A4, "Metaknight mantle layout is wrong!");

typedef wnArticleHierarchy<wnMetaknightMantle, 1, soInstancePoolRoot> ftMetaknightMantlePool;

class ftMetaknightArticleMediator : public soArticleMediator {
    ftMetaknightMantlePool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftMetaknightArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftMetaknightArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftMetaknightArticleMediator, 5> ftMetaknightArticleManageModuleBuilder;
static_assert(sizeof(ftMetaknightArticleManageModuleBuilder) == 0x2178, "Metaknight article manage builder layout is wrong!");
