#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

// The status process that every status of the End of Day article shares (its exit hands the photo-freeze back).
class wnPikminDolfinStatusUniqProcessCommon : public soStatusUniqProcess {
public:
    virtual ~wnPikminDolfinStatusUniqProcessCommon() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

extern wnPikminDolfinStatusUniqProcessCommon g_wnPikminDolfinStatusUniqProcessCommon;
