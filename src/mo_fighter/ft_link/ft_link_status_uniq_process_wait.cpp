#include <ft/ft_entry.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <types.h>

class ftLinkStatusUniqProcessWait : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    void updateShield(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
};

void ftLinkStatusUniqProcessWait::initStatus(soModuleAccesser* moduleAccesser) { updateShield(moduleAccesser); }

void ftLinkStatusUniqProcessWait::execStatus(soModuleAccesser* moduleAccesser) { updateShield(moduleAccesser); }

void ftLinkStatusUniqProcessWait::updateShield(soModuleAccesser* moduleAccesser) {
    Vec3f scale;
    scale.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb1, 0);
    scale.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb2, 0);
    scale.m_z = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0);
    moduleAccesser->getModelModule().setNodeScale(0xfd, &scale);
}

void ftLinkStatusUniqProcessWait::execStop(soModuleAccesser* moduleAccesser) { updateShield(moduleAccesser); }

ftLinkStatusUniqProcessWait g_ftLinkStatusUniqProcessWait;
