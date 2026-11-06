#pragma once

// The articles PokeFushigisou owns (Razor Leaf, Bullet Seed and the Vine Whip / tether vine) and the mediator that hands
// them out. The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/poke/wn_poke_fushigisou_seed.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnPokeFushigisouLeafCutterData;
struct wnPokeFushigisouVineData;

// Razor Leaf and the vine are built in their own units of the REL (wn_poke_fushigisou_leaf_cutter, wn_poke_fushigisou_vine);
// the fighter unit sees opaque shells of the same extent (the strides of the article holders).
class wnPokeFushigisouLeafCutter : public Weapon {
    u8 m_unreconstructed[0x1700 - sizeof(Weapon)];
public:
    wnPokeFushigisouLeafCutter(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouLeafCutterData* data);
    virtual ~wnPokeFushigisouLeafCutter();
    void activate(int founderTaskId, int resourceId, int team, Vec3f* position, float lr, float speed, float angle);
};
static_assert(sizeof(wnPokeFushigisouLeafCutter) == 0x1700, "wnPokeFushigisouLeafCutter extent is wrong!");

class wnPokeFushigisouVine : public Weapon {
    u8 m_unreconstructed[0x1F7C - sizeof(Weapon)];
public:
    wnPokeFushigisouVine(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouVineData* data);
    virtual ~wnPokeFushigisouVine();
    void activate(int founderTaskId, int resourceId, int unkC, int unk10, int unk14, int team, Vec3f* position, float lr,
                  bool attach);
};
static_assert(sizeof(wnPokeFushigisouVine) == 0x1F7C, "wnPokeFushigisouVine extent is wrong!");

typedef wnArticleHierarchy<wnPokeFushigisouVine, 1, soInstancePoolRoot> ftPokeFushigisouVinePool;
typedef wnArticleHierarchy<wnPokeFushigisouSeed, 3, ftPokeFushigisouVinePool> ftPokeFushigisouSeedPool;
typedef wnArticleHierarchy<wnPokeFushigisouLeafCutter, 2, ftPokeFushigisouSeedPool> ftPokeFushigisouLeafCutterPool;

class ftPokeFushigisouArticleMediator : public soArticleMediator {
    ftPokeFushigisouLeafCutterPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPokeFushigisouArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPokeFushigisouArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftPokeFushigisouArticleMediator, 5> ftPokeFushigisouArticleManageModuleBuilder;
static_assert(sizeof(ftPokeFushigisouArticleManageModuleBuilder) == 0x9278, "PokeFushigisou article manage builder layout is wrong!");
