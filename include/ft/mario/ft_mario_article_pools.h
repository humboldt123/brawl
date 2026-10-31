#pragma once

// The articles Mario owns and the mediator that hands them out: the cape (one), the fireballs (five), the Pump (F.L.U.D.D., one)
// and the Final Smash flame (one). The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// Full article types let the fighter own their pools and teardown.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_MARIO_FIREBALL_CTOR_OUTLINE
#include <wn/wn_module_accesser_builder.h>
// Instantiate F.L.U.D.D.'s shared script teardown before its first article use.
#pragma push
#pragma dont_inline on
template soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<0, 1, 3, 0, 0, 0, 0, 8> >::~soAnimCmdControlUnitBuilder();
template soInsideEventManageModuleBuilder<soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>, wnInsideEventManageModuleTypes>::~soInsideEventManageModuleBuilder();
#pragma pop
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 1);
#include <wn/mario/wn_mario_pump.h>
#include <wn/mario/wn_mario_mantle.h>
#include <wn/mario/wn_mario_huge_flame.h>
#include <wn/mario/wn_mario_fireball.h>

typedef wnArticleHierarchy<wnMarioHugeFlame, 1, soInstancePoolRoot> ftMarioHugeFlamePool;
typedef wnArticleHierarchy<wnMarioPump, 1, ftMarioHugeFlamePool> ftMarioPumpPool;
typedef wnArticleHierarchy<wnMarioFireball, 5, ftMarioPumpPool> ftMarioFireballPool;
typedef wnArticleHierarchy<wnMarioMantle, 1, ftMarioFireballPool> ftMarioMantlePool;

class ftMarioArticleMediator : public soArticleMediator {
    ftMarioMantlePool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftMarioArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftMarioArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftMarioArticleMediator, 5> ftMarioArticleManageModuleBuilder;
static_assert(sizeof(ftMarioArticleManageModuleBuilder) == 0x25AA8, "Mario article manage builder layout is wrong!");
