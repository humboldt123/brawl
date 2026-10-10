#include <ft/ft_entry.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftDededeStatusUniqProcessSpecialSuperJumpTurn : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJumpTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftDededeStatusUniqProcessSpecialSuperJumpTurn::initStatus(soModuleAccesser*) { }

void ftDededeStatusUniqProcessSpecialSuperJumpTurn::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000001);
        soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
        int limit = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0);
        if (workManage.getInt(0x20000001) > limit) {
            moduleAccesser->getWorkManageModule().offFlag(0x22000011);
            moduleAccesser->getWorkManageModule().onFlag(0x22000015);
        }
    }
}

ftDededeStatusUniqProcessSpecialSuperJumpTurn g_ftDededeStatusUniqProcessSpecialSuperJumpTurn;
