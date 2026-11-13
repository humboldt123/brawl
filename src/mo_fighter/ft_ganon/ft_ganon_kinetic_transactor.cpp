#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ganon/ft_ganon.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// Ganon's kinetic modes 0x64..0x68; the lower modes go to the common transactor.
// HYPOTHESIS: which statuses use these modes.
// MATCH-ONLY: the accesser copy and the shared flag local order the argument setup (as in the other fighters' transactors).
#pragma push
#pragma opt_propagation off
void ftGanonKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
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
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}
#pragma pop

// The motion energy (mode 4) restarts from rest and the controller keeps the horizontal speed of the entry.
void ftGanonKineticTransactor::changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(0, acc, 4, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    Vec2f entry = *speed;
    soKineticUtility::resetEnableEnergy(2, acc, 0, &Vec2f(entry.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

// The common mode 0x13 with the motion energy off and gravity from Ganon's constant 0xfb4.
void ftGanonKineticTransactor::changeKineticSub1(bool*, void* pools, Vec2f* speed, soModuleAccesser* acc) {
    ftKineticTransactor::changeKineticImpl(0x13, pools, speed, acc);
    acc->getKineticModule().getEnergy(0)->disable();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
}

// In mode 0x64 the controller's target speed follows the sum of its own and the motion energy's speed: past the limit
// (constant 0xfae times the base target 0xbd5) it is raised and the acceleration takes constant 0xfad.
// HYPOTHESIS: the meaning of the constants.
template <>
void ftGanonKineticTransactor::updateEnergy<ftKineticEnergyController>(ftKineticEnergyController* energy, soModuleAccesser* acc) {
    switch (acc->getKineticModule().getKineticType()) {
    case 0x64: {
        Vec2f target = energy->m_speedTarget;
        Vec2f speed = energy->getSpeed();
        speed += dynamic_cast<ftKineticEnergyMotion&>(*acc->getKineticModule().getEnergy(0)).getSpeed();
        target.m_x = soValueAccesser::getConstantFloat(acc, 0xbd5, 0);
        float accelMul = 1.0f;
        float limit = soValueAccesser::getConstantFloat(acc, 0xfae, 0);
        if ((float)fabs(speed.m_x) > target.m_x * limit) {
            target.m_x = target.m_x * limit;
            accelMul = soValueAccesser::getConstantFloat(acc, 0xfad, 0);
        }
        energy->m_speedTarget.m_x = target.m_x;
        energy->m_speedTarget.m_y = target.m_y;
        energy->m_accelMul = accelMul;
        break;
    }
    }
    energy->updateEnergy(acc);
}
