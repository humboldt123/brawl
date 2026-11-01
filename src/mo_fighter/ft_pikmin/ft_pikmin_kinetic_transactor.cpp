#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/pikmin/ft_pikmin.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>

// Olimar's own kinetic modes: 0x64 is the only one (the standing carry of the line), all the others are the common ones.
// HYPOTHESIS: the meaning of mode 0x64 and of the constant 0xfac (written into the energy pool at +0x78).
// MATCH-ONLY: the two equal arguments are copied from the first one in the native code.
#pragma opt_propagation off
void ftPikminKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, moduleAccesser);
        return;
    }
    int process = 1;
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(moduleAccesser, process, process);
    switch (mode) {
    case 0x64: {
        bool result = false;
        ftPikminKineticTransactor::changeKineticSub(&result, pools, &speed, moduleAccesser);
        break;
    }
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(moduleAccesser);
}

#pragma opt_propagation reset

// BYTES: the native changeKinetic calls the mode function out of line.
#pragma dont_inline on
void ftPikminKineticTransactor::changeKineticSub(bool*, void* pools, Vec2f*, soModuleAccesser* moduleAccesser) {
    ftKineticTransactor::changeKineticImpl(0);
    // MATCH-ONLY: the original keeps a dead flag in the frame.
    volatile bool result = false;
    *reinterpret_cast<float*>(reinterpret_cast<u8*>(pools) + 0x78) = soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0);
}
#pragma dont_inline reset
