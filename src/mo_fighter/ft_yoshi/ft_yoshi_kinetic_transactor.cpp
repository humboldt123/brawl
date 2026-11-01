// MATCH-ONLY: the native helpers call these vector operations out of line.
// Set the policy before any builder header can include mt_vector.h.
#define MT_VEC2F_CTOR_OUT_OF_LINE
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/yoshi/ft_yoshi_kinetic_transactor.h>
#include <ft/yoshi/ft_yoshi.h>
#include <ft/yoshi/ft_yoshi_special_s_param.h>
#include <ft/yoshi/ft_yoshi_final_param.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <ft/builder/ft_builder_kinetic.h>

// MATCH-ONLY: retain the native accesser load followed by a shared flag copy,
// rather than propagating the flag into two independent immediate loads.
#pragma push
#pragma opt_propagation off
void ftYoshiKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    // Preprocessing also runs for an unrecognized custom mode.
    soModuleAccesser* processAccesser = acc;
    bool process = true;
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(processAccesser, process, process);
    switch (mode) {
    case 0x64: {
        bool result = false;
        changeKineticSub(&result, pools, &speed, acc);
        break;
    }
    case 0x65: {
        bool result = false;
        changeKineticSub1(&result, pools, &speed, acc);
        break;
    }
    case 0x66: {
        bool result = false;
        changeKineticSub2(&result, pools, &speed, acc);
        break;
    }
    case 0x67: {
        bool result = false;
        changeKineticSub3(&result, pools, &speed, acc);
        break;
    }
    case 0x68: {
        bool result = false;
        changeKineticSub4(&result, pools, &speed, acc);
        break;
    }
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}
#pragma pop

// Ground Egg Roll retains the incoming velocity, then applies its own
// acceleration, braking and speed bounds to Stop energy.
void ftYoshiKineticTransactor::changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(3, acc, 0, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    ftYoshiSpecialSParam* param = static_cast<ftYoshiSpecialSParam*>(
        g_ftCommonDataAccesser.getData(Fighter_Yoshi)->extendParam[0]);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(
        *acc->getKineticModule().getEnergy(3));
    Vec2f acceleration;
    acceleration.m_x = param->unk2C * acc->getPostureModule().getLr();
    acceleration.m_y = 0.0f;
    stop.m_accel = acceleration;
    Vec2f brake;
    brake.m_x = param->unk30;
    brake.m_y = 0.0f;
    stop.m_brake = brake;
    Vec2f limit;
    limit.m_x = param->unk38;
    limit.m_y = 0.0f;
    stop.m_speedLimit = limit;
    Vec2f target;
    target.m_x = param->minimumGroundSpeed;
    target.m_y = 0.0f;
    stop.m_speedTarget = target;
}

// MATCH-ONLY: this unit owns the weak constructor between the ground and air helpers.
__declspec(weak) Vec2f::Vec2f(float x, float y) { m_x = x; m_y = y; }

// Air Egg Roll separates the entry velocity into gravity and horizontal
// control before applying the Egg Roll parameter record.
void ftYoshiKineticTransactor::changeKineticSub1(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    Vec2f gravitySpeed;
    float verticalSpeed = speed->m_y;
    gravitySpeed.m_x = 0.0f;
    gravitySpeed.m_y = verticalSpeed;
    soKineticUtility::resetEnableEnergy(1, acc, 0, &gravitySpeed, &Vec3f(0.0f, 0.0f, 0.0f));
    Vec2f controllerSpeed;
    controllerSpeed.m_x = speed->m_x;
    controllerSpeed.m_y = 0.0f;
    soKineticUtility::resetEnableEnergy(2, acc, 0, &controllerSpeed, &Vec3f(0.0f, 0.0f, 0.0f));
    ftYoshiSpecialSParam* param = static_cast<ftYoshiSpecialSParam*>(
        g_ftCommonDataAccesser.getData(Fighter_Yoshi)->extendParam[0]);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(
        *acc->getKineticModule().getEnergy(1));
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(
        *acc->getKineticModule().getEnergy(2));
    controller.m_accelMul = param->unk98;
    Vec2f limit;
    limit.m_x = param->unk48;
    limit.m_y = 0.0f;
    controller.m_speedLimit = limit;
    Vec2f target;
    target.m_x = param->minimumAirSpeed;
    target.m_y = 0.0f;
    controller.m_speedTarget = target;
    gravity.m_gravity = -param->gravity;
    gravity.unk1C = param->gravityLimit;
    gravity.m_fallSpeedMax = param->gravityLimit;
}

// Super Dragon transfers entry velocity between Stop, Gravity and Controller.
// The tuning belongs to this fighter instance's fourth extension record.
void ftYoshiKineticTransactor::changeKineticSub2(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticModule& kinetic = acc->getKineticModule();
    float lr = acc->getPostureModule().getLr();
    ftYoshi& fighter = dynamic_cast<ftYoshi&>(acc->getStageObject());
    ftYoshiFinalParam* param = static_cast<ftYoshiFinalParam*>(fighter.getExtendParam()[3]);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    Vec2f entrySpeed = *speed;
    Vec2f stopSpeed;
    stopSpeed.m_x = 0.0f;
    stopSpeed.m_y = 0.0f;
    Vec2f gravitySpeed;
    gravitySpeed.m_x = 0.0f;
    gravitySpeed.m_y = 0.0f;
    Vec2f controllerSpeed;
    controllerSpeed.m_x = 0.0f;
    controllerSpeed.m_y = 0.0f;
    kinetic.unableEnergyAll();

    float directedSpeed = entrySpeed.m_x * lr;
    // Native branches reject only speeds above the limit or below zero.
    if (directedSpeed > param->unk00 || directedSpeed < 0.0f) {
        stopSpeed.m_x = 0.0f;
        controllerSpeed.m_x = entrySpeed.m_x;
    } else {
        stopSpeed.m_x = param->unk00 * lr;
        controllerSpeed.m_x = 0.0f;
        stop.enable();
    }
    if (entrySpeed.m_y < -param->unk20 || entrySpeed.m_y > 0.0f) {
        gravitySpeed.m_y = 0.0f;
        controllerSpeed.m_y = entrySpeed.m_y;
    } else {
        gravitySpeed.m_y = entrySpeed.m_y;
        controllerSpeed.m_y = 0.0f;
        gravity.enable();
    }
    Vec2f zero;
    zero.m_x = 0.0f;
    zero.m_y = 0.0f;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    stop.resetEnergy(0x16, &stopSpeed, &rotation, acc);
    stop.m_accel = zero;
    Vec2f target;
    target.m_x = param->unk00;
    target.m_y = 0.0f;
    stop.m_speedTarget = target;
    Vec2f limit;
    limit.m_x = param->unk00;
    limit.m_y = 0.0f;
    stop.m_speedLimit = limit;
    stop.m_brake = zero;
    gravity.resetEnergy(0, &gravitySpeed, &rotation, acc);
    gravity.m_gravity = -param->unk1C;
    gravity.m_fallSpeedMax = param->unk20;
    gravity.unk1C = param->unk20;
    gravity.unk18 = 0.0f;
    controller.resetEnergy(0x0B, &zero, &rotation, acc);
    controller.m_speed = controllerSpeed;
    controller.m_accelMul = param->unk04;
    float neutral = 0.0f;
    controller.m_unk40 = neutral;
    float tuning = param->unk10;
    controller.m_unk44 = tuning;
    controller.m_unk48 = neutral;
    Vec2f controllerTarget(param->unk08, param->unk14);
    controller.m_speedTarget = controllerTarget;
    Vec2f controllerLimit(param->unk08, param->unk14);
    controller.m_speedLimit = controllerLimit;
    Vec2f controllerBrake(param->unk0C, param->unk18);
    controller.m_brake = controllerBrake;
    controller.enable();
}

// The related Super Dragon air mode keeps horizontal entry speed in Controller
// energy and only transfers the vertical component into Gravity.
void ftYoshiKineticTransactor::changeKineticSub3(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticModule& kinetic = acc->getKineticModule();
    acc->getPostureModule().getLr();
    ftYoshi& fighter = dynamic_cast<ftYoshi&>(acc->getStageObject());
    ftYoshiFinalParam* param = static_cast<ftYoshiFinalParam*>(fighter.getExtendParam()[3]);
    // The checked Stop cast is part of the native transition, even though unused.
    dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*kinetic.getEnergy(2));
    Vec2f entrySpeed = *speed;
    Vec2f gravitySpeed;
    gravitySpeed.m_x = 0.0f;
    gravitySpeed.m_y = 0.0f;
    Vec2f controllerSpeed;
    controllerSpeed.m_y = 0.0f;
    controllerSpeed.m_x = entrySpeed.m_x;
    kinetic.unableEnergyAll();
    if (entrySpeed.m_y < -param->unk20 || entrySpeed.m_y > 0.0f) {
        gravitySpeed.m_y = 0.0f;
        controllerSpeed.m_y = entrySpeed.m_y;
    } else {
        gravitySpeed.m_y = entrySpeed.m_y;
        controllerSpeed.m_y = 0.0f;
        gravity.enable();
    }
    Vec2f zero;
    zero.m_x = 0.0f;
    zero.m_y = 0.0f;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    gravity.resetEnergy(0, &gravitySpeed, &rotation, acc);
    gravity.m_gravity = -param->unk1C;
    gravity.m_fallSpeedMax = param->unk20;
    gravity.unk1C = param->unk20;
    gravity.unk18 = 0.0f;
    controller.resetEnergy(0x0B, &zero, &rotation, acc);
    controller.m_speed = controllerSpeed;
    controller.m_accelMul = param->unk04;
    float neutral = 0.0f;
    controller.m_unk40 = neutral;
    float tuning = param->unk10;
    controller.m_unk44 = tuning;
    controller.m_unk48 = neutral;
    Vec2f controllerTarget(param->unk08, param->unk14);
    controller.m_speedTarget = controllerTarget;
    Vec2f controllerLimit(param->unk08, param->unk14);
    controller.m_speedLimit = controllerLimit;
    Vec2f controllerBrake(param->unk0C, param->unk18);
    controller.m_brake = controllerBrake;
    controller.enable();
}

// Mode 0x68 switches back to Stop plus Gravity, discarding other active energies.
// Parameter meanings remain numeric where the assembly does not establish them.
void ftYoshiKineticTransactor::changeKineticSub4(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticModule& kinetic = acc->getKineticModule();
    float lr = acc->getPostureModule().getLr();
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    ftYoshi& fighter = dynamic_cast<ftYoshi&>(acc->getStageObject());
    ftYoshiFinalParam* param = static_cast<ftYoshiFinalParam*>(fighter.getExtendParam()[3]);
    kinetic.unableEnergyAll();
    Vec2f zero;
    zero.m_x = 0.0f;
    zero.m_y = 0.0f;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    Vec2f stopSpeed;
    stopSpeed.m_x = speed->m_x;
    stopSpeed.m_y = 0.0f;
    stop.resetEnergy(0x16, &stopSpeed, &rotation, acc);
    Vec2f acceleration;
    acceleration.m_x = param->unk04 * lr;
    acceleration.m_y = 0.0f;
    stop.m_accel = acceleration;
    Vec2f target;
    target.m_x = param->unk00;
    target.m_y = 0.0f;
    stop.m_speedTarget = target;
    Vec2f limit;
    limit.m_x = param->unk08;
    limit.m_y = 0.0f;
    stop.m_speedLimit = limit;
    Vec2f brake;
    brake.m_x = param->unk0C;
    brake.m_y = 0.0f;
    stop.m_brake = brake;
    stop.enable();
    Vec2f gravitySpeed;
    float verticalSpeed = speed->m_y;
    gravitySpeed.m_x = 0.0f;
    gravitySpeed.m_y = verticalSpeed;
    gravity.resetEnergy(0, &gravitySpeed, &rotation, acc);
    gravity.m_gravity = -param->unk18;
    gravity.m_fallSpeedMax = param->unk20;
    gravity.unk1C = param->unk14;
    gravity.unk18 = param->unk18;
    gravity.enable();
}
