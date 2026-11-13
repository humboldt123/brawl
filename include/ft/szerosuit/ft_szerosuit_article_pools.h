#pragma once

// The articles Zero Suit Samus owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

// Full article types let the fighter own their pools and teardown.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_WEAPON_NO_COMMON_ENERGIES
#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
// These observer lists call their shared sora_melee destructors (as in ft_pit_article_pools.h).
template <> class soEventUnitWithWorkArea<soStatusEventObserver, 16> : public soEventUnitImpl<soStatusEventObserver> {
    soInstanceManagerFullPropertyVector<soStatusEventObserver*, 16> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soStatusEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soAnimCmdEventObserver, 32> : public soEventUnitImpl<soAnimCmdEventObserver> {
    soInstanceManagerFullPropertyVector<soAnimCmdEventObserver*, 32> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soAnimCmdEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soLinkEventObserver, 2> : public soEventUnitImpl<soLinkEventObserver> {
    soInstanceManagerFullPropertyVector<soLinkEventObserver*, 2> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soLinkEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soArticleEventObserver, 2> : public soEventUnitImpl<soArticleEventObserver> {
    soInstanceManagerFullPropertyVector<soArticleEventObserver*, 2> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soArticleEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_szerosuit_paralyzer.h>
#include <wn/zelda/wn_szerosuit_paralyzer_bullet.h>
#include <wn/samus/wn_s_zerosuit_whip.h>

class Vec3f;
struct wnSZerosuitArmorData;

static_assert(sizeof(wnSZerosuitWhip) == 0x1D50, "wnSZerosuitWhip layout is wrong!");

// The armor piece (separate unit of this REL). Opaque shell until the article is reconstructed.
class wnSZerosuitArmor : public Weapon {
    u8 m_unreconstructed[0x15D4 - sizeof(Weapon)];
public:
    wnSZerosuitArmor(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitArmorData* data);
    virtual ~wnSZerosuitArmor();
    void activate(int founderTaskId, u32 etcResId, int index, int mdlResIndex, u32 param0, u32 param1, int team);
#ifdef FT_SZEROSUIT_ARMOR_SHOOT_FLOAT_FIRST
    void shoot(float speed, float rate, float lr, int param);
#else
    void shoot(int param, float speed, float lr, float rate);
#endif
};
static_assert(sizeof(wnSZerosuitArmor) == 0x15D4, "wnSZerosuitArmor layout is wrong!");

typedef wnArticleHierarchy<wnSZerosuitArmor, 8, soInstancePoolRoot> ftSZerosuitArmorPool;
typedef wnArticleHierarchy<wnSZerosuitWhip, 1, ftSZerosuitArmorPool> ftSZerosuitWhipPool;
typedef wnArticleHierarchy<wnSZerosuitParalyzerBullet, 2, ftSZerosuitWhipPool> ftSZerosuitBulletPool;
typedef wnArticleHierarchy<wnSZerosuitParalyzer, 1, ftSZerosuitBulletPool> ftSZerosuitParalyzerPool;

class ftSZerosuitArticleMediator : public soArticleMediator {
    ftSZerosuitParalyzerPool m_pools;
    volatile u8 m_autoRecycle; // MATCH-ONLY: keeps the store behind the (partly inlined) pool constructors
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftSZerosuitArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftSZerosuitArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftSZerosuitArticleMediator, 10> ftSZerosuitArticleManageModuleBuilder;
