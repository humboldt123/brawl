#include <ft/ft_entry.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/collision/so_collision_shield_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftNessStatusUniqProcessAttackS4 : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessAttackS4() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftNessStatusUniqProcessAttackS4::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    if (workManage.isFlag(0x22000010)) {
        moduleAccesser->getCollisionReflectorModule().setStatus(0, 1, 2);
        workManage.offFlag(0x22000010);
    }
    if (workManage.isFlag(0x22000011)) {
        moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
        workManage.offFlag(0x22000011);
    }
}

ftNessStatusUniqProcessAttackS4 g_ftNessStatusUniqProcessAttackS4;
