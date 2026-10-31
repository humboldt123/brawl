#pragma once

// The articles Popo owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
class wnPopoIceBergWind;
struct wnPopoBlizzardData;
struct wnPopoIceShotData;
struct wnPopoIceBergData;
struct wnPopoRubberData;
struct wnPopoIceBergWindData;

// The blizzard of the final smash (wn_popo_blizzard). Opaque shell: the class is a weapon builder that cannot coexist with this unit.
class wnPopoBlizzard : public Weapon {
    u8 m_unreconstructed[0x1F4C - sizeof(Weapon)];
public:
    wnPopoBlizzard(s32 articleId, const wnArticleBuildInfo& info, const wnPopoBlizzardData* data);
    virtual ~wnPopoBlizzard();
};
static_assert(sizeof(wnPopoBlizzard) == 0x1F4C, "wnPopoBlizzard layout is wrong!");

// The ice shot of the side special (the class is in sora_melee). Opaque shell for the same reason.
class wnPopoIceShot : public Weapon {
    u8 m_unreconstructed[0x23D0 - sizeof(Weapon)];
public:
    wnPopoIceShot(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceShotData* data);
    virtual ~wnPopoIceShot();
    void shoot();
};
static_assert(sizeof(wnPopoIceShot) == 0x23D0, "wnPopoIceShot layout is wrong!");

// The ice berg of the down special (wn_popo_ice_berg). Opaque shell for the same reason.
class wnPopoIceBerg : public Weapon {
    u8 m_unreconstructed[0x277C - sizeof(Weapon)];
public:
    wnPopoIceBerg(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceBergData* data);
    virtual ~wnPopoIceBerg();
    static bool activeArticle(wnPopoIceBergWind* wind, soModuleAccesser* acc);
};
static_assert(sizeof(wnPopoIceBerg) == 0x277C, "wnPopoIceBerg layout is wrong!");

// The rubber band of the up special (wn_popo_rubber). Opaque shell for the same reason.
class wnPopoRubber : public Weapon {
    u8 m_unreconstructed[0x210C - sizeof(Weapon)];
public:
    wnPopoRubber(s32 articleId, const wnArticleBuildInfo& info, const wnPopoRubberData* data);
    virtual ~wnPopoRubber();
};
static_assert(sizeof(wnPopoRubber) == 0x210C, "wnPopoRubber layout is wrong!");

// The wind the ice berg blows (wn/popo/wn_popo_ice_berg_wind.h). Opaque shell for the same reason.
class wnPopoIceBergWind : public Weapon {
    u8 m_unreconstructed[0x201C - sizeof(Weapon)];
public:
    wnPopoIceBergWind(s32 articleId, const wnArticleBuildInfo& info, const wnPopoIceBergWindData* data);
    virtual ~wnPopoIceBergWind();
};
static_assert(sizeof(wnPopoIceBergWind) == 0x201C, "wnPopoIceBergWind layout is wrong!");

typedef wnArticleHierarchy<wnPopoIceBergWind, 1, soInstancePoolRoot> ftPopoIceBergWindPool;
typedef wnArticleHierarchy<wnPopoRubber, 1, ftPopoIceBergWindPool> ftPopoRubberPool;
typedef wnArticleHierarchy<wnPopoIceBerg, 1, ftPopoRubberPool> ftPopoIceBergPool;
typedef wnArticleHierarchy<wnPopoIceShot, 5, ftPopoIceBergPool> ftPopoIceShotPool;
typedef wnArticleHierarchy<wnPopoBlizzard, 3, ftPopoIceShotPool> ftPopoBlizzardPool;

class ftPopoArticleMediator : public soArticleMediator {
    ftPopoBlizzardPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPopoArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPopoArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftPopoArticleMediator, 5> ftPopoArticleManageModuleBuilder;
static_assert(sizeof(ftPopoArticleManageModuleBuilder) == 0x17ACC, "Popo article manage builder layout is wrong!");
