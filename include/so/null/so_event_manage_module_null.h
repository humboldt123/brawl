#pragma once

#include <so/event/so_event_manage_module_impl.h>

class soEventManageModuleNull : public soEventManageModule {
public:
    virtual s16 addUnit(soEventUnit*);
    virtual void eraseUnit(u32);
    virtual bool isNullUnit(u32);
    virtual void eraseObserver(u32, int);
    virtual u32 getObserverCapacity(u32 index);
    virtual u32 getObserverSize(u32 index);
    virtual bool isExist(short);
    virtual s16 getManageId();
    virtual soEventUnit* getEventUnit(short);
};
