#include <ft/marth/ft_marth_status_uniq_process.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_manager.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// Counter preserves horizontal speed on entry and changes kinetic setup on landing/takeoff.
void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

void ftMarthStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    // MATCH-ONLY: copy the returned vector with the original integer-load/store path.
    float speedX = sumSpeed.m_x;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case ftMarthStatus::SpecialLw:
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            speedX *= soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_AirSpeedXRatio, 0);
            float zero = 0.0f;
            stop.resetEnergy(6, &Vec2f(speedX, zero), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.soKineticEnergyNormal::setBrake(&Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_AirBrakeX, 0), 0.0f));
            stop.enable();
            gravity.resetEnergy(0, &Vec2f(0.0f, zero), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_Gravity, 0);
            gravity.m_fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_FallSpeedMax, 0);
            gravity.enable();
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
            moduleAccesser->getWorkManageModule().setInt(2, ftMarthWork::SpecialLw_Situation);
        } else {
            moduleAccesser->getWorkManageModule().setInt(0, ftMarthWork::SpecialLw_Situation);
        }
        break;
    case ftMarthStatus::SpecialLwHit: {
        // Scale the stored hit power, then apply the configured minimum and maximum.
        float incomingPower = moduleAccesser->getWorkManageModule().getFloat(ftMarthWork::SpecialLw_Power);
        float powerMultiplier = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_PowerRatio, 0);
        float powerMax = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_PowerMax, 0);
        if (static_cast<u8>(g_ftManager->m_mode) == 1) {
            powerMultiplier = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_PowerRatioAlt, 0);
            powerMax = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_PowerMaxAlt, 0);
        }
        float counterPower = incomingPower * powerMultiplier;
        if (counterPower < soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_PowerMin, 0)) {
            counterPower = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_PowerMin, 0);
        }
        if (counterPower > powerMax) {
            counterPower = powerMax;
        }
        moduleAccesser->getWorkManageModule().setFloat(counterPower, ftMarthWork::SpecialLw_Power);
        break;
    }
    }
}

void soKineticEnergyNormal::setBrake(Vec2f* brake) {
    m_brake.m_x = brake->m_x;
    m_brake.m_y = brake->m_y;
}

void ftMarthStatusUniqProcessSpecialLw::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
        case ftMarthStatus::SpecialLw: {
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            soSituationModule& situation = moduleAccesser->getSituationModule();
            if (situation.getKind() != work.getInt(ftMarthWork::SpecialLw_Situation)) {
                ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
                ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
                Vec2f sumSpeed;
                Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
                float speedX = sumSpeed.m_x;
                float speedY = sumSpeed.m_y;
                if (moduleAccesser->getSituationModule().getKind() == 2) {
                    stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                    stop.soKineticEnergyNormal::setBrake(&Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_AirBrakeX, 0), 0.0f));
                    stop.enable();
                    gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                    gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_Gravity, 0);
                    gravity.m_fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialLw_FallSpeedMax, 0);
                    gravity.enable();
                    ftKineticEnergyDisableAndClear(2, moduleAccesser);
                    moduleAccesser->getWorkManageModule().setInt(2, ftMarthWork::SpecialLw_Situation);
                } else {
                    moduleAccesser->getWorkManageModule().setInt(0, ftMarthWork::SpecialLw_Situation);
                }
            }
            break;
        }
        case ftMarthStatus::SpecialLwHit:
            break;
    }
}

void ftMarthStatusUniqProcessSpecialLw::execStop(soModuleAccesser*) {}

void ftMarthStatusUniqProcessSpecialLw::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
        case ftMarthStatus::SpecialLw:
            if (!moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialLw_ShieldActive)) {
                if (moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialLw_ShieldEnable)) {
                    moduleAccesser->getCollisionShieldModule().setStatus(0, 1, 1);
                    moduleAccesser->getWorkManageModule().onFlag(ftMarthWork::SpecialLw_ShieldActive);
                }
            } else if (!moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialLw_ShieldEnable)) {
                moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
                moduleAccesser->getWorkManageModule().offFlag(ftMarthWork::SpecialLw_ShieldActive);
            }
            break;
        case ftMarthStatus::SpecialLwHit: {
            soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
            float power = moduleAccesser->getWorkManageModule().getFloat(ftMarthWork::SpecialLw_Power);
            if (!(power <= 0.0f)) {
                for (int i = 0; i < (int)attack.getPartSize(); i++) {
                    if (attack.isAttack(i, false)) {
                        attack.setPower(i, (int)power, false);
                    }
                }
            }
            break;
        }
    }
}

void ftMarthStatusUniqProcessSpecialLw::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialLw_ShieldActive)) {
        moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
    }
}

ftMarthStatusUniqProcessSpecialLw g_ftMarthStatusUniqProcessSpecialLw;
