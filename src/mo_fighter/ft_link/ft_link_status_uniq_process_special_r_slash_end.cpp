#include <ft/ft_entry.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftLinkStatusUniqProcessSpecialRSlashEnd : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessSpecialRSlashEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftLinkStatusUniqProcessSpecialRSlashEnd::initStatus(soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    int limit = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc0, 0);
    float value = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0);
    // HYPOTHESIS: base constant from lbl_93_rodata_40 (guess 1.0f)
    float base = 1.0f;
    float diff = value - base;
    float result = base + (float(count) / float(limit)) * diff;
    moduleAccesser->getCollisionAttackModule().setPowerMulStatus(result);
}

void ftLinkStatusUniqProcessSpecialRSlashEnd::execStatus(soModuleAccesser*) { }

ftLinkStatusUniqProcessSpecialRSlashEnd g_ftLinkStatusUniqProcessSpecialRSlashEnd;
