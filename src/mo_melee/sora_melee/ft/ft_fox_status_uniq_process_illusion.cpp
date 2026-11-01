#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <ft/fox/ft_fox_status_uniq_process.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// Fox's side special (Fox Illusion). The work int 0x20000000 holds the phase set by the animation (0: launch, 1: dash,
// 2: recovery), 0x20000001 the phase the kinetic energies were last set up for, 0x20000002 a countdown.
// HYPOTHESIS: names of the phases and of the work values.
ftFoxStatusUniqProcessIllusion g_ftFoxStatusUniqProcessIllusion;

void ftFoxStatusUniqProcessIllusion::initStatus(soModuleAccesser* moduleAccesser) { }
void ftFoxStatusUniqProcessIllusion::exitStatus(soModuleAccesser* moduleAccesser, int) { }

// Sets the kinetic energies up for the phase the animation just entered.
void ftFoxStatusUniqProcessIllusion::stepChanged(soModuleAccesser* moduleAccesser) {
    int step = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    moduleAccesser->getWorkManageModule().setInt(step, 0x20000001);
    switch (step) {
    case 0: {
        // Launch: the carried speed is slowed down; in the air it becomes the horizontal speed of the stop energy and
        // the fall is halted, on the ground it is the speed of the stop energy.
        int timer = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0);
        moduleAccesser->getWorkManageModule().setInt(timer, 0x20000002);
        Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        sumSpeed.m_x /= soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0);
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.resetEnergy(6, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa7, 0), 0.0f);
            stop.enable();
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            gravity.m_speedY = 0.0f;
            gravity.m_gravity = 0.0f;
            gravity.enable();
        } else {
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_speed = Vec2f(sumSpeed.m_x, 0.0f);
        }
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
        break;
    }
    case 2:
        // Recovery: brake to a stop and drift forward with the facing.
        int timer = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc2, 0);
        moduleAccesser->getWorkManageModule().setInt(timer, 0x20000002);
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0), 0.0f);
            float lr = moduleAccesser->getPostureModule().getLr();
            float speedX = soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0);
            speedX *= lr;
            stop.m_speed = Vec2f(speedX, 0.0f);
            stop.enable();
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            gravity.m_gravity = 0.0f;
        } else {
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfaa, 0), 0.0f);
            float lr = moduleAccesser->getPostureModule().getLr();
            float speedX = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa9, 0);
            speedX *= lr;
            stop.m_speed = Vec2f(speedX, 0.0f);
            stop.enable();
        }
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
        break;
    }
}

void ftFoxStatusUniqProcessIllusion::execStatus(soModuleAccesser* moduleAccesser) {
    int step = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    if (step != moduleAccesser->getWorkManageModule().getInt(0x20000001)) {
        stepChanged(moduleAccesser);
    }
    switch (step) {
    case 0:
        // In the air, once the countdown is over, gravity pulls again (reversed in sign: the value is a speed cap).
        if (moduleAccesser->getSituationModule().getKind() == 2 && moduleAccesser->getWorkManageModule().getInt(0x20000002) == 0) {
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            gravity.resetEnergy(0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfa8, 0);
            gravity.enable();
        }
        break;
    case 1: {
        // Pressing the special button again ends the dash early: both flags are raised for the animation and the phase advances.
        soControllerModule& controller = moduleAccesser->getControllerModule();
        u32 specialMask = soController::getButtonMask(soController::Pad_Button_Special);
        int trigger = controller.getTrigger();
        if (trigger & specialMask) {
            moduleAccesser->getWorkManageModule().incInt(0x20000000);
            moduleAccesser->getWorkManageModule().onFlag(0x22000011);
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
            stepChanged(moduleAccesser);
        }
        break;
    }
    case 2:
        if (moduleAccesser->getSituationModule().getKind() == 2 && moduleAccesser->getWorkManageModule().getInt(0x20000002) == 0) {
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            gravity.resetEnergy(0, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfad, 0);
            gravity.enable();
        }
        break;
    }
}
