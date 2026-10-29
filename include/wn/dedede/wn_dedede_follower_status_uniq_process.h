#pragma once

#include <so/status/so_status_module_impl.h>

class soModuleAccesser;

class wnDededeFollowerStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnDededeFollowerStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    void changeStatus(soModuleAccesser* moduleAccesser);
    void changeStatusWaddledee(soModuleAccesser* moduleAccesser);
    void changeStatusWaddledoo(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: the count is a plain int (it is stored with setInt).
    void setChangeStatusCount(soModuleAccesser* moduleAccesser, int count);
};
extern wnDededeFollowerStatusUniqProcess g_wnDededeFollowerStatusUniqProcess;

class wnDededeFollowerStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~wnDededeFollowerStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    void updateJumpCheckCount(soModuleAccesser* moduleAccesser);
    void checkJump(soModuleAccesser* moduleAccesser);
};
extern wnDededeFollowerStatusUniqProcessFinal g_wnDededeFollowerStatusUniqProcessFinal;
