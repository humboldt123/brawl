#pragma once

#include <mt/mt_vector.h>
#include <so/status/so_status_module_impl.h>

class ftStatusUniqProcessRebirth : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessRebirth() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void attachEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos);
    virtual void updateEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos);
};
