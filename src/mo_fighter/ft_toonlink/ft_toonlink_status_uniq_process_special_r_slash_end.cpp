#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

class ftToonLinkStatusUniqProcessSpecialRSlashEnd : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessSpecialRSlashEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftToonLinkStatusUniqProcessSpecialRSlashEnd::initStatus(soModuleAccesser* moduleAccesser) {
    int chargeFrames = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    int maxFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc0, 0);
    float value = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0);
    float diff;
    float base = 1.0f;
    diff = value - base;
    float ratio = (float)chargeFrames / (float)maxFrames;
    float result = base + ratio * diff;
    moduleAccesser->getCollisionAttackModule().setPowerMulStatus(result);
}

void ftToonLinkStatusUniqProcessSpecialRSlashEnd::execStatus(soModuleAccesser*) {}

ftToonLinkStatusUniqProcessSpecialRSlashEnd g_ftToonLinkStatusUniqProcessSpecialRSlashEnd;
