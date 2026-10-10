#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

class ftPitStatusUniqProcessSpecialHiFly : public soStatusUniqProcess {
public:
    virtual ~ftPitStatusUniqProcessSpecialHiFly() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftPitStatusUniqProcessSpecialHiFly::exitStatus(soModuleAccesser* moduleAccesser, int) {
    float speed = moduleAccesser->getWorkManageModule().getFloat(0x11000013);
    float scaled = speed * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb6, 0);
    moduleAccesser->getWorkManageModule().setFloat(scaled, 0x11000013);
}

ftPitStatusUniqProcessSpecialHiFly g_ftPitStatusUniqProcessSpecialHiFly;
