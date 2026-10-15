#pragma once

// The articles Ness owns and the mediator that hands them out: PK Flash, PK Fire (two flames), PK Thunder (two heads), PK
// Starstorm (sixteen stars), the Yo-Yo and its head. The pools are chained base first, the last article at the lowest offset
// (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
#include <wn/ness/wn_ness_pk_flash.h>

// The fighter unit only builds the articles into its pools; their definitions live in the article units (see
// src/mo_fighter/ft_ness/wn_ness_*.cpp). Like R.O.B.'s pools this header declares them as opaque shells of the right size,
// so the native-builder macros of the article units do not leak into the fighter unit.
struct wnNessPKFireData;
struct wnNessPKThunderData;
struct wnNessPKStarStormData;
struct wnNessYoyoData;
struct wnNessYoyoHeadData;
class Vec2f;
class Vec3f;

class wnNessPKFire : public Weapon {
    u8 m_unreconstructed[0x2408 - sizeof(Weapon)];
public:
    wnNessPKFire(s32 articleId, const wnArticleBuildInfo& info, const wnNessPKFireData* data);
    virtual ~wnNessPKFire();
    void activate(int founderTaskId, int team, Vec3f* pos, Vec2f* speed, float lr, float rot);
};
static_assert(sizeof(wnNessPKFire) == 0x2408, "PK Fire layout is wrong!");

class wnNessPKThunder : public Weapon {
    u8 m_unreconstructed[0xDE10 - sizeof(Weapon)];
public:
    wnNessPKThunder(s32 articleId, const wnArticleBuildInfo& info, const wnNessPKThunderData* data);
    virtual ~wnNessPKThunder();
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
};
static_assert(sizeof(wnNessPKThunder) == 0xDE10, "PK Thunder layout is wrong!");

class wnNessPKStarStorm : public Weapon {
    u8 m_unreconstructed[0x1C10 - sizeof(Weapon)];
public:
    wnNessPKStarStorm(s32 articleId, const wnArticleBuildInfo& info, const wnNessPKStarStormData* data);
    virtual ~wnNessPKStarStorm();
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
};
static_assert(sizeof(wnNessPKStarStorm) == 0x1C10, "PK Starstorm layout is wrong!");

class wnNessYoyo : public Weapon {
    u8 m_unreconstructed[0x2138 - sizeof(Weapon)];
public:
    wnNessYoyo(s32 articleId, const wnArticleBuildInfo& info, const wnNessYoyoData* data);
    virtual ~wnNessYoyo();
    void activate(int founderTaskId, int kind, int team, bool flag);
};
static_assert(sizeof(wnNessYoyo) == 0x2138, "Yo-Yo layout is wrong!");

class wnNessYoyoHead : public Weapon {
    u8 m_unreconstructed[0x21B8 - sizeof(Weapon)];
public:
    wnNessYoyoHead(s32 articleId, const wnArticleBuildInfo& info, const wnNessYoyoHeadData* data);
    virtual ~wnNessYoyoHead();
    void activate(int founderTaskId, int kind, int team, Vec3f* pos, float lr, bool flag);
};
static_assert(sizeof(wnNessYoyoHead) == 0x21B8, "Yo-Yo head layout is wrong!");

typedef wnArticleHierarchy<wnNessYoyoHead, 1, soInstancePoolRoot> ftNessYoyoHeadPool;
typedef wnArticleHierarchy<wnNessYoyo, 1, ftNessYoyoHeadPool> ftNessYoyoPool;
typedef wnArticleHierarchy<wnNessPKStarStorm, 16, ftNessYoyoPool> ftNessStarStormPool;
typedef wnArticleHierarchy<wnNessPKThunder, 2, ftNessStarStormPool> ftNessThunderPool;
typedef wnArticleHierarchy<wnNessPKFire, 2, ftNessThunderPool> ftNessFirePool;
typedef wnArticleHierarchy<wnNessPKFlash, 1, ftNessFirePool> ftNessFlashPool;

class ftNessArticleMediator : public soArticleMediator {
    ftNessFlashPool m_pools;
    u8 m_autoRecycle;
public:
    ftNessArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftNessArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The lists of the module hold five entries although the mediator knows six articles. They live in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef wnArticleManageModuleBuilder<ftNessArticleMediator, 5> ftNessArticleManageModuleBuilder;
static_assert(sizeof(ftNessArticleManageModuleBuilder) == 0x42A0C, "Ness article manage builder layout is wrong!");
