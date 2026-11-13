#pragma once

// The articles Fox owns and the mediator that hands them out: the blaster, its bullets (five), the Phantasm afterimages
// (two) and the Landmaster (Final Smash). The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

#ifdef FT_FOX_ARTICLE_POOLS
// These observer lists call their shared sora_melee destructors (as in ft_pit_article_pools.h): the articles'
// inside-event builder teardown calls them out of line.
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
// The Landmaster's larger lists (its weak destructor calls these teardowns out of line too).
template <> class soEventUnitWithWorkArea<soStatusEventObserver, 17> : public soEventUnitImpl<soStatusEventObserver> {
    soInstanceManagerFullPropertyVector<soStatusEventObserver*, 17> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soStatusEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soAnimCmdEventObserver, 33> : public soEventUnitImpl<soAnimCmdEventObserver> {
    soInstanceManagerFullPropertyVector<soAnimCmdEventObserver*, 33> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soAnimCmdEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soModelEventObserver, 5> : public soEventUnitImpl<soModelEventObserver> {
    soInstanceManagerFullPropertyVector<soModelEventObserver*, 5> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soModelEventObserver>(managerId, eventId) { }
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
// The Blaster's three model observers (the Blaster destructor calls this teardown out of line).
template <> class soEventUnitWithWorkArea<soModelEventObserver, 3> : public soEventUnitImpl<soModelEventObserver> {
    soInstanceManagerFullPropertyVector<soModelEventObserver*, 3> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soModelEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
#endif

// Full article types let the fighter own their pools and teardown.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_FOX_LAND_MASTER_FIGHTER_TU
#include <wn/wn_module_accesser_builder.h>
#include <wn/fox/wn_fox_illusion.h>
#include <wn/fox/wn_fox_blaster_bullet.h>
#include <wn/fox/wn_fox_blaster.h>
#include <wn/fox/wn_fox_land_master.h>

typedef wnArticleHierarchy<wnFoxLandMaster, 1, soInstancePoolRoot> ftFoxLandMasterPool;
typedef wnArticleHierarchy<wnFoxIllusion, 2, ftFoxLandMasterPool> ftFoxIllusionPool;
typedef wnArticleHierarchy<wnFoxBlasterBullet, 5, ftFoxIllusionPool> ftFoxBulletPool;
typedef wnArticleHierarchy<wnFoxBlaster, 1, ftFoxBulletPool> ftFoxBlasterPool;

class ftFoxArticleMediator : public soArticleMediator {
    ftFoxBlasterPool m_pools;
    u8 m_autoRecycle;
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftFoxArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftFoxArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftFoxArticleMediator, 5> ftFoxArticleManageModuleBuilder;
static_assert(sizeof(ftFoxArticleManageModuleBuilder) == 0x14CD0, "Fox article manage builder layout is wrong!");
