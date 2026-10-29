#pragma once

// The turn module of the enemies (mo_enemy/sora_enemy/turn/em_turn_module_impl.cpp): soTurnModuleImpl driven by the
// enemy's anim cmds (group 'l'), turning with one of the enemy's kinetic energies and resetting a model node's rotation
// when the turn ends.
//
// soTurnModuleImpl is a class of sora_melee. The other enemy units only need its size, which ft/builder/ft_dol_types.h
// gives (an opaque 0x38-byte class); the turn unit defines EM_TURN_MODULE_IMPL_FULL to take the full class of
// so/turn/so_turn_module_impl.h (it must then include this header before em/enemy.h).

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/turn/so_turn_module_impl.h>
#include <types.h>

class acAnimCmd;
class emKineticModuleImpl;
class soModuleAccesser;
class soPostureModule;

#ifdef EM_TURN_MODULE_IMPL_FULL
#ifndef SO_TURN_MODULE_IMPL_DEFINED
#error "em/turn/em_turn_module_impl.h: define SO_TURN_MODULE_IMPL_FULL before any include"
#endif
#else
#include <ft/builder/ft_dol_types.h> // soTurnModuleImpl (opaque)
#endif

class emTurnModuleImpl : public soTurnModuleImpl, public soAnimCmdEventObserver {
public:
    int m_energyId;    // 0x44 the kinetic energy that turns the enemy (-1: none)
    u32 m_rotNodeId;   // 0x48 the node whose rotation Enemy::initRot resets when the turn ends

    emTurnModuleImpl(soModuleAccesser* acc);
    virtual ~emTurnModuleImpl();
#ifdef EM_TURN_MODULE_IMPL_FULL
    virtual void activate();
    virtual void end();
    virtual void update(bool active);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, soAnimCmdEventS32 unk3);

    // Starts a turn driven by kinetic energy energyId (anim cmd 'l' 0 / 1).
    void start(int energyId, u32 rotNodeId, bool changeLr, float lr);
    // Reverses the facing and every valid kinetic energy except the first one.
    void updateSubReverseLr(emKineticModuleImpl* kinetic, soPostureModule* posture);
#elif defined(SO_ANIM_CMD_EVENT_OBSERVER_PURE)
    // The observer's slots are pure in such a unit (so/anim/so_anim_cmd_event_presenter.h): the overrides.
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, soAnimCmdEventS32 unk3);
#endif
};
static_assert(sizeof(emTurnModuleImpl) == 0x4C, "Class is wrong size!");
