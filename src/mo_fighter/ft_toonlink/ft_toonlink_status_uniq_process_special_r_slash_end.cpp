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
    float powerScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0) - 1.0f;
    float ratio = (float)chargeFrames / (float)maxFrames;
    float scaled = ratio * powerScale;
    moduleAccesser->getCollisionAttackModule().setPowerMulStatus(1.0f + scaled);
}

void ftToonLinkStatusUniqProcessSpecialRSlashEnd::execStatus(soModuleAccesser*) {}

ftToonLinkStatusUniqProcessSpecialRSlashEnd g_ftToonLinkStatusUniqProcessSpecialRSlashEnd;
