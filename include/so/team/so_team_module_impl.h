#pragma once

// Local header (shadows the BrawlHeaders copy, doldecomp/brawl #152): the team interface classes (soTeam, soTeamModule)
// are our own headers, so only the event observer/presenter and the concrete module are taken from upstream.

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/event/so_event_presenter.h>
#include <so/so_null.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <types.h>

class soTeamEventObserver : public soEventObserver<soTeamEventObserver> {
public:
    soTeamEventObserver(s16 unitID) : soEventObserver(unitID) {
        addObserver(-1,  -1);
    }
    virtual void addObserver(s16 manageId, s8 p2);
    virtual void notifyEventChangeTeam(s32 newTeam, s32 prevTeam, u32 taskId) { }
};
static_assert(sizeof(soTeamEventObserver) == 0xC, "Class is the wrong size!");

class soTeamEventPresenter : public soEventPresenter<soTeamEventObserver> {
public:
    bool m_enableOwnerEvent;

    soTeamEventPresenter(s16 manageId) : soEventPresenter(manageId) {
        m_enableOwnerEvent = true;
    }

    virtual ~soTeamEventPresenter() { }
};
static_assert(sizeof(soTeamEventPresenter) == 0x10, "Class is the wrong size!");

class soTeamModuleImpl : public soTeamModule, public soTeamEventObserver, public soAnimCmdEventObserver {
    soModuleAccesser* m_moduleAccsr;
    soTeam* m_team;
    soTeam* m_hitTeam;
    soTeam* m_indirectTeam;
    s32 m_teamOwnerId;
    soTeamEventPresenter m_eventPresenter;
public:
    soTeamModuleImpl(soTeam* team,
                     soTeam* hitTeam,
                     soTeam* indirectTeam,
                     soModuleAccesser* accsr,
                     soEventObserverRegistrationDesc* desc);
    virtual ~soTeamModuleImpl();
    virtual soTeam* getTeam();
    virtual void setTeam(s32 team, bool notify);
    virtual soTeam* getHitTeam();
    virtual soTeam* getIndirectTeam();
    virtual void setHitTeam(s32 team);
    virtual void setTeamOwnerId(s32 taskId);
    virtual s32 getTeamOwnerId();
    virtual void set2nd(s32 p1);
    virtual void setHit2nd(s32 p1);
    virtual void activate();
    virtual void deactivate();
    virtual void reset();
    virtual void setEnableOwnerEvent(bool enable);
    virtual void notifyEventChangeTeam(s32 newTeam, s32 prevTeam, u32 taskId);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual bool isObserv(char unk1);
};
static_assert(sizeof(soTeamModuleImpl) == 0x44, "Class is the wrong size!");
