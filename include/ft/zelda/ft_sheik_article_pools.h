#pragma once

// The articles Sheik owns and the mediator that hands them out (see include/wn/wn_article_pool.h). The needles are
// sora_melee/blob shells; the chain, the vanish (Fusin) and the light bow of the Final Smash are built here.
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#include <wn/sheik/wn_sheik_needle.h>
#include <wn/sheik/wn_sheik_needle_have.h>
#include <wn/sheik/wn_sheik_iron_wire.h>
#include <wn/zelda/wn_sheik_fusin.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

struct wnZeldaLightingBowData;
struct wnZeldaLightingBowArrowData;

// The light bow of the Final Smash (the class is in sora_melee).
class wnZeldaLightingBow : public Weapon {
    u8 m_unreconstructed[0x1F28 - sizeof(Weapon)];
public:
    wnZeldaLightingBow(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaLightingBowData* data);
    virtual ~wnZeldaLightingBow();
    void activate(int founderTaskId, int resourceId, int team, int kind);
};
static_assert(sizeof(wnZeldaLightingBow) == 0x1F28, "wnZeldaLightingBow layout is wrong!");

// The light arrow of the Final Smash (the class is in sora_melee).
class wnZeldaLightingBowArrow : public Weapon {
    u8 m_unreconstructed[0x1F30 - sizeof(Weapon)];
public:
    wnZeldaLightingBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaLightingBowArrowData* data);
    virtual ~wnZeldaLightingBowArrow();
    void activate(int founderTaskId, int resourceId, int team, int kind);
};
static_assert(sizeof(wnZeldaLightingBowArrow) == 0x1F30, "wnZeldaLightingBowArrow layout is wrong!");

typedef wnArticleHierarchy<wnZeldaLightingBowArrow, 1, soInstancePoolRoot> ftSheikLightingBowArrowPool;
typedef wnArticleHierarchy<wnZeldaLightingBow, 1, ftSheikLightingBowArrowPool> ftSheikLightingBowPool;
typedef wnArticleHierarchy<wnSheikFusin, 1, ftSheikLightingBowPool> ftSheikFusinPool;
typedef wnArticleHierarchy<wnSheikIronWire, 1, ftSheikFusinPool> ftSheikIronWirePool;
typedef wnArticleHierarchy<wnSheikNeedleHave, 1, ftSheikIronWirePool> ftSheikNeedleHavePool;
typedef wnArticleHierarchy<wnSheikNeedle, 8, ftSheikNeedleHavePool> ftSheikNeedlePool;

class ftSheikArticleMediator : public soArticleMediator {
    ftSheikNeedlePool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftSheikArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftSheikArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftSheikArticleMediator, 5> ftSheikArticleManageModuleBuilder;
