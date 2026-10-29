// SHADOW of BrawlHeaders/so/situation/so_situation_module_impl.h: identical except for the FT_MODULE_BUILDER-guarded declarations
// (constructors/destructors of sora_melee classes that fighter module builders call), the public constructor and the
// m_lastKind/m_isFixed/m_isKeepAir fields that so_situation_module_impl.cpp uses.
#pragma once

#include <StaticAssert.h>
#include <so/event/so_event_presenter.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <types.h>

class soModuleAccesser;

class soSituationModule {
public:
    virtual ~soSituationModule();
    virtual void activate();
    virtual void deactivate();
    virtual SituationKind getKind();
    virtual SituationKind getPrevKind();
    virtual bool isSituationChanged();
    virtual void setKind(SituationKind kind, bool);
    virtual void setKeepAir(bool);
    virtual void update();
};
static_assert(sizeof(soSituationModule) == 4, "Class is wrong size!");

class soSituationModuleImpl : public soSituationModule, public soStatusEventObserver, public soAnimCmdEventObserver, public soEventPresenter<soSituationEventObserver> {
public:
    soSituationModuleImpl(s16 unitId, soModuleAccesser* acc, soEventObserverRegistrationDesc* regDesc);
#ifdef FT_MODULE_BUILDER
private:
#endif
    soModuleAccesser* m_moduleAccesser;
    int m_kind;
    int m_prevKind;
    // HYPOTHESIS: the kind as of the last setKind; getPrevKind/isSituationChanged compare it with m_prevKind.
    int m_lastKind;
    // HYPOTHESIS: set by setKind (its bool, or the Restraint/Outfield kinds); update leaves the kind alone while it is set.
    u8 m_isFixed : 1;
    u8 m_isKeepAir : 1;
    char _57[3];

public:
    virtual ~soSituationModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual SituationKind getKind();
    virtual SituationKind getPrevKind();
    virtual bool isSituationChanged();
    virtual void setKind(SituationKind kind, bool);
    virtual void setKeepAir(bool);
    virtual void update();

    virtual bool isObserv(char unk1);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, soAnimCmdEventS32 unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soSituationModuleImpl) == 60, "Class is wrong size!");