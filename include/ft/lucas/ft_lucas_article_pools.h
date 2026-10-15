#pragma once

// The articles Lucas owns and the mediator that hands them out: PK Flash, PK Fire (two bolts), PK Thunder (two heads), PK
// Starstorm (sixteen stars) and the Rope Snake. The pools are chained base first, the last article at the lowest offset
// (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
#include <wn/lucas/wn_lucas_pk_flash.h>

// The fighter unit only builds the articles into its pools; their definitions live in the article units (see
// src/mo_fighter/ft_lucas/wn_lucas_*.cpp). Like R.O.B.'s pools this header declares them as opaque shells of the right size,
// so the native-builder macros of the article units do not leak into the fighter unit.
struct wnLucasPKFireData;
struct wnLucasPKThunderData;
struct wnLucasPKStarStormData;
struct wnLucasHimohebiData;
class Vec2f;
class Vec3f;

class wnLucasPKFire : public Weapon {
    u8 m_unreconstructed[0x2408 - sizeof(Weapon)];
public:
    wnLucasPKFire(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKFireData* data);
    virtual ~wnLucasPKFire();
    void activate(int founderTaskId, int team, Vec3f* pos, Vec2f* speed, float lr, float rot);
};
static_assert(sizeof(wnLucasPKFire) == 0x2408, "PK Fire layout is wrong!");

class wnLucasPKThunder : public Weapon {
    u8 m_unreconstructed[0xDE10 - sizeof(Weapon)];
public:
    wnLucasPKThunder(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKThunderData* data);
    virtual ~wnLucasPKThunder();
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
};
static_assert(sizeof(wnLucasPKThunder) == 0xDE10, "PK Thunder layout is wrong!");

class wnLucasPKStarStorm : public Weapon {
    u8 m_unreconstructed[0x1C10 - sizeof(Weapon)];
public:
    wnLucasPKStarStorm(s32 articleId, const wnArticleBuildInfo& info, const wnLucasPKStarStormData* data);
    virtual ~wnLucasPKStarStorm();
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
};
static_assert(sizeof(wnLucasPKStarStorm) == 0x1C10, "PK Starstorm layout is wrong!");

class wnLucasHimohebi : public Weapon {
    u8 m_unreconstructed[0x2104 - sizeof(Weapon)];
public:
    wnLucasHimohebi(s32 articleId, const wnArticleBuildInfo& info, const wnLucasHimohebiData* data);
    virtual ~wnLucasHimohebi();
    void activate(int founderTaskId, int kind, int team, float lr, int attach, int plain);
};
static_assert(sizeof(wnLucasHimohebi) == 0x2104, "Rope Snake layout is wrong!");

typedef wnArticleHierarchy<wnLucasHimohebi, 1, soInstancePoolRoot> ftLucasHimohebiPool;
typedef wnArticleHierarchy<wnLucasPKStarStorm, 16, ftLucasHimohebiPool> ftLucasStarStormPool;
typedef wnArticleHierarchy<wnLucasPKThunder, 2, ftLucasStarStormPool> ftLucasThunderPool;
typedef wnArticleHierarchy<wnLucasPKFire, 2, ftLucasThunderPool> ftLucasFirePool;
typedef wnArticleHierarchy<wnLucasPKFlash, 1, ftLucasFirePool> ftLucasFlashPool;

class ftLucasArticleMediator : public soArticleMediator {
    ftLucasFlashPool m_pools;
    u8 m_autoRecycle;
public:
    ftLucasArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftLucasArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The list of the articles of the module lives in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef wnArticleManageModuleBuilder<ftLucasArticleMediator, 5> ftLucasArticleManageModuleBuilder;
static_assert(sizeof(ftLucasArticleManageModuleBuilder) == 0x40814, "Lucas article manage builder layout is wrong!");
