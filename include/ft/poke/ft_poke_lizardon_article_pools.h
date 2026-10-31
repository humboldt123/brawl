#pragma once

// The articles PokeLizardon owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
class wnPokeLizardonRockStone;
struct wnPokeLizardonBreathData;
struct wnPokeLizardonRockData;
struct wnPokeLizardonRockStoneData;

// The flame breath of Lizardon (the class is in sora_melee). Opaque shell: the class is a weapon builder that cannot coexist with this unit.
class wnPokeLizardonBreath : public Weapon {
    u8 m_unreconstructed[0x164C - sizeof(Weapon)];
public:
    wnPokeLizardonBreath(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonBreathData* data);
    virtual ~wnPokeLizardonBreath();
};
static_assert(sizeof(wnPokeLizardonBreath) == 0x164C, "wnPokeLizardonBreath layout is wrong!");

// The rock of the down special (wn_poke_lizardon_rock). Opaque shell for the same reason.
class wnPokeLizardonRock : public Weapon {
    u8 m_unreconstructed[0x18EC - sizeof(Weapon)];
public:
    wnPokeLizardonRock(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonRockData* data);
    virtual ~wnPokeLizardonRock();
    void shoot(bool unk);
    static bool activeArticle(wnPokeLizardonRockStone* stone, soModuleAccesser* acc);
};
static_assert(sizeof(wnPokeLizardonRock) == 0x18EC, "wnPokeLizardonRock layout is wrong!");

// The stones the rock breaks into (wn_poke_lizardon_rock_stone). Opaque shell for the same reason.
class wnPokeLizardonRockStone : public Weapon {
    u8 m_unreconstructed[0x16DC - sizeof(Weapon)];
public:
    wnPokeLizardonRockStone(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonRockStoneData* data);
    virtual ~wnPokeLizardonRockStone();
};
static_assert(sizeof(wnPokeLizardonRockStone) == 0x16DC, "wnPokeLizardonRockStone layout is wrong!");

typedef wnArticleHierarchy<wnPokeLizardonRockStone, 4, soInstancePoolRoot> ftPokeLizardonRockStonePool;
typedef wnArticleHierarchy<wnPokeLizardonRock, 2, ftPokeLizardonRockStonePool> ftPokeLizardonRockPool;
typedef wnArticleHierarchy<wnPokeLizardonBreath, 5, ftPokeLizardonRockPool> ftPokeLizardonBreathPool;

class ftPokeLizardonArticleMediator : public soArticleMediator {
    ftPokeLizardonBreathPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPokeLizardonArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPokeLizardonArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftPokeLizardonArticleMediator, 5> ftPokeLizardonArticleManageModuleBuilder;
static_assert(sizeof(ftPokeLizardonArticleManageModuleBuilder) == 0xFDF0, "PokeLizardon article manage builder layout is wrong!");
