#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/zelda/ft_sheik_kinetic_transactor.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// Sheik's kinetic modes 0x64..0x68; the lower modes go to the common transactor. Unlike most fighters, none of Sheik's own
// modes enables the outside energies afterwards.
// HYPOTHESIS: which statuses use these modes (the constants 0xfa7/0xfa8 and 0xfb4/0xfb5 are Sheik's fall parameters).
// MATCH-ONLY: the accesser copy and the shared flag local order the argument setup (as in the other fighters' transactors).
#pragma push
#pragma opt_propagation off
void ftSheikKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
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
    case 0x68:
        ftKineticTransactor::changeKineticHook(soIntToType<0x68>(), pools, acc);
        break;
    }
}
#pragma pop

// Gravity keeps the vertical speed of the entry, the motion energy (mode 4) the horizontal one.
void ftSheikKineticTransactor::changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(0, acc, 4, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

// Gravity keeps the vertical speed and the stop energy (mode 7) the horizontal one; gravity then uses Sheik's own fall
// acceleration and maximum fall speed.
void ftSheikKineticTransactor::changeKineticSub1(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 7, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    float accel = soValueAccesser::getConstantFloat(acc, 0xfa7, 0);
    float maxSpeed = soValueAccesser::getConstantFloat(acc, 0xfa8, 0);
    gravity.m_gravity = -accel;
    gravity.m_fallSpeedMax = maxSpeed;
    gravity.unk1C = maxSpeed;
}

// The stop energy (mode 0x16) carries the whole entry speed with no acceleration.
void ftSheikKineticTransactor::changeKineticSub2(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(3, acc, 0x16, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    stop.m_accel = Vec2f(0.0f, 0.0f);
    stop.m_speedTarget = *speed;
    stop.m_speedLimit = *speed;
}

// As mode 0x65 with the second pair of fall parameters.
void ftSheikKineticTransactor::changeKineticSub3(bool*, void*, Vec2f* speed, soModuleAccesser* acc) {
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 7, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    float accel = soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
    float maxSpeed = soValueAccesser::getConstantFloat(acc, 0xfb5, 0);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
    gravity.m_gravity = -accel;
    gravity.m_fallSpeedMax = maxSpeed;
    gravity.unk1C = maxSpeed;
}
