#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Treading on another fighter (Tread_Jump / Tread_Damage* / Tread_Fall): the fighter is linked to the one it stands on,
// which holds it through a model constraint until the process ends.
class ftStatusUniqProcessTread : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessTread() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);

    // Called by Fighter::notifyEventLink as well.
    void removeConstraint(soModuleAccesser* moduleAccesser);
};

extern ftStatusUniqProcessTread g_ftStatusUniqProcessTread;
