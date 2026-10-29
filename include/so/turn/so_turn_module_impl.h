#pragma once

// SHADOW of BrawlHeaders so/turn/so_turn_module_impl.h (which only includes the turn event presenter).
// soTurnModuleImpl (sora_melee), recovered from the RTTI and vtable of emTurnModuleImpl and the inline accessors the
// sora_enemy turn unit has weak copies of. Opt-in (SO_TURN_MODULE_IMPL_FULL, defined before any include): the units
// that only embed the module use the opaque 0x38-byte class of ft/builder/ft_dol_types.h instead.

#include <so/turn/so_turn_event_presenter.h>

#ifdef SO_TURN_MODULE_IMPL_FULL
#define SO_TURN_MODULE_IMPL_DEFINED

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/event/so_event_presenter.h>
#include <so/turn/so_turn_event_presenter.h>
#include <so/turn/so_turn_module.h>
#include <types.h>

class acAnimCmd;
class soModuleAccesser;

// From the RTTI: soTurnModule at 0, soAnimCmdEventObserver at 4, soEventPresenter<soTurnEventObserver> at 0x10.
class soTurnModuleImpl : public soTurnModule, public soAnimCmdEventObserver, public soEventPresenter<soTurnEventObserver> {
public:
    soModuleAccesser* m_moduleAccesser; // 0x1C
    const soTurnData* m_turnData;       // 0x20
    u32 unk24;                          // 0x24 (cleared by set)
    float m_lr;                         // 0x28 (getRYLr: 90 * lr + rotY)
    float m_rotY;                       // 0x2C
    float unk30;                        // 0x30 (cleared with m_rotY when the turn ends)
    // 0: not turning, 1: turning (rotY has not crossed 0 yet), 2: past the half turn. HYPOTHESIS: the meaning of 1 / 2.
    u8 m_state : 3;                     // 0x34
    // (u8 flags, not bool: isExtern converts the bit to bool.)
    u8 m_changeLr : 1;                  // the turn reverses the facing (HYPOTHESIS: set by set's changeLr)
    u8 m_lrReversed : 1;                // the facing was reversed during this turn
    u8 m_isExtern : 1;
    u8 m_omitIntermediate : 1;

    soTurnModuleImpl(soModuleAccesser* acc);
    virtual ~soTurnModuleImpl();
    virtual void activate();
    virtual void set(const soTurnData* data, bool changeLr, bool isExtern, float lr);
    virtual bool isTurn() { return m_state != 0; }
    virtual bool isExtern() { return m_isExtern; }
    virtual float getRY() { return m_rotY; }
    virtual float getRYLr() { return 90.0f * m_lr + m_rotY; }
    virtual unsigned int getEndFrame() { return m_turnData->endFrame; }
    virtual void setOmitIntermediate(bool omit) { m_omitIntermediate = omit; }
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
};
static_assert(sizeof(soTurnModuleImpl) == 0x38, "Class is wrong size!");
#endif
