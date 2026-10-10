#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

class ftToonLinkStatusUniqProcessWait : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    void updateShield(soModuleAccesser* moduleAccesser);
};

void ftToonLinkStatusUniqProcessWait::initStatus(soModuleAccesser* moduleAccesser) {
    updateShield(moduleAccesser);
}

void ftToonLinkStatusUniqProcessWait::execStatus(soModuleAccesser* moduleAccesser) {
    updateShield(moduleAccesser);
}

void ftToonLinkStatusUniqProcessWait::updateShield(soModuleAccesser* moduleAccesser) {
    Vec3f scale;
    scale.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb1, 0);
    scale.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb2, 0);
    scale.m_z = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0);
    moduleAccesser->getModelModule().setNodeScale(0xfd, &scale);
}

void ftToonLinkStatusUniqProcessWait::execStop(soModuleAccesser* moduleAccesser) {
    updateShield(moduleAccesser);
}

ftToonLinkStatusUniqProcessWait g_ftToonLinkStatusUniqProcessWait;
