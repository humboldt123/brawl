#pragma once

// The articles Falco owns and the mediator that hands them out: the blaster, its bullets (five), the Phantasm afterimages
// (two) and the Landmaster (Final Smash). The pools are chained base first (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

#ifdef FT_FALCO_ARTICLE_POOLS
// These observer lists call their shared sora_melee destructors (Fox's form, ft/fox/ft_fox_article_pools.h): the
// articles' inside-event builder teardown calls them out of line.
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
// The Blaster's three model observers (the Blaster destructor calls this teardown out of line).
template <> class soEventUnitWithWorkArea<soModelEventObserver, 3> : public soEventUnitImpl<soModelEventObserver> {
    soInstanceManagerFullPropertyVector<soModelEventObserver*, 3> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soModelEventObserver>(managerId, eventId) { }
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
#endif

// Full article types let the fighter own their pools and teardown.
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_FOX_LAND_MASTER_FIGHTER_TU
#include <wn/wn_module_accesser_builder.h>
#ifdef FT_FALCO_ARTICLE_POOLS
// The afterimage's two-part attack builder and the Landmaster shell's one-part one: each has a single user in this unit,
// so its teardown is expanded inside that article's destructor (the original has no out-of-line copy of either), unlike
// the bullet's four-part one (WN_ARTICLE_BUILDER_USER_DTORS).
#define FT_FALCO_INLINE_ATTACK_BUILDER(PARTS)                                                                                  template <>                                                                                                                class soCollisionAttackModuleBuilder<                                                                                          soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, PARTS, 0, soCollisionAttackModuleImpl, 1, false, true> > {         soArrayVector<soCollisionAttackPart, PARTS> m_attackPartArrayVector;                                                      soArrayVector<soCollisionGroup, 1> m_collisionGroupArrayVector;                                                            soArrayNull<soCollisionAttackAbsolute> m_attackAbsoluteArray;                                                              soCollisionAttackModuleImpl m_attackModule;                                                                            public:                                                                                                                        soCollisionAttackModuleBuilder(soModuleAccesser* moduleAccesser, int taskId, u8 taskCategory,                                                             soEventObserverRegistrationDesc* registrationDesc) :                                            m_attackPartArrayVector(PARTS, soCollisionAttackPart(soCollision::Category_Fighter, false), 0),                            m_collisionGroupArrayVector(1, 0),                                                                                         m_attackAbsoluteArray(0, soCollisionAttackAbsolute()),                                                                     m_attackModule(moduleAccesser, taskId, (gfTask::Category)taskCategory, &m_attackPartArrayVector,                                          &m_collisionGroupArrayVector, &m_attackAbsoluteArray, registrationDesc, true) { }                       ~soCollisionAttackModuleBuilder() { }                                                                                      soCollisionAttackModule* getModule() { return &m_attackModule; }                                                       };
FT_FALCO_INLINE_ATTACK_BUILDER(2)
FT_FALCO_INLINE_ATTACK_BUILDER(1)
#undef FT_FALCO_INLINE_ATTACK_BUILDER
#endif
#include <wn/falco/wn_falco_illusion.h>
#include <wn/falco/wn_falco_blaster_bullet.h>
#include <wn/falco/wn_falco_blaster.h>
#include <wn/fox/wn_fox_land_master.h>
static_assert(sizeof(wnFalcoIllusion) == 0x1FA0, "Falco Illusion layout is wrong!");

typedef wnArticleHierarchy<wnFoxLandMaster, 1, soInstancePoolRoot> ftFalcoLandMasterPool;
typedef wnArticleHierarchy<wnFalcoIllusion, 2, ftFalcoLandMasterPool> ftFalcoIllusionPool;
typedef wnArticleHierarchy<wnFalcoBlasterBullet, 5, ftFalcoIllusionPool> ftFalcoBulletPool;
typedef wnArticleHierarchy<wnFalcoBlaster, 1, ftFalcoBulletPool> ftFalcoBlasterPool;

class ftFalcoArticleMediator : public soArticleMediator {
    ftFalcoBlasterPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftFalcoArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftFalcoArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftFalcoArticleMediator, 5> ftFalcoArticleManageModuleBuilder;
static_assert(sizeof(ftFalcoArticleManageModuleBuilder) == 0x15040, "Falco article manage builder layout is wrong!");
