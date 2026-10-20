#pragma once

#include <StaticAssert.h>
#include <so/event/so_event_presenter.h>
#include <so/so_null.h>
#include <so/so_module_accesser.h>
#include <types.h>

class soArticleEventObserver : public soEventObserver<soArticleEventObserver> {
public:
    virtual void addObserver(short param1, s8 param2);
    char _spacer1[2];
};
static_assert(sizeof(soArticleEventObserver) == 12, "Class is wrong size!");

// Local copy of BrawlHeaders' so/article/so_article.h that shadows it (-I include comes first): the original
// declares soLogAttackInfo as a bare aggregate, but the REL has an out-of-line default constructor for it
// (ftLogPatternModule builds arrays of these through __construct_array).
struct soLogAttackInfo {
    int _0;
    int _4;
    int _8;
    soLogAttackInfo(); // defined out of line in ft_log_pattern_module.cpp
    soLogAttackInfo(int a, int b) : _0(a), _4(b), _8(0) { } // value of the article work variables (Weapon::getLogAttackInfo)
    ~soLogAttackInfo() { } // non-trivial: makes array construction go through __construct_array
};

class soArticle : public soNullable, public soEventPresenter<soArticleEventObserver> {
public:
    // (manageId, unitId) constructor of the presenter used by the generic article base (Weapon).
    soArticle(s16 manageId, s16 unitId) : soNullable(false), soEventPresenter<soArticleEventObserver>(manageId, unitId) { }
    soArticle() { }
    bool setDeactivateDescendant();
    // Non-virtual helpers of the generic article code (sora_melee).
    void setNotifyArticleEventRemove(soModuleAccesser* moduleAccesser);
    void setNotifyArticleEventEject(int unk, soModuleAccesser* moduleAccesser);
    void updateLogAttackInfo(u32 teamOwnerId);
    // TODO
#ifdef SO_ARTICLE_DTOR_INLINE
    virtual ~soArticle() { } // MATCH-ONLY: inline in the generic article base (Weapon)
#else
    virtual ~soArticle();
#endif
    virtual void remove() = 0;
    virtual int getArticleId() = 0;
    virtual s16 getArticleEventManageId() = 0;
    virtual int getTaskId() = 0;
    virtual void changeMotion(int motionKind, bool);
    virtual void setFrame(float frame);
    virtual void setRate(float rate);
    virtual void changeStatus(int statusKind);
    virtual void setVisibilityWhole(u8);
    virtual void setVisibility(int, u8);
    virtual void set2nd(u8);
#ifdef WN_WEAPON_BUILDER // inline in the article RELs (weak copies)
    virtual void action(int) { }
#else
    virtual void action(int);
#endif
    virtual void setSituationKind(SituationKind);
    virtual bool isConstraint(bool);
    virtual void deactivateDescendantForce();
    virtual bool isActiveArticle() = 0;
    virtual void deactivateArticle() = 0;
    virtual int getOwnerDeactiveTreatType();
    virtual u32 getFounderTaskId();
    virtual void have(int, int, int) = 0;
    virtual void linkOwner(int, int) = 0;
    virtual void unlinkOwner(int) = 0;
    virtual bool isSyncOwnerStatus() = 0;
    virtual void setSyncOwnerStatus(bool) = 0;
    virtual void setLogAttackInfo(soLogAttackInfo* logAttackInfo);
    virtual soLogAttackInfo getLogAttackInfo();
    virtual void updateLogAttackInfo();
#ifdef WN_WEAPON_BUILDER // inline in the article RELs (weak copies)
    virtual void intrudeLogAttackInfo() { }
#else
    virtual void intrudeLogAttackInfo();
#endif
    virtual float getActiveGlobalFrame();
    virtual void setAttackPowerMulPattern(float);
};
static_assert(sizeof(soArticle) == 20, "Class is wrong size!");
