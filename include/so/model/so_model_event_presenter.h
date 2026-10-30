// Local BrawlHeaders shadow: adds the (manageId, p2) constructor that soShadowModuleImpl's constructor inlines
// (unit id 9, then addObserver), like soStatusEventObserver's and soAnimCmdEventObserver's.
#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <so/event/so_event_presenter.h>

class soModuleAccesser;

class soModelEventObserver : public soEventObserver<soModelEventObserver> {
public:
    soModelEventObserver(short unitID) : soEventObserver<soModelEventObserver>(unitID) {};
    soModelEventObserver(short manageId, s8 p2) : soEventObserver<soModelEventObserver>(0x9) { initialize(manageId, p2); }

    virtual void addObserver(short param1, s8 param2);
    virtual void notifyEventConstructInstance(bool, soModuleAccesser* moduleAccesser);
    virtual void notifyEventDestructInstance(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soModelEventObserver) == 12, "Class is wrong size!");
