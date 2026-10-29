#pragma once
#include <types.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/link/so_link_event_presenter.h>
#include <StaticAssert.h>
class soArticle;
class soModuleAccesser;
class soArticleMediator;
class soArticleEventObserver;
struct soLogAttackInfo;
template<class T> class soArray;
// Primary interface order verified against the concrete implementation vtable.
// HYPOTHESIS: unused selector source types are represented as int until their
// original enum declarations are recovered; argument counts and ABI are verified.
class soGenerateArticleManageModule {
public:
    virtual ~soGenerateArticleManageModule();
    virtual void activate();
    virtual void deactivate(bool treatAsRemoved); // HYPOTHESIS: the flag says the owner counts as removed (seen in StageObject::deactivate)
    // HYPOTHESIS: the second argument is a pointer (the log attack info, null by default) and the third a flag: with that
    // typing the callers get the native register copies (the flag reuses the article id zero, never the pointer zero).
    virtual soArticle* generate(int articleId, void* logAttackInfo, bool);
    virtual void have(int articleId, int, int selector, int taskId);
    virtual void shoot(int articleId, int selector, bool);
    virtual void changeMotion(int articleId, int motionKind, bool);
    virtual void setFrame(int articleId, float);
    virtual void setRate(int articleId, float);
    virtual void changeStatus(int articleId, int statusKind, int selector);
    virtual void setVisibilityWhole(int articleId, u8, int selector);
    virtual void set2nd(int articleId, u8, int selector);
    virtual void setSituationKind(int articleId, SituationKind);
    virtual void entry(soArticle*);
    virtual void eject(int taskId);
    virtual void remove(int articleId, int selector);
    virtual void removeExist(int articleId, int selector);
    virtual void removeExistTaskId(int taskId);
    virtual bool isExist(int articleId);
    virtual int getNum(int articleId);
    virtual bool isGeneratable(int articleId);
    virtual int getActiveNum(int articleId);
    virtual int getGenerateMaxNum(int articleId);
    virtual int getGenerateRestNum(int articleId);
    virtual soArticle* getArticle(int articleId);
    virtual soArticle* getArticleFromNo(int articleId, int number);
    virtual void getArticleList(soArray<soArticle*>*, int articleId, int selector);
    virtual void setDynamicArticleMediator(soArticleMediator*);
    virtual void setAutoRecycle(bool);
};
// The null module of sora_melee: an enemy without articles has it in its module accesser (Enemy::shoot / addParts test
// for it with a dynamic_cast).
class soGenerateArticleManageModuleNull : public soGenerateArticleManageModule {
public:
    virtual ~soGenerateArticleManageModuleNull();
};

static_assert(sizeof(soGenerateArticleManageModule) == 4, "Article interface layout is wrong!");
// Primary base verified by implementation RTTI at offset zero; the observer bases and fields follow the constructor.
class soGenerateArticleManageModuleImpl : public soGenerateArticleManageModule, public soStatusEventObserver,
                                          public soAnimCmdEventObserver, public soLinkEventObserver {
public:
    soModuleAccesser* m_moduleAccesser;
    soArray<soArticle*>* m_articles;
    soArticleMediator* m_mediator;
    soArray<soArticleEventObserver>* m_observers;
    soArticleMediator* m_dynamicMediator;

    soGenerateArticleManageModuleImpl(soModuleAccesser*, soArray<soArticle*>*,
        soArticleMediator*, soArray<soArticleEventObserver>*);
    virtual ~soGenerateArticleManageModuleImpl();
    virtual void activate();
    virtual void changeMotion(int articleId, int motionKind, bool);
    virtual void setFrame(int articleId, float);
    virtual void setRate(int articleId, float);
    virtual void setSituationKind(int articleId, SituationKind);
    virtual void removeExist(int articleId, int selector);
    virtual bool isExist(int articleId);
    virtual int getNum(int articleId);
    virtual bool isGeneratable(int articleId);
    virtual int getActiveNum(int articleId);
    virtual int getGenerateMaxNum(int articleId);
    virtual int getGenerateRestNum(int articleId);
    virtual soArticle* getArticle(int articleId);
    virtual soArticle* getArticleFromNo(int articleId, int number);
    virtual void setDynamicArticleMediator(soArticleMediator* mediator);
    virtual void setAutoRecycle(bool);
    virtual bool isObserv(char unk1);
#ifdef SO_ANIM_CMD_EVENT_OBSERVER_PURE
    // The observer's slots are pure in such a unit (so/anim/so_anim_cmd_event_presenter.h): the override.
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, soAnimCmdEventS32 unk3);
#endif
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4);
};
static_assert(sizeof(soGenerateArticleManageModuleImpl) == 0x3c, "Article module layout is wrong!");
