#pragma once

// The articles Lucario owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnLucarioAuraBallData;
struct wnLucarioQigongData;

// The aura ball (the class is in sora_melee). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnLucarioAuraBall : public Weapon {
    u8 m_unreconstructed[0x2050 - sizeof(Weapon)];
public:
    wnLucarioAuraBall(s32 articleId, const wnArticleBuildInfo& info, const wnLucarioAuraBallData* data);
    virtual ~wnLucarioAuraBall();
};
static_assert(sizeof(wnLucarioAuraBall) == 0x2050, "wnLucarioAuraBall layout is wrong!");

// The Aura Sphere / force palm wave (wn/lucario/wn_lucario_qigong.h). Opaque shell for the same reason.
class wnLucarioQigong : public Weapon {
    u8 m_unreconstructed[0x1E3C - sizeof(Weapon)];
public:
    wnLucarioQigong(s32 articleId, const wnArticleBuildInfo& info, const wnLucarioQigongData* data);
    virtual ~wnLucarioQigong();
    void activate(int founderTaskId, int team, const Vec3f* position, float a, float b);
};
static_assert(sizeof(wnLucarioQigong) == 0x1E3C, "wnLucarioQigong layout is wrong!");

typedef wnArticleHierarchy<wnLucarioQigong, 1, soInstancePoolRoot> ftLucarioQigongPool;
typedef wnArticleHierarchy<wnLucarioAuraBall, 3, ftLucarioQigongPool> ftLucarioAuraBallPool;

class ftLucarioArticleMediator : public soArticleMediator {
    ftLucarioAuraBallPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftLucarioArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftLucarioArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftLucarioArticleMediator, 5> ftLucarioArticleManageModuleBuilder;
static_assert(sizeof(ftLucarioArticleManageModuleBuilder) == 0x801C, "Lucario article manage builder layout is wrong!");
