#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// Moving the fighter onto the ledge hand-hold (Cliff_Catch_Move): the fighter is snapped to the ledge and its rotation
// follows the line to the cliff position while the motion plays.
class ftStatusUniqProcessCliffCatchMove : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCliffCatchMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

extern ftStatusUniqProcessCliffCatchMove g_ftStatusUniqProcessCliffCatchMove;
