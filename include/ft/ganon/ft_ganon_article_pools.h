#pragma once

// The articles Ganon owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h): the beast (article 0) holds the pool of the simple article (article 1).
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
#include <wn/ganon/wn_ganon_beast.h>
#define WN_SIMPLE_AFTER_ARTICLE_BUILDERS
// Only the simple article uses five model observers; its teardown calls sora_melee's event unit teardown out of
// line (the same declaration as wn/wn_simple_builder.h and ft_snake_article_pools.h).
template <> class soEventUnitWithWorkArea<soModelEventObserver, 5> : public soEventUnitImpl<soModelEventObserver> {
    soInstanceManagerFullPropertyVector<soModelEventObserver*, 5> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soModelEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
#include <wn/wn_simple.h>

typedef wnArticleHierarchy<wnSimple, 1, soInstancePoolRoot> ftGanonSimplePool;
typedef wnArticleHierarchy<wnGanonBeast, 1, ftGanonSimplePool> ftGanonBeastPool;

class ftGanonArticleMediator : public soArticleMediator {
    ftGanonBeastPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftGanonArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftGanonArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftGanonArticleMediator, 5> ftGanonArticleManageModuleBuilder;
static_assert(sizeof(ftGanonArticleManageModuleBuilder) == 0x4630, "Ganon article manage builder layout is wrong!");
