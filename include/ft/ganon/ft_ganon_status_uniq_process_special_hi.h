#pragma once

#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>

// The native HiThrow vtable reuses these Cling callbacks as its base subobject.
class ftGanonStatusUniqProcessSpecialHiCling : public soStatusUniqProcess {
public:
    virtual ~ftGanonStatusUniqProcessSpecialHiCling() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* data);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};
