#include <ft/captain/ft_captain.h>
#include <ft/captain/ft_captain_status_uniq_process_falcon_kick.h>
#include <ft/ft_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>


// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftCaptainStatusUniqProcessFalconKickEnd : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconKickEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftCaptainStatusUniqProcessFalconKick::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftCaptainStatusUniqProcessFalconKick::execStatus(soModuleAccesser* moduleAccesser) {
}


void ftCaptainStatusUniqProcessFalconKick::setEventCollisionAttack(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soStatusModule& status = moduleAccesser->getStatusModule();
    int variant = workManage.getInt(0x20000000);
    moduleAccesser->getSituationModule().getKind();
    switch (status.getStatusKind()) {
    case 0x115:
        if (variant == 0) {
            int frame = workManage.getInt(0x20000001);
            if (frame >= 1) {
                float* param = ftCaptain::getFalconKickParam();
                float posture = workManage.getFloat(0x21000004);
                float speed = posture * param[1];
                ftKineticEnergyMotion& motion =
                    dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
                motion.m_speedMul = speed;
                workManage.setFloat(speed, 0x21000004);
                workManage.setInt(frame - 1, 0x20000001);
            }
        }
        break;
    case 0x11b:
        break;
    }
}

void ftCaptainStatusUniqProcessFalconKickEnd::initStatus(soModuleAccesser* moduleAccesser) {
    int situation = moduleAccesser->getSituationModule().getKind();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    int variant = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    if (variant == 0 && situation == 0) {
        Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
        soKineticEnergy* stop = kinetic.getEnergy(3);
        stop->resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    } else if (variant == 2 && situation == 2) {
        Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
        kinetic.unableEnergyAll();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        soKineticEnergy* stop = kinetic.getEnergy(3);
        gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop->resetEnergy(9, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        stop->enable();
    }
}

// The end of the kick brakes with constant 0xfb7 (ground variant 0) or 0xfb8 (variant 2) while on the ground.
// HYPOTHESIS: work int 0x20000000 is the kick variant set at the start (0 = ground start, 2 = air start).
void ftCaptainStatusUniqProcessFalconKickEnd::execStatus(soModuleAccesser* moduleAccesser) {
    int situation = moduleAccesser->getSituationModule().getKind();
    int variant = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    if (variant == 0 && situation == 0) {
        moduleAccesser->set_special_brake_coefficient(0xfb7, 0xbbc);
    } else if (variant == 2 && situation == 0) {
        moduleAccesser->set_special_brake_coefficient(0xfb8, 0xbbc);
    }
}

void soModuleAccesser::set_special_brake_coefficient(u32 airId, u32 groundId) {
    soWorkManageModule& workManage = getWorkManageModule();
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*getKineticModule().getEnergy(3));
    float coefficient;
    if (workManage.isFlag(0x22000010)) {
        coefficient = soValueAccesser::getConstantFloat(this, airId, 0);
    } else {
        coefficient = soValueAccesser::getConstantFloat(this, groundId, 0);
    }
    Vec2f brake;
    brake.m_x = coefficient;
    brake.m_y = 0.0f;
    energy.m_brake.m_x = brake.m_x;
    energy.m_brake.m_y = brake.m_y;
}

ftCaptainStatusUniqProcessFalconKick g_ftCaptainStatusUniqProcessFalconKick;
ftCaptainStatusUniqProcessFalconKickEnd g_ftCaptainStatusUniqProcessFalconKickEnd;
