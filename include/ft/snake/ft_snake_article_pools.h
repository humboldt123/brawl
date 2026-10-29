#pragma once

// The articles Snake owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#include <wn/wn_simple.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
class BaseItem;
class soArticle;

// HYPOTHESIS: the helper of the generic article mediator that creates an item as an article of a slot.
class soArticleMediatorHelper {
public:
    static BaseItem* createItem(int itemKind, int articleId, int ownerTaskId);
    static soArticle* castArticle(BaseItem* item);
};
struct wnSnakeMineData;
struct wnSnakeTrenchMortarData;
struct wnSnakeRpg7Data;
struct wnSnakeCypherData;
struct wnSnakeC4Data;
struct wnSnakeC4SwitchData;
struct wnSnakeNikitaData;
struct wnSnakeLadderData;
struct wnSnakeRgb6Data;
struct wnSnakeRgb6SightData;
struct wnSnakeRgb6ClipData;
struct wnSnakeRgb6BulletData;

// Snake's land mine (wn_snake_mine). Opaque shell: the class is a weapon builder that cannot coexist with this unit.
class wnSnakeMine : public Weapon {
    u8 m_unreconstructed[0x2884 - sizeof(Weapon)];
public:
    wnSnakeMine(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeMineData* data);
    virtual ~wnSnakeMine();
    virtual void processCollision();
    // HYPOTHESIS: the soCollisionSearchEventObserver and soDamageEventObserver overrides
    // (the shell does not model the observer bases).
    bool notifyEventCollisionSearchCheck();
    void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
    void activate(int founderTaskId, int etcResourceId, int team, float powerMul);
    void shoot(const Vec3f& position);
};
static_assert(sizeof(wnSnakeMine) == 0x2884, "wnSnakeMine layout is wrong!");

// The grenade launcher shell (the class is in sora_melee). Opaque shell for the same reason.
class wnSnakeTrenchMortar : public Weapon {
    u8 m_unreconstructed[0x7CE8 - sizeof(Weapon)];
public:
    wnSnakeTrenchMortar(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeTrenchMortarData* data);
    virtual ~wnSnakeTrenchMortar();
    void activate(int etcResourceId, int team, float lr, int founderTaskId);
};
static_assert(sizeof(wnSnakeTrenchMortar) == 0x7CE8, "wnSnakeTrenchMortar layout is wrong!");

// The rocket launcher rocket (the class is in sora_melee). Opaque shell for the same reason.
class wnSnakeRpg7 : public Weapon {
    u8 m_unreconstructed[0x16B0 - sizeof(Weapon)];
public:
    wnSnakeRpg7(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRpg7Data* data);
    virtual ~wnSnakeRpg7();
    void activate(int founderTaskId, int etcResourceId);
};
static_assert(sizeof(wnSnakeRpg7) == 0x16B0, "wnSnakeRpg7 layout is wrong!");

// The Cypher (wn/snake/wn_snake_cypher.h). Opaque shell for the same reason.
class wnSnakeCypher : public Weapon {
    u8 m_unreconstructed[0x2220 - sizeof(Weapon)];
public:
    wnSnakeCypher(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeCypherData* data);
    virtual ~wnSnakeCypher();
    void activate(int etcResourceId, int team, int founderTaskId);
        void shoot(const float* speedX);
};
static_assert(sizeof(wnSnakeCypher) == 0x2220, "wnSnakeCypher layout is wrong!");

// The C4 charge (wn_snake_c4). Opaque shell for the same reason.
class wnSnakeC4 : public Weapon {
    u8 m_unreconstructed[0x2430 - sizeof(Weapon)];
public:
    wnSnakeC4(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4Data* data);
    virtual ~wnSnakeC4();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    void activate(int founderTaskId, int etcResourceId, int team, float lr);
    void shoot();
};
static_assert(sizeof(wnSnakeC4) == 0x2430, "wnSnakeC4 layout is wrong!");

// The C4 detonator (wn_snake_c4_switch). Opaque shell for the same reason.
class wnSnakeC4Switch : public Weapon {
    u8 m_unreconstructed[0x1364 - sizeof(Weapon)];
public:
    wnSnakeC4Switch(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4SwitchData* data);
    virtual ~wnSnakeC4Switch();
    void activate(int founderTaskId, int etcResourceId, int team);
};
static_assert(sizeof(wnSnakeC4Switch) == 0x1364, "wnSnakeC4Switch layout is wrong!");

// The Nikita missile (wn/snake/wn_snake_nikita.h). Opaque shell for the same reason.
class wnSnakeNikita : public Weapon {
    u8 m_unreconstructed[0x67C4 - sizeof(Weapon)];
public:
    wnSnakeNikita(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeNikitaData* data);
    virtual ~wnSnakeNikita();
    void activate(int founderTaskId, int etcResourceId, int team, float lr);
    void forceFallMissile();
};
static_assert(sizeof(wnSnakeNikita) == 0x67C4, "wnSnakeNikita layout is wrong!");

// The ladder (the class is in sora_melee). Opaque shell for the same reason.
class wnSnakeLadder : public Weapon {
    u8 m_unreconstructed[0x1F28 - sizeof(Weapon)];
public:
    wnSnakeLadder(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeLadderData* data);
    virtual ~wnSnakeLadder();
    void activate(int founderTaskId, int finalResourceId);
};
static_assert(sizeof(wnSnakeLadder) == 0x1F28, "wnSnakeLadder layout is wrong!");

// The grenade launcher (the class is in sora_melee). Opaque shell for the same reason.
class wnSnakeRgb6 : public Weapon {
    u8 m_unreconstructed[0x1F58 - sizeof(Weapon)];
public:
    wnSnakeRgb6(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6Data* data);
    virtual ~wnSnakeRgb6();
    void activate(int founderTaskId, int finalResourceId, int team);
};
static_assert(sizeof(wnSnakeRgb6) == 0x1F58, "wnSnakeRgb6 layout is wrong!");

// The grenade launcher sight (wn/snake/wn_snake_rgb6_sight.h). Opaque shell for the same reason.
class wnSnakeRgb6Sight : public Weapon {
    u8 m_unreconstructed[0x1FC4 - sizeof(Weapon)];
public:
    wnSnakeRgb6Sight(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6SightData* data);
    virtual ~wnSnakeRgb6Sight();
    void activate(int founderTaskId, int finalResourceId, int team);
};
static_assert(sizeof(wnSnakeRgb6Sight) == 0x1FC4, "wnSnakeRgb6Sight layout is wrong!");

// The grenade launcher clip (the class is in sora_melee). Opaque shell for the same reason.
class wnSnakeRgb6Clip : public Weapon {
    u8 m_unreconstructed[0x1F44 - sizeof(Weapon)];
public:
    wnSnakeRgb6Clip(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6ClipData* data);
    virtual ~wnSnakeRgb6Clip();
    void activate(int founderTaskId, int finalResourceId);
};
static_assert(sizeof(wnSnakeRgb6Clip) == 0x1F44, "wnSnakeRgb6Clip layout is wrong!");

// The grenade launcher bullet (the class is in sora_melee). Opaque shell for the same reason.
class wnSnakeRgb6Bullet : public Weapon {
    u8 m_unreconstructed[0x1FA4 - sizeof(Weapon)];
public:
    wnSnakeRgb6Bullet(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6BulletData* data);
    virtual ~wnSnakeRgb6Bullet();
};
static_assert(sizeof(wnSnakeRgb6Bullet) == 0x1FA4, "wnSnakeRgb6Bullet layout is wrong!");

typedef wnArticleHierarchy<wnSimple, 2, soInstancePoolRoot> ftSnakeSimplePool;
typedef wnArticleHierarchy<wnSnakeRgb6Bullet, 1, ftSnakeSimplePool> ftSnakeRgb6BulletPool;
typedef wnArticleHierarchy<wnSnakeRgb6Clip, 1, ftSnakeRgb6BulletPool> ftSnakeRgb6ClipPool;
typedef wnArticleHierarchy<wnSnakeRgb6Sight, 1, ftSnakeRgb6ClipPool> ftSnakeRgb6SightPool;
typedef wnArticleHierarchy<wnSnakeRgb6, 1, ftSnakeRgb6SightPool> ftSnakeRgb6Pool;
typedef wnArticleHierarchy<wnSnakeLadder, 1, ftSnakeRgb6Pool> ftSnakeLadderPool;
typedef wnArticleHierarchy<wnSnakeNikita, 1, ftSnakeLadderPool> ftSnakeNikitaPool;
typedef wnArticleHierarchy<wnSnakeC4Switch, 1, ftSnakeNikitaPool> ftSnakeC4SwitchPool;
typedef wnArticleHierarchy<wnSnakeC4, 1, ftSnakeC4SwitchPool> ftSnakeC4Pool;
typedef wnArticleHierarchy<wnSnakeCypher, 1, ftSnakeC4Pool> ftSnakeCypherPool;
typedef wnArticleHierarchy<wnSnakeRpg7, 1, ftSnakeCypherPool> ftSnakeRpg7Pool;
typedef wnArticleHierarchy<wnSnakeTrenchMortar, 1, ftSnakeRpg7Pool> ftSnakeTrenchMortarPool;
typedef wnArticleHierarchy<wnSnakeMine, 1, ftSnakeTrenchMortarPool> ftSnakeMinePool;

class ftSnakeArticleMediator : public soArticleMediator {
    ftSnakeMinePool m_pools;
    u8 m_autoRecycle;
public:
    ftSnakeArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftSnakeArticleMediator();
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
FT_DOL_ARRAY_VECTOR(soArticle*, 7);

typedef wnArticleManageModuleBuilder<ftSnakeArticleMediator, 7> ftSnakeArticleManageModuleBuilder;
static_assert(sizeof(ftSnakeArticleManageModuleBuilder) == 0x249E4, "Snake article manage builder layout is wrong!");
