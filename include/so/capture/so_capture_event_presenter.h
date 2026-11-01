#pragma once
// SHADOW of BrawlHeaders/so/capture/so_capture_event_presenter.h: addObserver is inline in the fighter RELs (FT_MODULE_BUILDER)

#include <StaticAssert.h>
#include <types.h>
#include <so/event/so_event_presenter.h>

class soModuleAccesser;

class soCaptureEventObserver : public soEventObserver<soCaptureEventObserver> {
public:
    soCaptureEventObserver(short unitID) : soEventObserver<soCaptureEventObserver>(unitID) {};
    soCaptureEventObserver(short manageId, s8 p2) : soEventObserver<soCaptureEventObserver>(0xd) { initialize(manageId, p2); }

#ifdef FT_MODULE_BUILDER
    virtual void addObserver(short param1, s8 param2) { addObserverSub(param1, this, param2); } // BYTES: the owning unit's weak copy; the #else units call the sora_melee instance
#else
    virtual void addObserver(short param1, s8 param2);
#endif
#ifdef SO_EVENT_OBSERVER_DEFAULTS_INLINE
    // Opt-in per unit (the Enemy): its observer vtables point at weak copies of the default handlers, so the original
    // header defined them inline.
    virtual bool notifyEventCaptureStatus(soModuleAccesser* moduleAccesser, int taskId, int, int) { return false; }
#else
    virtual bool notifyEventCaptureStatus(soModuleAccesser* moduleAccesser, int taskId, int, int);
#endif
};
static_assert(sizeof(soCaptureEventObserver) == 12, "Class is wrong size!");