#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/marth/ft_marth_status_uniq_process.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

void ftMarthStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    soKineticEnergyNormal& motion = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float speedX = sumSpeed.m_x;
    float speedY = sumSpeed.m_y;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case ftMarthStatus::SpecialS:
        // First entry removes downward velocity; otherwise use the configured vertical speed.
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            speedX *= soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialS_AirSpeedXRatio, 0);
            float initialSpeedY = speedY < 0.0f ? 0.0f : soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialS_RiseSpeedY, 0);
            stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.enable();
            gravity.resetEnergy(0, &Vec2f(0.0f, initialSpeedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.enable();
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
        } else {
            motion.resetEnergy(3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            motion.enable();
            ftKineticEnergyDisableAndClear(3, moduleAccesser);
            ftKineticEnergyDisableAndClear(1, moduleAccesser);
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), ftMarthWork::SpecialS_Situation);
        break;
    case ftMarthStatus::SpecialS2:
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.enable();
            gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.enable();
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
        } else {
            ftKineticEnergyDisableAndClear(3, moduleAccesser);
            ftKineticEnergyDisableAndClear(1, moduleAccesser);
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), ftMarthWork::SpecialS_Situation);
        break;
    case ftMarthStatus::SpecialS3:
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.enable();
            gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.enable();
        } else {
            motion.resetEnergy(3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            motion.enable();
            ftKineticEnergyDisableAndClear(3, moduleAccesser);
            ftKineticEnergyDisableAndClear(1, moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), ftMarthWork::SpecialS_Situation);
        break;
    case ftMarthStatus::SpecialS4:
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.enable();
            gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.enable();
        } else {
            motion.resetEnergy(3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            motion.enable();
            ftKineticEnergyDisableAndClear(3, moduleAccesser);
            ftKineticEnergyDisableAndClear(1, moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), ftMarthWork::SpecialS_Situation);
        break;
    }
    moduleAccesser->getKineticModule().getEnergy(2)->disable();
}

void ftMarthStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    int situation = moduleAccesser->getSituationModule().getKind();
    // Rebuild movement only when Marth changes between ground and air.
    int previousSituation = moduleAccesser->getWorkManageModule().getInt(ftMarthWork::SpecialS_Situation);
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    if (situation != previousSituation) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        soKineticEnergyNormal& motion = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), ftMarthWork::SpecialS_Situation);
        switch (moduleAccesser->getStatusModule().getStatusKind()) {
        case ftMarthStatus::SpecialS:
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                stop.resetEnergy(6, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                stop.enable();
                gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                gravity.enable();
            } else {
                motion.resetEnergy(3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                motion.enable();
                ftKineticEnergyDisableAndClear(3, moduleAccesser);
                ftKineticEnergyDisableAndClear(1, moduleAccesser);
            }
            break;
        case ftMarthStatus::SpecialS2:
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                stop.resetEnergy(6, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                stop.enable();
                gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                gravity.enable();
                ftKineticEnergyDisableAndClear(2, moduleAccesser);
            } else {
                ftKineticEnergyDisableAndClear(3, moduleAccesser);
                ftKineticEnergyDisableAndClear(1, moduleAccesser);
                ftKineticEnergyDisableAndClear(2, moduleAccesser);
            }
            break;
        case ftMarthStatus::SpecialS3:
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                stop.resetEnergy(6, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                stop.enable();
                gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                gravity.enable();
            } else {
                motion.resetEnergy(3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                motion.enable();
                ftKineticEnergyDisableAndClear(3, moduleAccesser);
                ftKineticEnergyDisableAndClear(1, moduleAccesser);
            }
            break;
        case ftMarthStatus::SpecialS4:
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                stop.resetEnergy(6, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                stop.enable();
                gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                gravity.enable();
            } else {
                motion.resetEnergy(3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                motion.enable();
                ftKineticEnergyDisableAndClear(3, moduleAccesser);
                ftKineticEnergyDisableAndClear(1, moduleAccesser);
            }
            break;
        }
    }
}

// Shared virtual defaults are emitted in their original function order.
void soStatusUniqProcess::execStop(soModuleAccesser*) {}
void soStatusUniqProcess::execFixPos(soModuleAccesser*) {}
void soStatusUniqProcess::exitStatus(soModuleAccesser*, int) {}
bool soStatusUniqProcess::checkTransitionPrecede(soModuleAccesser*, void*, int) { return true; }
void soStatusUniqProcess::leaveStop(soModuleAccesser*, int, bool) {}
bool soStatusUniqProcess::onChangeLr(soModuleAccesser*, float, float) { return false; }
void soStatusUniqProcess::checkAttack(soModuleAccesser*, void*, float) {}
bool soStatusUniqProcess::checkDamage(soModuleAccesser*, void*) { return false; }
void soStatusUniqProcess::execFixCamera(soModuleAccesser*) {}
void soStatusUniqProcess::execFixPosCounter(soModuleAccesser*) {}
void soStatusUniqProcess::execMapCorrection(soModuleAccesser*) {}

ftMarthStatusUniqProcessSpecialS g_ftMarthStatusUniqProcessSpecialS;
