#pragma once

// The articles Zelda owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnZeldaDeinData;
struct wnZeldaDeinSData;
struct wnZeldaLightingBowData;
struct wnZeldaLightingBowArrowData;

// Din's Fire (the class is in the REL). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnZeldaDein : public Weapon {
    u8 m_unreconstructed[0x20D8 - sizeof(Weapon)];
public:
    wnZeldaDein(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaDeinData* data);
    virtual ~wnZeldaDein();
    void activate(int founderTaskId, int team, const Vec3f* position, float lr);
};
static_assert(sizeof(wnZeldaDein) == 0x20D8, "wnZeldaDein layout is wrong!");

// The small Din's Fire (wn/zelda/wn_zelda_dein_s.h). Opaque shell for the same reason.
class wnZeldaDeinS : public Weapon {
    u8 m_unreconstructed[0x1F18 - sizeof(Weapon)];
public:
    wnZeldaDeinS(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaDeinSData* data);
    virtual ~wnZeldaDeinS();
    bool activeArticle(soModuleAccesser* acc);
};
static_assert(sizeof(wnZeldaDeinS) == 0x1F18, "wnZeldaDeinS layout is wrong!");

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

typedef wnArticleHierarchy<wnZeldaLightingBowArrow, 1, soInstancePoolRoot> ftZeldaLightingBowArrowPool;
typedef wnArticleHierarchy<wnZeldaLightingBow, 1, ftZeldaLightingBowArrowPool> ftZeldaLightingBowPool;
typedef wnArticleHierarchy<wnZeldaDeinS, 3, ftZeldaLightingBowPool> ftZeldaDeinSPool;
typedef wnArticleHierarchy<wnZeldaDein, 3, ftZeldaDeinSPool> ftZeldaDeinPool;

class ftZeldaArticleMediator : public soArticleMediator {
    ftZeldaDeinPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftZeldaArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftZeldaArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftZeldaArticleMediator, 5> ftZeldaArticleManageModuleBuilder;
static_assert(sizeof(ftZeldaArticleManageModuleBuilder) == 0xFF40, "Zelda article manage builder layout is wrong!");
