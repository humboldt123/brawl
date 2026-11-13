#pragma once

// The articles Popo owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// Full article types let the fighter own their pools and teardown.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#include <wn/popo/wn_popo_blizzard.h>
#include <wn/popo/wn_popo_ice_shot.h>
#include <wn/popo/wn_popo_ice_berg.h>
#include <wn/popo/wn_popo_rubber.h>
#include <wn/popo/wn_popo_ice_berg_wind.h>

typedef wnArticleHierarchy<wnPopoIceBergWind, 1, soInstancePoolRoot> ftPopoIceBergWindPool;
typedef wnArticleHierarchy<wnPopoRubber, 1, ftPopoIceBergWindPool> ftPopoRubberPool;
typedef wnArticleHierarchy<wnPopoIceBerg, 1, ftPopoRubberPool> ftPopoIceBergPool;
typedef wnArticleHierarchy<wnPopoIceShot, 5, ftPopoIceBergPool> ftPopoIceShotPool;
typedef wnArticleHierarchy<wnPopoBlizzard, 3, ftPopoIceShotPool> ftPopoBlizzardPool;

class ftPopoArticleMediator : public soArticleMediator {
    ftPopoBlizzardPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPopoArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPopoArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftPopoArticleMediator, 5> ftPopoArticleManageModuleBuilder;
static_assert(sizeof(ftPopoArticleManageModuleBuilder) == 0x17ACC, "Popo article manage builder layout is wrong!");
