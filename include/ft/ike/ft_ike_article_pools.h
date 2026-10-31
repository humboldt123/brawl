#pragma once

// The articles Ike owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnIkeSwordData;

// Ike's sword (wn/ike/wn_ike_sword.h). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnIkeSword : public Weapon {
    u8 m_unreconstructed[0x2300 - sizeof(Weapon)];
public:
    wnIkeSword(s32 articleId, const wnArticleBuildInfo& info, const wnIkeSwordData* data);
    virtual ~wnIkeSword();
    void activate(int founderTaskId, int resourceId, int team, Vec3f* position, float lr, int situationKind, bool followUp, bool unk);
};
static_assert(sizeof(wnIkeSword) == 0x2300, "wnIkeSword layout is wrong!");

typedef wnArticleHierarchy<wnIkeSword, 1, soInstancePoolRoot> ftIkeSwordPool;

class ftIkeArticleMediator : public soArticleMediator {
    ftIkeSwordPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftIkeArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftIkeArticleMediator();
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
FT_DOL_ARRAY_VECTOR(soArticle*, 1);

typedef wnArticleManageModuleBuilder<ftIkeArticleMediator, 1> ftIkeArticleManageModuleBuilder;
static_assert(sizeof(ftIkeArticleManageModuleBuilder) == 0x2384, "Ike article manage builder layout is wrong!");
