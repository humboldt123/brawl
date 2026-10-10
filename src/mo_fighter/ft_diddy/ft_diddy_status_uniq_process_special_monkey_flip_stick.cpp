#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDiddyStatusUniqProcessSpecialMonkeyFlipStick : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialMonkeyFlipStick() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

// The stick variant of the monkey flip has no per-frame behavior of its own;
// the flip itself is driven by ftDiddyStatusUniqProcessSpecialMonkeyFlip.
void ftDiddyStatusUniqProcessSpecialMonkeyFlipStick::initStatus(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialMonkeyFlipStick::execStatus(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialMonkeyFlipStick::execStop(soModuleAccesser* moduleAccesser) { }
void ftDiddyStatusUniqProcessSpecialMonkeyFlipStick::exitStatus(soModuleAccesser* moduleAccesser, int) { }

ftDiddyStatusUniqProcessSpecialMonkeyFlipStick g_ftDiddyStatusUniqProcessSpecialMonkeyFlipStick;
