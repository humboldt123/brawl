#pragma once
// SHADOW of BrawlHeaders/so/collision/so_collision_search_event_presenter.h: addObserver is inline in the fighter RELs (FT_MODULE_BUILDER)

#include <StaticAssert.h>
#include <types.h>
#include <so/event/so_event_presenter.h>
#include <so/collision/so_collision_log.h>

class soModuleAccesser;

class soCollisionSearchEventObserver : public soEventObserver<soCollisionSearchEventObserver> {
public:
    soCollisionSearchEventObserver() : soEventObserver<soCollisionSearchEventObserver>(0x11) {};
    soCollisionSearchEventObserver(short unitID) : soEventObserver<soCollisionSearchEventObserver>(unitID) {};
#ifdef EM_ENEMY_BUILDER
    // The Enemy constructor expands it.
    soCollisionSearchEventObserver(short manageId, s8 p2) : soEventObserver<soCollisionSearchEventObserver>(0x11) { initialize(manageId, p2); }
#else
    soCollisionSearchEventObserver(short, s8);
#endif

#ifdef FT_MODULE_BUILDER
    virtual void addObserver(short param1, s8 param2) { addObserverSub(param1, this, param2); } // BYTES: the owning unit's weak copy; the #else units call the sora_melee instance
#else
    virtual void addObserver(short param1, s8 param2);
#endif
#ifdef SO_EVENT_OBSERVER_DEFAULTS_INLINE
    // Opt-in per unit (the Enemy): a null vtable slot for the handler, a weak copy of the default check (see
    // so_capture_event_presenter.h).
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) = 0;
    virtual bool notifyEventCollisionSearchCheck() { return false; }
#else
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
#endif
};
static_assert(sizeof(soCollisionSearchEventObserver) == 12, "Class is wrong size!");
