#include <ft/marth/ft_marth_status_uniq_process.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

void ftMarthStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float speedX = sumSpeed.m_x;
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        speedX *= soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_AirSpeedXRatio, 0);
        float zero = 0.0f;
        stop.resetEnergy(6, &Vec2f(speedX, zero), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        gravity.resetEnergy(0, &Vec2f(0.0f, zero), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        soKineticEnergyNormal& motion = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        motion.resetEnergy(5, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        motion.enable();
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
    } else {
        soKineticEnergyNormal& motion = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        motion.resetEnergy(3, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        motion.enable();
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        ftKineticEnergyDisableAndClear(1, moduleAccesser);
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
    }
}

#pragma dont_inline on
static float absValue(float value);
#pragma dont_inline off

static inline void resetGravity(ftKineticEnergyGravity& gravity, float speedY, soModuleAccesser* moduleAccesser) {
    Vec2f speed(0.0f, speedY);
    gravity.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
}
static inline void resetController(ftKineticEnergyController& controller, int mode, float speedX, soModuleAccesser* moduleAccesser) {
    Vec2f speed(speedX, 0.0f);
    controller.resetEnergy(mode, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
}
// MATCH-ONLY: retain the reset mode across Vec3f construction; the inline helpers
// keep the speed vectors in the original temporary slots.
#pragma opt_propagation off
void ftMarthStatusUniqProcessSpecialHi::execStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
    float stickMagnitude = absValue(moduleAccesser->getControllerModule().getStickX());
    if (!moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialHi_LrChanged)) {
        float threshold = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_StickThreshold, 0);
        if (stickMagnitude > threshold) {
            float angle = (stickMagnitude - threshold) / (1.0f - threshold);
            angle *= soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_AngleScale, 0);
            if (moduleAccesser->getControllerModule().getStickX() > 0.0f) {
                angle = -(0.01745329238474369f * angle);
            } else {
                angle = 0.01745329238474369f * angle;
            }
            // Keep the strongest stick-derived angle sampled during the launch window.
            float previousAngle = moduleAccesser->getWorkManageModule().getFloat(ftMarthWork::SpecialHi_StickAngle);
            float previousMagnitude = absValue(previousAngle);
            if (absValue(angle) > previousMagnitude) previousAngle = angle;
            moduleAccesser->getWorkManageModule().setFloat(previousAngle, ftMarthWork::SpecialHi_StickAngle);
        }
    }
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    if (moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialHi_Launched)) {
        if (!moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialHi_Falling)) {
            if (moduleAccesser->getMotionModule().getKind() == 0x1e9) {
                motion.unk40 = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_MotionUnk40, 0);
            }
            // After the first active update, falling starts the gravity/control handoff.
            if (moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialHi_Updated) && sumSpeed.m_y < 0.0f) {
                moduleAccesser->getWorkManageModule().onFlag(ftMarthWork::SpecialHi_Falling);
            }
        } else {
            if (moduleAccesser->getKineticModule().getEnergy(1)->isEnable() == 0) {
                ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
                resetGravity(gravity, sumSpeed.m_y, moduleAccesser);
                gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_Gravity, 0);
                gravity.m_fallSpeedMax = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_FallSpeedMax, 0);
                gravity.enable();
            }
            if (moduleAccesser->getKineticModule().getEnergy(2)->isEnable() == 0) {
                ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
                int resetType = 0xb;
                resetController(controller, resetType, sumSpeed.m_x, moduleAccesser);
                controller.mulXAccelMul(soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_ControlXRatio, 0));
                controller.mulXSpeedMax(soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_ControlXRatio, 0));
                controller.enable();
            }
            motion.disable();
        }
        moduleAccesser->getWorkManageModule().onFlag(ftMarthWork::SpecialHi_Updated);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialHi_ApplyAngle)) {
        motion.unk3C = moduleAccesser->getWorkManageModule().getFloat(ftMarthWork::SpecialHi_StickAngle);
        moduleAccesser->getWorkManageModule().offFlag(ftMarthWork::SpecialHi_ApplyAngle);
    }
}

#pragma opt_propagation reset
#pragma dont_inline on
static float absValue(float value) { return __fabsf(value); }
#pragma dont_inline off
void ftMarthStatusUniqProcessSpecialHi::execStop(soModuleAccesser*) {}
void ftMarthStatusUniqProcessSpecialHi::execFixPos(soModuleAccesser*) {}

void ftMarthStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int) {
    float exitValue = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_ExitValue, 0);
    moduleAccesser->getWorkManageModule().setFloat(exitValue, 0x11000000);
    float controlRatio = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::SpecialHi_ControlXRatio, 0);
    moduleAccesser->getWorkManageModule().setFloat(controlRatio, 0x11000001);
    moduleAccesser->getWorkManageModule().onFlag(0x12000003);
}

bool ftMarthStatusUniqProcessSpecialHi::onChangeLr(soModuleAccesser* moduleAccesser, float, float) {
    moduleAccesser->getWorkManageModule().setFloat(0.0f, ftMarthWork::SpecialHi_StickAngle);
    moduleAccesser->getWorkManageModule().onFlag(ftMarthWork::SpecialHi_LrChanged);
    return true;
}

ftMarthStatusUniqProcessSpecialHi g_ftMarthStatusUniqProcessSpecialHi;
