#include <ft/ft_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// HYPOTHESIS: member soModuleAccesser::set_landing_frame(float) in the original; no member declared, so a file-local helper.
static void soModuleAccesser__set_landing_frame(soModuleAccesser* moduleAccesser, float value);

class ftCaptainStatusUniqProcessFalconKnuckle : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconKnuckle() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    void transitionToFall(soModuleAccesser* moduleAccesser);
    void checkHitValidFlag(soModuleAccesser* moduleAccesser);
};

void ftCaptainStatusUniqProcessFalconKnuckle::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftCaptainStatusUniqProcessFalconKnuckle::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x118) {
        if (moduleAccesser->getWorkManageModule().getInt(0x20000000) == 0) {
            soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            Vec2f speed = energy.getSpeed();
            float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0);
            Vec2f scaled;
            scaled.m_x = speed.m_x * scale;
            scaled.m_y = speed.m_y * scale;
            Vec2f copy = scaled;
            energy.m_speed.m_x = copy.m_x;
            energy.m_speed.m_y = copy.m_y;
        }
    }
}

void ftCaptainStatusUniqProcessFalconKnuckle::exitStatus(soModuleAccesser* moduleAccesser, int) {
    int flag = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x113:
        if (flag == 2) {
            float value = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa8, 0);
            soModuleAccesser__set_landing_frame(moduleAccesser, value);
        }
        break;
    case 0x118:
        if (flag == 2) {
            float value = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa9, 0);
            soModuleAccesser__set_landing_frame(moduleAccesser, value);
        }
        break;
    }
}

void ftCaptainStatusUniqProcessFalconKnuckle::transitionToFall(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000000) == 0) {
        soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(1));
        float a = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0);
        float b = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa7, 0);
        energy.m_accel.m_x = -a;
        energy.m_speedTarget.m_y = b;
    }
}

void ftCaptainStatusUniqProcessFalconKnuckle::checkHitValidFlag(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    if (workManage.isFlag(0x22000011)) {
        workManage.onFlag(0x22000010);
    }
}

static void soModuleAccesser__set_landing_frame(soModuleAccesser* moduleAccesser, float value) {
    moduleAccesser->getWorkManageModule().setFloat(value, 0x11000000);
    moduleAccesser->getWorkManageModule().onFlag(0x12000003);
}

ftCaptainStatusUniqProcessFalconKnuckle g_ftCaptainStatusUniqProcessFalconKnuckle;
