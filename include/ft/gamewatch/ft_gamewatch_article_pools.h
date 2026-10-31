#pragma once

// The articles GameWatch owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnGameWatchSausageData;
struct wnGameWatchRescueData;
struct wnGameWatchParachuteData;
struct wnGameWatchPanelData;
struct wnGameWatchOilData;
struct wnGameWatchOctopusData;
struct wnGameWatchNormalWeaponData;

// The sausages of the down special (wn/gamewatch/wn_game_watch_sausage.h in sora_melee). Opaque shell: the class is a weapon builder that cannot coexist with this unit.
class wnGameWatchSausage : public Weapon {
    u8 m_unreconstructed[0x21A8 - sizeof(Weapon)];
public:
    wnGameWatchSausage(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchSausageData* data);
    virtual ~wnGameWatchSausage();
};
static_assert(sizeof(wnGameWatchSausage) == 0x21A8, "wnGameWatchSausage layout is wrong!");

// The rescue article (wn/gamewatch/wn_game_watch_rescue.h). Opaque shell for the same reason.
class wnGameWatchRescue : public Weapon {
    u8 m_unreconstructed[0x20E0 - sizeof(Weapon)];
public:
    wnGameWatchRescue(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchRescueData* data);
    virtual ~wnGameWatchRescue();
    void activate(int founderTaskId, int resourceId, int team, const Vec3f& position, float color);
};
static_assert(sizeof(wnGameWatchRescue) == 0x20E0, "wnGameWatchRescue layout is wrong!");

// The parachute (wn/gamewatch/wn_game_watch_parachute.h). Opaque shell for the same reason.
class wnGameWatchParachute : public Weapon {
    u8 m_unreconstructed[0x20E0 - sizeof(Weapon)];
public:
    wnGameWatchParachute(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchParachuteData* data);
    virtual ~wnGameWatchParachute();
    void activate(int founderTaskId, int resourceId, int team, float frame);
};
static_assert(sizeof(wnGameWatchParachute) == 0x20E0, "wnGameWatchParachute layout is wrong!");

// The panel (wn/gamewatch/wn_game_watch_panel.h). Opaque shell for the same reason.
class wnGameWatchPanel : public Weapon {
    u8 m_unreconstructed[0x20FC - sizeof(Weapon)];
public:
    wnGameWatchPanel(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchPanelData* data);
    virtual ~wnGameWatchPanel();
    void activate(int founderTaskId, int resourceId, int team, float lr, int selector, float frame);
};
static_assert(sizeof(wnGameWatchPanel) == 0x20FC, "wnGameWatchPanel layout is wrong!");

// The oil (wn/gamewatch/wn_game_watch_oil.h). Opaque shell for the same reason.
class wnGameWatchOil : public Weapon {
    u8 m_unreconstructed[0x20E0 - sizeof(Weapon)];
public:
    wnGameWatchOil(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchOilData* data);
    virtual ~wnGameWatchOil();
    void activate(int founderTaskId, int resourceId, int team, float color);
};
static_assert(sizeof(wnGameWatchOil) == 0x20E0, "wnGameWatchOil layout is wrong!");

// The octopus (wn/gamewatch/wn_game_watch_octopus.h). Opaque shell for the same reason.
class wnGameWatchOctopus : public Weapon {
    u8 m_unreconstructed[0x291C - sizeof(Weapon)];
public:
    wnGameWatchOctopus(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchOctopusData* data);
    virtual ~wnGameWatchOctopus();
    void activate(int founderTaskId, int resourceId, int team, const Vec3f& position, float lr, float unkFloatFromDB2, float fighterColor);
};
static_assert(sizeof(wnGameWatchOctopus) == 0x291C, "wnGameWatchOctopus layout is wrong!");

// The weapon of the side special (wn/gamewatch/wn_game_watch_normal_weapon.h). Opaque shell for the same reason.
class wnGameWatchNormalWeapon : public Weapon {
    u8 m_unreconstructed[0x20E0 - sizeof(Weapon)];
public:
    wnGameWatchNormalWeapon(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchNormalWeaponData* data);
    virtual ~wnGameWatchNormalWeapon();
    void activate(int founderTaskId, int resourceId, int team, int selector, float frame);
};
static_assert(sizeof(wnGameWatchNormalWeapon) == 0x20E0, "wnGameWatchNormalWeapon layout is wrong!");

typedef wnArticleHierarchy<wnGameWatchNormalWeapon, 1, soInstancePoolRoot> ftGameWatchNormalWeaponPool;
typedef wnArticleHierarchy<wnGameWatchOctopus, 1, ftGameWatchNormalWeaponPool> ftGameWatchOctopusPool;
typedef wnArticleHierarchy<wnGameWatchOil, 1, ftGameWatchOctopusPool> ftGameWatchOilPool;
typedef wnArticleHierarchy<wnGameWatchPanel, 1, ftGameWatchOilPool> ftGameWatchPanelPool;
typedef wnArticleHierarchy<wnGameWatchParachute, 1, ftGameWatchPanelPool> ftGameWatchParachutePool;
typedef wnArticleHierarchy<wnGameWatchRescue, 1, ftGameWatchParachutePool> ftGameWatchRescuePool;
typedef wnArticleHierarchy<wnGameWatchSausage, 5, ftGameWatchRescuePool> ftGameWatchSausagePool;

class ftGameWatchArticleMediator : public soArticleMediator {
    ftGameWatchSausagePool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftGameWatchArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftGameWatchArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftGameWatchArticleMediator, 5> ftGameWatchArticleManageModuleBuilder;
static_assert(sizeof(ftGameWatchArticleManageModuleBuilder) == 0x1771C, "GameWatch article manage builder layout is wrong!");
