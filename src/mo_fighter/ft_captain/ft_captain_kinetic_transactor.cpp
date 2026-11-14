#define FT_KINETIC_MEDIATOR_POST_UPDATE // the mediator hands postUpdateEnergy to the transactor
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/captain/ft_captain.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// Captain Falcon's kinetic modes 0x64..0x6D; the lower modes go to the common transactor.
// HYPOTHESIS: which statuses use these modes.
// MATCH-ONLY: the accesser copy and the shared flag local order the argument setup (as in the other fighters' transactors).
#pragma push
#pragma opt_propagation off
void ftCaptainKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    soModuleAccesser* processAccesser = acc;
    int process = 1;
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
    case 0x66:
        changeKineticHook(soIntToType<0x66>(), pools, acc);
        break;
    case 0x67:
        changeKineticHook(soIntToType<0x67>(), pools, acc);
        break;
    case 0x68:
        changeKineticHook(soIntToType<0x68>(), pools, acc);
        break;
    case 0x69:
        changeKineticHook(soIntToType<0x69>(), pools, acc);
        break;
    case 0x6a:
        changeKineticHook(soIntToType<0x6a>(), pools, acc);
        break;
    case 0x6b:
        changeKineticHook(soIntToType<0x6b>(), pools, acc);
        break;
    case 0x6c:
        changeKineticHook(soIntToType<0x6c>(), pools, acc);
        break;
    case 0x6d:
        changeKineticHook(soIntToType<0x6d>(), pools, acc);
        break;
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}
#pragma pop

// The motion energy (mode 4) restarts from rest, the controller keeps the horizontal speed of the entry with its own
// brake (constant 0xc47) and target/limit speed (0xbd5 times 0xfac), and the stop energy takes mode 0x16.
// HYPOTHESIS: the meaning of the constants.
void ftCaptainKineticTransactor::changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(0, acc, 4, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(2, acc, 0, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    float mul = soValueAccesser::getConstantFloat(acc, 0xfac, 0);
    float base = soValueAccesser::getConstantFloat(acc, 0xbd5, 0);
    float limit = base * mul;
    controller.m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xc47, 0), 0.0f);
    controller.m_speedTarget = Vec2f(limit, 0.0f);
    controller.m_speedLimit = Vec2f(limit, 0.0f);
    soKineticUtility::resetEnableEnergy(3, acc, 0x16, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

// The common mode 0x13 with the motion energy off and gravity from Captain Falcon's constant 0xfb2.
void ftCaptainKineticTransactor::changeKineticSub1(bool*, void* pools, Vec2f* speed, soModuleAccesser* acc) {
    ftKineticTransactor::changeKineticImpl(0x13, pools, speed, acc);
    acc->getKineticModule().getEnergy(0)->disable();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -soValueAccesser::getConstantFloat(acc, 0xfb2, 0);
}

// The controller is not updated here in mode 0x64: postUpdateEnergy drives it once the total speed is known.
template <>
void ftCaptainKineticTransactor::updateEnergy<ftKineticEnergyController>(ftKineticEnergyController* energy,
                                                                         soModuleAccesser* acc) {
    switch (acc->getKineticModule().getKineticType()) {
    case 0x64:
        break;
    default:
        energy->updateEnergy(acc);
        break;
    }
}

// HYPOTHESIS: the pull-back test takes the magnitude through a small by-value helper (the same idiom as the task id
// accessors of this fighter); with it the zero of the pull vector and the magnitude take the original's float
// registers. The first test of the function reads fabs directly (the helper there changes its registers).
static inline float absSpeed(float speed) {
    float magnitude = (float)fabs(speed);
    return magnitude;
}

// In mode 0x64 the controller stops accelerating once the total horizontal speed is past its limit; otherwise its
// acceleration is the base 0xbd3 times 0xfaa on the ground or 0xfab in the air. After the update the stop energy pulls
// any speed above the limit back to it.
// HYPOTHESIS: the meaning of the constants.
void ftCaptainKineticTransactor::postUpdateEnergy(void*, soModuleAccesser* acc) {
    if (acc->getKineticModule().getKineticType() != 0x64) {
        return;
    }
    ftKineticEnergyController& controller =
        dynamic_cast<ftKineticEnergyController&>(*acc->getKineticModule().getEnergy(2));
    Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    Vec2f limit = controller.m_speedLimit;
    if ((float)fabs(speed.m_x) > limit.m_x) {
        controller.m_accel = Vec2f(0.0f, 0.0f);
        controller.updateEnergy(acc);
    } else {
        float accel = soValueAccesser::getConstantFloat(acc, 0xbd3, 0);
        float accelMul;
        if (acc->getSituationModule().getKind() == Situation_Ground) {
            accelMul = accel * soValueAccesser::getConstantFloat(acc, 0xfaa, 0);
        } else {
            accelMul = accel * soValueAccesser::getConstantFloat(acc, 0xfab, 0);
        }
        controller.m_accelMul = accelMul;
        controller.updateEnergy(acc);
    }
    speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    Vec2f pull(0.0f, 0.0f);
    if (absSpeed(speed.m_x) > limit.m_x) {
        pull.m_x = (speed.m_x > 0.0f ? limit.m_x : -limit.m_x) - speed.m_x;
    }
    stop.m_speed = pull;
}
