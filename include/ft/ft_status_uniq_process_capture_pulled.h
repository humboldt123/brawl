#pragma once

#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

// Captured-by-grab (pulled) status process. The Capture* family derives from it: CaptureDamage only
// overrides initStatus/execStatus and tail-calls execStatusCommon with its own status ids.
class ftStatusUniqProcessCapturePulled : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCapturePulled() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
    // HYPOTHESIS: checks how far the captured fighter has been pulled; *outStatus receives the near/far status to use and the
    // result tells whether the pull reached the end. (Not virtual; the receiver is unused.)
    bool checkPullHeight(soModuleAccesser* moduleAccesser, int nearStatus, int farStatus, int* outStatus);
    void execStatusCommon(soModuleAccesser* moduleAccesser, int, int, u32);
};
