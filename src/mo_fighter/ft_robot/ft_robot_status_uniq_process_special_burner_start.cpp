#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/robot/ft_robot_status_uniq_process_special_burner_start.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// HYPOTHESIS: shared absolute value helper of the ft_robot REL (emitted once as an 8-byte function in the first user).
inline float absValue(float value) __attribute__((never_inline)) { return __fabsf(value); }

// Work variables: see ftRobotStatusUniqProcessSpecialBurner (0x11000015 fuel, 0x22000012 thrust input held).
//   flag 0x22000017  the start motion asked for the burner partial animation (HYPOTHESIS)
//   flag 0x12000040 / 0x12000041  cleared on entry (HYPOTHESIS: animcmd side flags of the start motion)
//   float 0x21000004 cleared on entry

// MATCH-ONLY: the native code builds the start speed, the zero acceleration and the stored vectors through small inline
// calls; with plain member calls and propagation off the temporaries are kept in saved registers.
static inline void resetController(ftKineticEnergyController& controller, int mode, Vec3f* accel, const Vec2f& speed, soModuleAccesser* moduleAccesser) {
    controller.resetEnergy(mode, const_cast<Vec2f*>(&speed), accel, moduleAccesser);
}
static inline void setControllerSpeed(ftKineticEnergyController& controller, const Vec2f& speed) {
    controller.m_speed = speed;
}
static inline void setControllerTarget(ftKineticEnergyController& controller, const Vec2f& target) {
    controller.m_speedTarget = target;
}
static inline void resetGravity(ftKineticEnergyGravity& gravity, float speedY, soModuleAccesser* moduleAccesser) {
    Vec2f speed(0.0f, speedY);
    gravity.resetEnergy(0, &speed, &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
}

// MATCH-ONLY: the reset mode is kept in a saved register across the speed queries and vector construction, as in the
// native register lifetime. HYPOTHESIS: the first gravity reset below still has its vector address in a saved register
// (the native code does not), the only remaining difference.
#pragma push
#pragma opt_propagation off
void ftRobotStatusUniqProcessSpecialBurnerStart::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
    moduleAccesser->getWorkManageModule().onFlag(0x22000012);
    Vec2f speed;
    Vec2f::copy(speed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    if (moduleAccesser->getWorkManageModule().getFloat(0x11000015) > 0.0f) {
        // Limit the vertical speed carried into the thrust, boosting it on the ground and softening a fall in the air.
        if (absValue(speed.m_y) > soValueAccesser::getConstantFloat(moduleAccesser, 0xfc3, 0)) {
            int sign = (speed.m_y < 0.0f) ? -1 : 1;
            float signF = sign;
            speed.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc3, 0) * signF;
        }
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            speed.m_y += soValueAccesser::getConstantFloat(moduleAccesser, 0xfc0, 0);
        } else if (speed.m_y < 0.0f) {
            speed.m_y += soValueAccesser::getConstantFloat(moduleAccesser, 0xfc1, 0);
        }
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        int controllerMode = 0xb;
        if (absValue(speed.m_x) > soValueAccesser::getConstantFloat(moduleAccesser, 0xfc2, 0)) {
            int sign = (speed.m_x < 0.0f) ? -1 : 1;
            float signF = sign;
            speed.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc2, 0) * signF;
        }
        resetController(controller, controllerMode, &Vec3f(0.0f, 0.0f, 0.0f), Vec2f(speed.m_x, 0.0f), moduleAccesser);
        setControllerSpeed(controller, Vec2f(speed.m_x, 0.0f));
        controller.m_unk40 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc7, 0);
        setControllerTarget(controller, Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfc8, 0), 0.0f));
        controller.enable();
        // The walking/jumping style energies are switched off while the burner owns the movement.
        moduleAccesser->getKineticModule().getEnergy(3)->disable();
        moduleAccesser->getKineticModule().getEnergy(0)->disable();
        moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000004);
        moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0x24000a), 0x60, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, -90.0f), 1.0f, false, 1, 0, -1);
        moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0x240002), 0x60, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, -90.0f), 1.0f, false, 1, 0, -1);
    } else {
        // Out of fuel: no lift, the fighter keeps its speed.
        int controllerMode = 0xb;
        resetController(controller, controllerMode, &Vec3f(0.0f, 0.0f, 0.0f), Vec2f(speed.m_x, 0.0f), moduleAccesser);
        setControllerSpeed(controller, Vec2f(speed.m_x, 0.0f));
        resetGravity(gravity, speed.m_y, moduleAccesser);
        gravity.enable();
    }
    moduleAccesser->getMotionModule().removePartialAnimChr(2);
    moduleAccesser->getWorkManageModule().offFlag(0x12000040);
    moduleAccesser->getWorkManageModule().offFlag(0x12000041);
}
#pragma pop

// While there is fuel the start motion already applies the lift of the thrust (and burns one unit per frame).
void ftRobotStatusUniqProcessSpecialBurnerStart::execStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    if (moduleAccesser->getWorkManageModule().getFloat(0x11000015) > 0.0f) {
        float lift = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc4, 0);
        Vec2f speed;
        Vec2f::copy(speed, gravity.getSpeed());
        moduleAccesser->getWorkManageModule().addFloat(-1.0f, 0x11000015);
        if (speed.m_y >= soValueAccesser::getConstantFloat(moduleAccesser, 0xfc5, 0)) {
            lift = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc6, 0);
        }
        gravity.m_gravity = lift;
    }
}

void ftRobotStatusUniqProcessSpecialBurnerStart::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000017)) {
        moduleAccesser->getMotionModule().addPartialAnimChr(2, 0x1d8, 0x60, 1, 0.0f, 1.0f, 0);
        moduleAccesser->getWorkManageModule().offFlag(0x22000017);
        moduleAccesser->getWorkManageModule().onFlag(0x12000040);
    }
}

void ftRobotStatusUniqProcessSpecialBurnerStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) { }

ftRobotStatusUniqProcessSpecialBurnerStart g_ftRobotStatusUniqProcessSpecialBurnerStart;
