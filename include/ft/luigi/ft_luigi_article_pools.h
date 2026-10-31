#pragma once

// The articles Luigi owns and the mediator that hands them out: the fireballs (two, shared with Mario) and the Negative Zone
// (one). The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec2f;
class Vec3f;
struct wnMarioFireballData;
struct wnLuigiNegativeZoneData;

// The fireball. The full class lives in wn/luigi/wn_luigi_fireball.h (a weapon builder specialization that cannot coexist with
// this Fighter-based builder unit), so the fighter unit sees an opaque shell of the right size.
class wnLuigiFireball : public Weapon {
    u8 m_unreconstructed[0x1F44 - sizeof(Weapon)];
public:
    wnLuigiFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data);
    virtual ~wnLuigiFireball();
};
static_assert(sizeof(wnLuigiFireball) == 0x1F44, "Luigi fireball layout is wrong!");

// The Negative Zone (the down special). Same reason for the shell (wn/luigi/wn_luigi_negative_zone.h).
class wnLuigiNegativeZone : public Weapon {
    u8 m_unreconstructed[0x2090 - sizeof(Weapon)];
public:
    wnLuigiNegativeZone(s32 articleId, const wnArticleBuildInfo& info, const wnLuigiNegativeZoneData* data);
    virtual ~wnLuigiNegativeZone();
    void activate(int founderTaskId, int resourceId, int team, float lr);
    void toFly(Vec3f* position);
};
static_assert(sizeof(wnLuigiNegativeZone) == 0x2090, "Luigi negative zone layout is wrong!");

typedef wnArticleHierarchy<wnLuigiNegativeZone, 1, soInstancePoolRoot> ftLuigiNegativeZonePool;
typedef wnArticleHierarchy<wnLuigiFireball, 2, ftLuigiNegativeZonePool> ftLuigiFireballPool;

class ftLuigiArticleMediator : public soArticleMediator {
    ftLuigiFireballPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftLuigiArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftLuigiArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftLuigiArticleMediator, 5> ftLuigiArticleManageModuleBuilder;
static_assert(sizeof(ftLuigiArticleManageModuleBuilder) == 0x6000, "Luigi article manage builder layout is wrong!");
