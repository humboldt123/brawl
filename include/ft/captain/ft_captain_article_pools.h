#pragma once

// The articles Captain owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnCaptainBlueFalconData;
struct wnCaptainFinalBgData;

// The Falcon of the Final Smash (wn/captain/wn_captain_blue_falcon.h). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnCaptainBlueFalcon : public Weapon {
    u8 m_unreconstructed[0x2110 - sizeof(Weapon)];
public:
    wnCaptainBlueFalcon(s32 articleId, const wnArticleBuildInfo& info, const wnCaptainBlueFalconData* data);
    virtual ~wnCaptainBlueFalcon();
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float scale, bool attached);
};
static_assert(sizeof(wnCaptainBlueFalcon) == 0x2110, "wnCaptainBlueFalcon layout is wrong!");

// The background of the Final Smash (wn/captain/wn_captain_final_bg.h). Opaque shell for the same reason.
class wnCaptainFinalBg : public Weapon {
    u8 m_unreconstructed[0x1F28 - sizeof(Weapon)];
public:
    wnCaptainFinalBg(s32 articleId, const wnArticleBuildInfo& info, const wnCaptainFinalBgData* data);
    virtual ~wnCaptainFinalBg();
    void activate(int founderTaskId, int resourceId, int team);
};
static_assert(sizeof(wnCaptainFinalBg) == 0x1F28, "wnCaptainFinalBg layout is wrong!");

typedef wnArticleHierarchy<wnCaptainFinalBg, 1, soInstancePoolRoot> ftCaptainFinalBgPool;
typedef wnArticleHierarchy<wnCaptainBlueFalcon, 1, ftCaptainFinalBgPool> ftCaptainBlueFalconPool;

class ftCaptainArticleMediator : public soArticleMediator {
    ftCaptainBlueFalconPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftCaptainArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftCaptainArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftCaptainArticleMediator, 5> ftCaptainArticleManageModuleBuilder;
static_assert(sizeof(ftCaptainArticleManageModuleBuilder) == 0x4118, "Captain article manage builder layout is wrong!");
