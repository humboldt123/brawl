#pragma once

// The articles Pikachu owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
class ftPikachu;
class soCollisionAttackModule;
struct wnPikachuThunderJoltData;
struct wnPikachuCloudData;
struct wnPikachuVortexData;
struct wnPikachuThunderData;

// The Thunder Jolt (the class is in the REL). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnPikachuThunderJolt : public Weapon {
    u8 m_unreconstructed[0x1F94 - sizeof(Weapon)];
public:
    wnPikachuThunderJolt(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuThunderJoltData* data);
    virtual ~wnPikachuThunderJolt();
};
static_assert(sizeof(wnPikachuThunderJolt) == 0x1F94, "wnPikachuThunderJolt layout is wrong!");

// The Thunder cloud (the class is in the REL). Opaque shell for the same reason.
class wnPikachuCloud : public Weapon {
    u8 m_unreconstructed[0xA5FC - sizeof(Weapon)];
public:
    wnPikachuCloud(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuCloudData* data);
    virtual ~wnPikachuCloud();
    // Called by the Thunder status processes (names from the module map, signatures are HYPOTHESES).
    void activate(int founderTaskId, int team, const Vec3f* position, int etcResId, ftPikachu* owner);
    bool thunder_hitcheck(Vec3f* position);
    void deactive_thunder(int thunder);
    void ref_log_group(soCollisionAttackModule* attack);
    void unref_log_group(soCollisionAttackModule* attack);
    void set_pikachu_pos(float y);
};
static_assert(sizeof(wnPikachuCloud) == 0xA5FC, "wnPikachuCloud layout is wrong!");

// The Final Smash vortex. Opaque shell for the same reason.
class wnPikachuVortex : public Weapon {
    u8 m_unreconstructed[0x1FC8 - sizeof(Weapon)];
public:
    wnPikachuVortex(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuVortexData* data);
    virtual ~wnPikachuVortex();
    void activate(int founderTaskId, int finalResId, int team, const Vec3f* position);
};
static_assert(sizeof(wnPikachuVortex) == 0x1FC8, "wnPikachuVortex layout is wrong!");

// The Thunder bolt (the class is in sora_melee). Opaque shell for the same reason.
class wnPikachuThunder : public Weapon {
    u8 m_unreconstructed[0x1FC4 - sizeof(Weapon)];
public:
    wnPikachuThunder(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuThunderData* data);
    virtual ~wnPikachuThunder();
    bool activeArticle(soModuleAccesser* acc);
};
static_assert(sizeof(wnPikachuThunder) == 0x1FC4, "wnPikachuThunder layout is wrong!");

typedef wnArticleHierarchy<wnPikachuThunder, 3, soInstancePoolRoot> ftPikachuThunderPool;
typedef wnArticleHierarchy<wnPikachuVortex, 1, ftPikachuThunderPool> ftPikachuVortexPool;
typedef wnArticleHierarchy<wnPikachuCloud, 2, ftPikachuVortexPool> ftPikachuCloudPool;
typedef wnArticleHierarchy<wnPikachuThunderJolt, 3, ftPikachuCloudPool> ftPikachuThunderJoltPool;

class ftPikachuArticleMediator : public soArticleMediator {
    ftPikachuThunderJoltPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPikachuArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPikachuArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftPikachuArticleMediator, 5> ftPikachuArticleManageModuleBuilder;
static_assert(sizeof(ftPikachuArticleManageModuleBuilder) == 0x22AE8, "Pikachu article manage builder layout is wrong!");
