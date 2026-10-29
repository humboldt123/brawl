#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <so/event/so_event_presenter.h>
#include <ac/ac_anim_cmd_impl.h>

class soModuleAccesser;

#ifdef FT_MODULE_BUILDER
// MATCH-ONLY: the destructor of this base is an out-of-line function in sora_melee (fighter RELs call it), so the
// class is specialized with the destructor declared only.
class soAnimCmdEventObserver;
template <>
class soEventObserver<soAnimCmdEventObserver> {
public:
    virtual void addObserver(s16 param1, s8 param2) { }
    s16 m_manageID;
    s16 m_unitID;
    s16 m_sendID;
    s32 getObserverId() const { return m_sendID; }
    soEventObserver(s16 unitID) {
        m_manageID = -1;
        m_unitID = unitID;
        m_sendID = -1;
    }
    ~soEventObserver();
    void addObserverSub(s32 manageId, soAnimCmdEventObserver* obsvr, s8 p3); // sora_melee
    void initialize(s16 param1, s8 param2) { addObserver(param1, param2); }
};
#endif

// SO_ANIM_CMD_EVENT_OBSERVER_PURE (the enemy damage unit): isObserv and notifyEventAnimCmd are pure and addObserver is
// inline, as in the original, whose weak copy of this vtable holds 0 in those two slots. Every observer class whose
// definition such a unit sees must then override both with the observer's own third argument. The original has one
// type for it everywhere; our headers still say int (WN_ARTICLE_ANIMCMD_INT units) or s32 (int and long give the same
// code, only the mangled names differ). The s32 declarations of the classes em/enemy.h embeds use soAnimCmdEventS32,
// which is s32 except in a pure-observer unit, so no other unit's names change.
#ifdef SO_ANIM_CMD_EVENT_OBSERVER_PURE
typedef int soAnimCmdEventS32;
#else
typedef s32 soAnimCmdEventS32;
#endif

class soAnimCmdEventObserver : public soEventObserver<soAnimCmdEventObserver> {
public:
    soAnimCmdEventObserver(short unitID) : soEventObserver<soAnimCmdEventObserver>(unitID) {};
    soAnimCmdEventObserver();
    // NOTE: shadows the BrawlHeaders copy; (manageId, p2) constructor added by agent/damage
    soAnimCmdEventObserver(short manageId, s8 p2) : soEventObserver<soAnimCmdEventObserver>(0x5) { initialize(manageId, p2); }
    // HYPOTHESIS: constructor that also registers with the given manager (seen in soControllerModuleImpl ctor).
    soAnimCmdEventObserver(s16 unitID, s16 manageID) : soEventObserver<soAnimCmdEventObserver>(unitID) { addObserver(manageID, -1); }

#if defined(FT_MODULE_BUILDER) || defined(YK_STAGE_FULL) || defined(SO_EVENT_OBSERVER_INLINE) || defined(SO_ANIM_CMD_EVENT_OBSERVER_PURE)
    virtual void addObserver(short param1, s8 param2) { addObserverSub(param1, this, param2); } // MATCH-ONLY: inline in the REL
#else
    virtual void addObserver(short param1, s8 param2);
#endif
#ifdef SO_ANIM_CMD_EVENT_OBSERVER_PURE
    virtual bool isObserv(char unk1) = 0;
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) = 0;
#else
    virtual bool isObserv(char unk1);
#endif
#if defined(SO_ANIM_CMD_EVENT_OBSERVER_PURE)
#elif defined(WN_ARTICLE_ANIMCMD_INT)
    // The article tables share this callback with Weapon's int-index method.
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
#else
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
#endif
};
static_assert(sizeof(soAnimCmdEventObserver) == 12, "Class is wrong size!");
