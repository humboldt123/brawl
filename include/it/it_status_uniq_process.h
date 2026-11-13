#pragma once

#include <so/status/so_status_module_impl.h>

class itStatusUniqProcessStandby : public soStatusUniqProcess {
public:
    virtual ~itStatusUniqProcessStandby();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int statusKind);
    static itStatusUniqProcessStandby* getInstance();
};

class itStatusUniqProcessHave : public soStatusUniqProcess {
public:
    virtual ~itStatusUniqProcessHave();
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int statusKind);
    static itStatusUniqProcessHave* getInstance();
};
