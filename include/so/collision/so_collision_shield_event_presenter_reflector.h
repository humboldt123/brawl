#pragma once
// SHADOW of BrawlHeaders/so/collision/so_collision_shield_event_presenter_reflector.h: addObserver is inline in the fighter RELs (FT_MODULE_BUILDER)

#include <StaticAssert.h>
#include <types.h>
#include <so/event/so_event_presenter.h>
#include <so/collision/so_collision_log.h>

class soModuleAccesser;
class soCollisionAttackModule;
class soCollisionSearchModule;

class soCollisionReflectorEventObserver : public soEventObserver<soCollisionReflectorEventObserver> {
public:
    soCollisionReflectorEventObserver() : soEventObserver<soCollisionReflectorEventObserver>(0xf) {};
    soCollisionReflectorEventObserver(short unitID) : soEventObserver<soCollisionReflectorEventObserver>(unitID) {};
#ifdef EM_ENEMY_BUILDER
    // The Enemy constructor expands it.
    soCollisionReflectorEventObserver(short manageId, s8 p2) : soEventObserver<soCollisionReflectorEventObserver>(0xf) { initialize(manageId, p2); }
#else
    soCollisionReflectorEventObserver(short, s8);
#endif

#ifdef FT_MODULE_BUILDER
    virtual void addObserver(short param1, s8 param2) { addObserverSub(param1, this, param2); } // BYTES: the owning unit's weak copy; the #else units call the sora_melee instance
#else
    virtual void addObserver(short param1, s8 param2);
#endif
#ifdef SO_EVENT_OBSERVER_DEFAULTS_INLINE
    // Opt-in per unit (the Enemy): its observer vtable has null slots for the handlers and a weak copy of the default check,
    // so the original header declared the handlers pure and defined the check inline (see so_capture_event_presenter.h).
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser) = 0;
    virtual void notifyEventCollisionReflectorSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) = 0;
    virtual bool notifyEventCollisionReflectorCheck() { return false; }
#else
    virtual void notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionReflectorSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionReflectorCheck();
#endif
};
static_assert(sizeof(soCollisionReflectorEventObserver) == 12, "Class is wrong size!");

