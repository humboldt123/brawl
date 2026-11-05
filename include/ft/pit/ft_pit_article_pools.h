#pragma once

// The articles Pit owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
#ifdef FT_PIT_ARTICLE_POOLS
// These observer lists call their shared sora_melee destructors (as in ft_snake_article_pools.h): the inside-event
// builder teardown calls them out of line.
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
// The article builders are complete here (their kinetic builder teardown expands the weapon kinetic module's), as in
// ft_mario_article_pools.h.
#define WN_ARTICLE_NATIVE_BUILDERS
#include <wn/wn_module_accesser_builder.h>
#endif

class Vec3f;
struct wnPitBowData;
struct wnPitBowArrowData;
struct wnPitParthenaData;
class wnPitIkarus;

#ifdef WN_PIT_BOW_FULL_BUILDER // only the fighter TU owns the bow classes (the Parthena unit keeps the shells)
#include <wn/pit/wn_pit_bow.h>
#else
// Pit's bow (the class is in sora_melee). Opaque shell: the full class is a weapon builder that cannot coexist with this unit.
class wnPitBow : public Weapon {
    u8 m_unreconstructed[0x1534 - sizeof(Weapon)];
public:
    wnPitBow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowData* data);
    virtual ~wnPitBow();
};
static_assert(sizeof(wnPitBow) == 0x1534, "wnPitBow layout is wrong!");
#endif

#ifdef WN_PIT_BOW_FULL_BUILDER
#include <wn/pit/wn_pit_bow_arrow.h>
#else
// The arrows of the bow (the class is in the REL, wn_pit_bow_arrow). Opaque shell for the same reason.
class wnPitBowArrow : public Weapon {
    u8 m_unreconstructed[0x1FD8 - sizeof(Weapon)];
public:
    wnPitBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowArrowData* data);
    virtual ~wnPitBowArrow();
};
static_assert(sizeof(wnPitBowArrow) == 0x1FD8, "wnPitBowArrow layout is wrong!");
#endif

#ifdef WN_PIT_IKARUS_FULL_BUILDER
#include <wn/pit/wn_pit_parthena.h>
#else
// The Final Smash article (Palutena's army). Opaque shell for the same reason.
class wnPitParthena : public Weapon {
    u8 m_unreconstructed[0x180A4 - sizeof(Weapon)];
public:
    wnPitParthena(s32 articleId, const wnArticleBuildInfo& info, const wnPitParthenaData* data);
    virtual ~wnPitParthena();
    void activate(int founderTaskId, int resourceId, int team, float scale);
    static bool activeArticle(wnPitIkarus* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(wnPitParthena) == 0x180A4, "wnPitParthena layout is wrong!");

#endif

typedef wnArticleHierarchy<wnPitParthena, 1, soInstancePoolRoot> ftPitParthenaPool;
typedef wnArticleHierarchy<wnPitBowArrow, 4, ftPitParthenaPool> ftPitBowArrowPool;
typedef wnArticleHierarchy<wnPitBow, 1, ftPitBowArrowPool> ftPitBowPool;

class ftPitArticleMediator : public soArticleMediator {
    ftPitBowPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftPitArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftPitArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftPitArticleMediator, 5> ftPitArticleManageModuleBuilder;
static_assert(sizeof(ftPitArticleManageModuleBuilder) == 0x2163C, "Pit article manage builder layout is wrong!");
#ifdef FT_PIT_ARTICLE_POOLS
#undef WN_ARTICLE_NATIVE_BUILDERS // the fighter's own builders keep their out-of-line teardown (ft/builder/ft_builder_noinline.h)
#endif
