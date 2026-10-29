#include <ft/ft_kinetic_energy.h>
#include <ft/ft_manager.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

class ftIkeStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

// HYPOTHESIS: Counter has the shape and work ids of Marth's Counter; the kinds and param ids are Ike's own.
enum {
    IkeStatus_SpecialLw = 0x115,
    IkeStatus_SpecialLwHit = 0x121,
};
enum {
    IkeWork_SpecialLw_Situation = 0x20000000,
    IkeWork_SpecialLw_Power = 0x21000004,
    IkeWork_SpecialLw_ShieldEnable = 0x22000011,
    IkeWork_SpecialLw_ShieldActive = 0x22000012,
};
enum {
    IkeParam_SpecialLw_AirSpeedXRatio = 0xfb5,
    IkeParam_SpecialLw_AirBrakeX = 0xfb6,
    IkeParam_SpecialLw_Gravity = 0xfb7,
    IkeParam_SpecialLw_FallSpeedMax = 0xfb8,
    IkeParam_SpecialLw_PowerRatio = 0xfb9,
    IkeParam_SpecialLw_PowerRatioAlt = 0xfba,
    IkeParam_SpecialLw_PowerMin = 0xfbb,
    IkeParam_SpecialLw_PowerMax = 0xfbc,
    IkeParam_SpecialLw_PowerMaxAlt = 0xfbd,
};

void ftIkeStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float speedX = sumSpeed.m_x;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case IkeStatus_SpecialLw:
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.enable();
            ftKineticEnergyDisableAndClear(1, moduleAccesser);
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
            moduleAccesser->getWorkManageModule().setInt(0, IkeWork_SpecialLw_Situation);
        } else {
            speedX *= soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_AirSpeedXRatio, 0);
            float zero = 0.0f;
            stop.resetEnergy(6, &Vec2f(speedX, zero), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.soKineticEnergyNormal::setBrake(&Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_AirBrakeX, 0), 0.0f));
            stop.enable();
            gravity.resetEnergy(0, &Vec2f(0.0f, zero), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_Gravity, 0);
            gravity.m_fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_FallSpeedMax, 0);
            gravity.enable();
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
            moduleAccesser->getWorkManageModule().setInt(2, IkeWork_SpecialLw_Situation);
        }
        break;
    case IkeStatus_SpecialLwHit: {
        float incomingPower = moduleAccesser->getWorkManageModule().getFloat(IkeWork_SpecialLw_Power);
        float powerMultiplier = soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_PowerRatio, 0);
        float powerMax = soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_PowerMax, 0);
        if (static_cast<u8>(g_ftManager->m_mode) == 1) {
            powerMultiplier = soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_PowerRatioAlt, 0);
            powerMax = soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_PowerMaxAlt, 0);
        }
        float counterPower = incomingPower * powerMultiplier;
        if (counterPower < soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_PowerMin, 0)) {
            counterPower = soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_PowerMin, 0);
        }
        if (counterPower > powerMax) {
            counterPower = powerMax;
        }
        moduleAccesser->getWorkManageModule().setFloat(counterPower, IkeWork_SpecialLw_Power);
        break;
    }
    }
}

void soKineticEnergyNormal::setBrake(Vec2f* brake) {
    m_brake.m_x = brake->m_x;
    m_brake.m_y = brake->m_y;
}

void ftIkeStatusUniqProcessSpecialLw::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
        case IkeStatus_SpecialLw: {
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            soSituationModule& situation = moduleAccesser->getSituationModule();
            if (situation.getKind() != work.getInt(IkeWork_SpecialLw_Situation)) {
                ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
                ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
                Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
                float speedX = sumSpeed.m_x;
                float speedY = sumSpeed.m_y;
                if (moduleAccesser->getSituationModule().getKind() == 0) {
                    stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                    stop.enable();
                    ftKineticEnergyDisableAndClear(1, moduleAccesser);
                    ftKineticEnergyDisableAndClear(2, moduleAccesser);
                    moduleAccesser->getWorkManageModule().setInt(0, IkeWork_SpecialLw_Situation);
                } else {
                    stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                    stop.soKineticEnergyNormal::setBrake(&Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_AirBrakeX, 0), 0.0f));
                    stop.enable();
                    gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                    gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_Gravity, 0);
                    gravity.m_fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, IkeParam_SpecialLw_FallSpeedMax, 0);
                    gravity.enable();
                    ftKineticEnergyDisableAndClear(2, moduleAccesser);
                    moduleAccesser->getWorkManageModule().setInt(2, IkeWork_SpecialLw_Situation);
                }
            }
            break;
        }
        case IkeStatus_SpecialLwHit:
            break;
    }
}

void ftIkeStatusUniqProcessSpecialLw::execStop(soModuleAccesser*) {}

void ftIkeStatusUniqProcessSpecialLw::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
        case IkeStatus_SpecialLw:
            if (!moduleAccesser->getWorkManageModule().isFlag(IkeWork_SpecialLw_ShieldActive)) {
                if (moduleAccesser->getWorkManageModule().isFlag(IkeWork_SpecialLw_ShieldEnable)) {
                    moduleAccesser->getCollisionShieldModule().setStatus(0, 1, 1);
                    moduleAccesser->getWorkManageModule().onFlag(IkeWork_SpecialLw_ShieldActive);
                }
            } else if (!moduleAccesser->getWorkManageModule().isFlag(IkeWork_SpecialLw_ShieldEnable)) {
                moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
                moduleAccesser->getWorkManageModule().offFlag(IkeWork_SpecialLw_ShieldActive);
            }
            break;
        case IkeStatus_SpecialLwHit: {
            soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
            float power = moduleAccesser->getWorkManageModule().getFloat(IkeWork_SpecialLw_Power);
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

void ftIkeStatusUniqProcessSpecialLw::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().isFlag(IkeWork_SpecialLw_ShieldActive)) {
        moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
    }
}

ftIkeStatusUniqProcessSpecialLw g_ftIkeStatusUniqProcessSpecialLw;
