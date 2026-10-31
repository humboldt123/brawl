#pragma once

// The articles Peach owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#include <wn/peach/wn_peach_kinopio.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
class BaseItem;
class soArticle;

// HYPOTHESIS: the helper of the generic article mediator that creates an item as an article of a slot.
class soArticleMediatorHelper {
public:
    static BaseItem* createItem(int itemKind, int articleId, int ownerTaskId);
    static soArticle* castArticle(BaseItem* item);
};
struct wnPeachKassarData;

// Peach's Kassar (the Final Smash castle, wn/peach/wn_peach_kassar.h). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnPeachKassar : public Weapon {
    u8 m_unreconstructed[0x20E4 - sizeof(Weapon)];
public:
    wnPeachKassar(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKassarData* data);
    virtual ~wnPeachKassar();
    void activate(int founderTaskId, int resourceId, int unkC, int team);
};
static_assert(sizeof(wnPeachKassar) == 0x20E4, "wnPeachKassar layout is wrong!");

typedef wnArticleHierarchy<wnPeachKinopio, 1, soInstancePoolRoot> ftPeachKinopioPool;
typedef wnArticleHierarchy<wnPeachKassar, 1, ftPeachKinopioPool> ftPeachKassarPool;

class ftPeachArticleMediator : public soArticleMediator {
    ftPeachKassarPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPeachArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPeachArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftPeachArticleMediator, 5> ftPeachArticleManageModuleBuilder;
static_assert(sizeof(ftPeachArticleManageModuleBuilder) == 0xFEBC, "Peach article manage builder layout is wrong!");
