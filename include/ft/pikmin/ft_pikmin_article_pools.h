#pragma once

// The articles Pikmin owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnPikminPikminData;
struct wnPikminDolfinData;
struct wnPikminChainData;

// Olimar's Pikmin (wn/pikmin/wn_pikmin_pikmin.h). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnPikminPikmin : public Weapon {
    u8 m_unreconstructed[0x3344 - sizeof(Weapon)];
public:
    wnPikminPikmin(s32 articleId, const wnArticleBuildInfo& info, const wnPikminPikminData* data);
    virtual ~wnPikminPikmin();
    void activate(float lr, float unk2, float unk3, int unk18, Vec3f* pos, int founderTaskId, int resourceId, int team,
                  int initStatus, bool unk10, int unk11);
    void onStatusFlag(u32 flag);
};
static_assert(sizeof(wnPikminPikmin) == 0x3344, "wnPikminPikmin layout is wrong!");

// The End of Day article (wn/pikmin/wn_pikmin_dolfin.h). Opaque shell for the same reason.
class wnPikminDolfin : public Weapon {
    u8 m_unreconstructed[0x2428 - sizeof(Weapon)];
public:
    wnPikminDolfin(s32 articleId, const wnArticleBuildInfo& info, const wnPikminDolfinData* data);
    virtual ~wnPikminDolfin();
    void activate(int founderTaskId, int kind, int team, Vec3f* pos);
};
static_assert(sizeof(wnPikminDolfin) == 0x2428, "wnPikminDolfin layout is wrong!");

// The Pikmin Chain (wn/pikmin/wn_pikmin_chain.h). Opaque shell for the same reason.
class wnPikminChain : public Weapon {
    u8 m_unreconstructed[0x1F40 - sizeof(Weapon)];
public:
    wnPikminChain(s32 articleId, const wnArticleBuildInfo& info, const wnPikminChainData* data);
    virtual ~wnPikminChain();
    void activate(float lr, int founderTaskId, int kind, int team, bool unk, int pikminNum);
};
static_assert(sizeof(wnPikminChain) == 0x1F40, "wnPikminChain layout is wrong!");

typedef wnArticleHierarchy<wnPikminChain, 1, soInstancePoolRoot> ftPikminChainPool;
typedef wnArticleHierarchy<wnPikminDolfin, 1, ftPikminChainPool> ftPikminDolfinPool;
typedef wnArticleHierarchy<wnPikminPikmin, 6, ftPikminDolfinPool> ftPikminPikminPool;

class ftPikminArticleMediator : public soArticleMediator {
    ftPikminPikminPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPikminArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPikminArticleMediator();
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
FT_DOL_ARRAY_VECTOR(soArticle*, 10);

typedef wnArticleManageModuleBuilder<ftPikminArticleMediator, 10> ftPikminArticleManageModuleBuilder;
static_assert(sizeof(ftPikminArticleManageModuleBuilder) == 0x17878, "Pikmin article manage builder layout is wrong!");
