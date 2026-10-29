#pragma once
// SHADOW of BrawlHeaders/so/turn/so_turn_event_presenter.h: addObserver is inline in the fighter RELs (FT_MODULE_BUILDER)

#include <StaticAssert.h>
#include <types.h>
#include <so/event/so_event_presenter.h>

class soModuleAccesser;

class soTurnEventObserver : public soEventObserver<soTurnEventObserver> {
public:
    soTurnEventObserver(short unitID) : soEventObserver<soTurnEventObserver>(unitID) {};
    // HYPOTHESIS: registering constructor like soStatusEventObserver's (seen in ftControllerModuleImpl's ctor).
    soTurnEventObserver(short manageId, s8 p2) : soEventObserver<soTurnEventObserver>(0x12) { initialize(manageId, p2); }

#ifdef FT_MODULE_BUILDER
    virtual void addObserver(short param1, s8 param2) { addObserverSub(param1, this, param2); } // MATCH-ONLY: inline in the REL
#else
    virtual void addObserver(short param1, s8 param2);
#endif
    virtual void notifyEventTurn(float, float, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soTurnEventObserver) == 12, "Class is wrong size!");