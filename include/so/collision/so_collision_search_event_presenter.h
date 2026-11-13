#pragma once
// SHADOW of BrawlHeaders/so/collision/so_collision_search_event_presenter.h: addObserver is inline in the fighter RELs (FT_MODULE_BUILDER)

#include <StaticAssert.h>
#include <types.h>
#include <so/event/so_event_presenter.h>
#include <so/collision/so_collision_log.h>

class soModuleAccesser;

#ifdef WN_EM_SIMPLE_UNIT
// BYTES: wnemSimple calls the sora_melee constructor and initialize of this base.
class soCollisionSearchEventObserver;
template <>
class soEventObserver<soCollisionSearchEventObserver> {
public:
    virtual void addObserver(s16 param1, s8 param2) { }
    s16 m_manageID;
    s16 m_unitID;
    s16 m_sendID;
    s32 getObserverId() const { return m_sendID; }
    soEventObserver(s16 unitID);
    ~soEventObserver();
    void addObserverSub(s32 manageId, soCollisionSearchEventObserver* obsvr, s8 p3);
    void initialize(s16 param1, s8 param2);
};
#endif

class soCollisionSearchEventObserver : public soEventObserver<soCollisionSearchEventObserver> {
public:
    soCollisionSearchEventObserver() : soEventObserver<soCollisionSearchEventObserver>(0x11) {};
    soCollisionSearchEventObserver(short unitID) : soEventObserver<soCollisionSearchEventObserver>(unitID) {};
#ifdef WN_EM_SIMPLE_UNIT
    ~soCollisionSearchEventObserver();
#endif
#if defined(EM_ENEMY_BUILDER) || defined(SO_COLLISION_SEARCH_OBSERVER_INLINE) || defined(SO_COLLISION_SEARCH_OBSERVER_INLINE_CTOR)
    // The Enemy constructor and Snake's C4 (opt-in SO_COLLISION_SEARCH_OBSERVER_INLINE) expand it; Snake's mine
    // (SO_COLLISION_SEARCH_OBSERVER_INLINE_CTOR) expands only this constructor, without the weak default check below.
    soCollisionSearchEventObserver(short manageId, s8 p2) : soEventObserver<soCollisionSearchEventObserver>(0x11) { initialize(manageId, p2); }
#else
    soCollisionSearchEventObserver(short, s8)
#ifdef WN_EM_SIMPLE_UNIT
        __attribute__((never_inline))
#endif
        ;
#endif

#ifdef FT_MODULE_BUILDER
    virtual void addObserver(short param1, s8 param2) { addObserverSub(param1, this, param2); } // BYTES: the owning unit's weak copy; the #else units call the sora_melee instance
#else
    virtual void addObserver(short param1, s8 param2);
#endif
#if defined(SO_EVENT_OBSERVER_DEFAULTS_INLINE) || defined(SO_COLLISION_SEARCH_OBSERVER_INLINE)
    // Opt-in per unit (the Enemy, and Snake's C4 for this presenter only): a null vtable slot for the handler, a weak copy of the default check (see
    // so_capture_event_presenter.h).
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) = 0;
    virtual bool notifyEventCollisionSearchCheck() { return false; }
#else
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
#endif
};
static_assert(sizeof(soCollisionSearchEventObserver) == 12, "Class is wrong size!");
