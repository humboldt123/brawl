#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// Same declaration as include/ft/zelda/ft_zelda.h (that header pulls in the whole fighter builder).
FT_KINETIC_TRANSACTOR(ftZeldaKineticTransactor);

// Splits the carried speed between the gravity energy (vertical part) and the stop energy (horizontal part).
// HYPOTHESIS: the sibling ftPeachKineticTransactor keeps its per-mode bodies as changeKineticSub* members; Zelda's map has
// only changeKinetic, so whatever helpers her transactor had were inlined.
// BYTES: the native stack slots of the two calls' temporaries (gravity entry below the stop entry) only come out when this
// pair is an inlined helper; written out in each case the compiler hands the first temporary the higher slot.
static inline void resetGravityAndStopEnergy(soModuleAccesser* acc, Vec2f* speed) {
    soKineticUtility::resetEnableEnergy(1, acc, 0, &Vec2f(0.0f, speed->m_y), &Vec3f(0.0f, 0.0f, 0.0f));
    soKineticUtility::resetEnableEnergy(3, acc, 7, &Vec2f(speed->m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
}

// Hands the whole carried speed to the stop energy and holds it there (no deceleration).
// BYTES: an inlined helper like the one above; when only the gravity/stop pair is inlined, this case's temporaries get the
// top slots instead.
static inline void holdSpeedWithStopEnergy(soModuleAccesser* acc, Vec2f* speed) {
    soKineticUtility::resetEnableEnergy(3, acc, 0x16, speed, &Vec3f(0.0f, 0.0f, 0.0f));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    stop.m_accel = Vec2f(0.0f, 0.0f);
    stop.m_speedTarget = *speed;
    stop.m_speedLimit = *speed;
}

// Zelda's own kinetic modes: 0x64..0x66 and 0x68 set the gravity and stop energies from the carried speed and then override
// the gravity; 0x67 only seeds the stop energy.
// HYPOTHESIS: which special moves use which mode.
void ftZeldaKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    soModuleAccesser* processAccesser = acc;
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(processAccesser, 1, 1);
    switch (mode) {
    case 0x64: {
        resetGravityAndStopEnergy(acc, &speed);
        float gravity = acc->getConstantFloatKirby(0xfa2);
        ftKineticEnergyGravity& energy = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        energy.m_gravity = -gravity;
        break;
    }
    case 0x65: {
        resetGravityAndStopEnergy(acc, &speed);
        float gravity = soValueAccesser::getConstantFloat(acc, 0xfa5, 0);
        ftKineticEnergyGravity& energy = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        energy.m_gravity = -gravity;
        break;
    }
    case 0x66: {
        resetGravityAndStopEnergy(acc, &speed);
        float gravity = soValueAccesser::getConstantFloat(acc, 0xfa9, 0);
        float fallSpeedMax = soValueAccesser::getConstantFloat(acc, 0xfaa, 0);
        ftKineticEnergyGravity& energy = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        energy.m_gravity = -gravity;
        energy.m_fallSpeedMax = fallSpeedMax;
        energy.unk1C = fallSpeedMax;
        break;
    }
    case 0x67: {
        holdSpeedWithStopEnergy(acc, &speed);
        break;
    }
    case 0x68: {
        resetGravityAndStopEnergy(acc, &speed);
        float gravity = soValueAccesser::getConstantFloat(acc, 0xfb5, 0);
        float fallSpeedMax = soValueAccesser::getConstantFloat(acc, 0xfb6, 0);
        ftKineticEnergyGravity& energy = dynamic_cast<ftKineticEnergyGravity&>(*acc->getKineticModule().getEnergy(1));
        energy.m_gravity = -gravity;
        energy.m_fallSpeedMax = fallSpeedMax;
        energy.unk1C = fallSpeedMax;
        break;
    }
    default:
        return;
    }
}
