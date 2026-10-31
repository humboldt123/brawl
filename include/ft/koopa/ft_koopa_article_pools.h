#pragma once

// The articles Koopa owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnKoopaBreathData;

// Bowser's fire breath (the class is in sora_melee). Opaque shell for the same reason as the other fighter pools.
class wnKoopaBreath : public Weapon {
    u8 m_unreconstructed[0x1F80 - sizeof(Weapon)];
public:
    wnKoopaBreath(s32 articleId, const wnArticleBuildInfo& info, const wnKoopaBreathData* data);
    virtual ~wnKoopaBreath();
};
static_assert(sizeof(wnKoopaBreath) == 0x1F80, "wnKoopaBreath layout is wrong!");

typedef wnArticleHierarchy<wnKoopaBreath, 6, soInstancePoolRoot> ftKoopaBreathPool;

class ftKoopaArticleMediator : public soArticleMediator {
    ftKoopaBreathPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftKoopaArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftKoopaArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftKoopaArticleMediator, 5> ftKoopaArticleManageModuleBuilder;
static_assert(sizeof(ftKoopaArticleManageModuleBuilder) == 0xBDFC, "Koopa article manage builder layout is wrong!");
