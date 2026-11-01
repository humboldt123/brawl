#pragma once
// SHADOW of BrawlHeaders/so/collision/so_collision_shield_event_presenter_absorber.h: addObserver is inline in the fighter RELs (FT_MODULE_BUILDER)

#include <StaticAssert.h>
#include <types.h>
#include <so/event/so_event_presenter.h>
#include <so/collision/so_collision_log.h>

class soModuleAccesser;
class soCollisionAttackModule;
class soCollisionSearchModule;

class soCollisionAbsorberEventObserver : public soEventObserver<soCollisionAbsorberEventObserver> {
public:
    soCollisionAbsorberEventObserver(short unitID) : soEventObserver<soCollisionAbsorberEventObserver>(unitID) {};
    soCollisionAbsorberEventObserver(short manageId, s8 p2) : soEventObserver<soCollisionAbsorberEventObserver>(0x10) {
        initialize(manageId, p2);
    }

#ifdef FT_MODULE_BUILDER
    virtual void addObserver(short param1, s8 param2) { addObserverSub(param1, this, param2); } // BYTES: the owning unit's weak copy; the #else units call the sora_melee instance
#else
    virtual void addObserver(short param1, s8 param2);
#endif
#ifdef SO_EVENT_OBSERVER_DEFAULTS_INLINE
    // Opt-in per unit (the Enemy): a null vtable slot for the handler, a weak copy of the default check (see
    // so_capture_event_presenter.h).
    virtual void notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser) = 0;
    virtual bool notifyEventCollisionAbsorberCheck() { return false; }
#else
    virtual void notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAbsorberCheck();
#endif
};
static_assert(sizeof(soCollisionAbsorberEventObserver) == 12, "Class is wrong size!");
