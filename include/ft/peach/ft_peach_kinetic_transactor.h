#pragma once

#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <mt/mt_vector.h>

class soModuleAccesser;

// Peach's own kinetic modes (the code is in ft_peach_kinetic_transactor.cpp). The mediator in ft_peach.cpp calls the
// specializations of updateEnergy for the motion, controller and stop energies out of line; the other energies use the
// generic template.
// The setups and update helpers take a tag temporary naming their mode (soIntToType, as the empty hook cases do).
// HYPOTHESIS: the modes 0x64..0x6e select the numbered setups changeKineticSub..Sub10 (Peach's floating and the
// Kirby copy-ability variants); 0x6f..0x71 only enable the outside energy.
class ftPeachKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    static void changeKineticSub(const soIntToType<0x64>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub1(const soIntToType<0x65>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub2(const soIntToType<0x66>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub3(const soIntToType<0x67>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub4(const soIntToType<0x68>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub5(const soIntToType<0x69>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub6(const soIntToType<0x6a>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub7(const soIntToType<0x6b>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub8(const soIntToType<0x6c>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub9(const soIntToType<0x6d>&, void* pools, Vec2f* speed, soModuleAccesser* acc);
    // BYTES: the native dispatcher calls this setup out of line.
    static void changeKineticSub10(const soIntToType<0x6e>&, void* pools, Vec2f* speed, soModuleAccesser* acc) __attribute__((never_inline));

    // Tilts the motion energy with the stick while mode 0x65 is active.
    static void updateEnergySub(ftKineticEnergyMotion* energy, const soIntToType<0x65>&, soModuleAccesser* acc);
    // Controller acceleration of mode 0x66 while the stick is pushed.
    static void updateEnergySub1(ftKineticEnergyController* energy, const soIntToType<0x66>&, soModuleAccesser* acc);

    // The modes without an update of their own run the energy unchanged.
    template <int Mode, typename E>
    static void updateEnergyHook(const soIntToType<Mode>&, E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() == true && energy->isSuspend() == false) {
            energy->updateEnergy(acc);
        }
    }

    // HYPOTHESIS: the stop energy runs the shared update (the base class instance) while mode 0x6c is active.
    template <int Mode, typename E>
    static void updateEnergyShared(const soIntToType<Mode>&, E* energy, soModuleAccesser* acc) {
        ftKineticTransactor::updateEnergy(energy, acc);
    }

    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() != true) {
            return;
        }
        if (energy->isSuspend()) {
            return;
        }
        return energy->updateEnergy(acc);
    }
};

template <> void ftPeachKineticTransactor::updateEnergy<ftKineticEnergyMotion>(ftKineticEnergyMotion* energy, soModuleAccesser* acc);
template <> void ftPeachKineticTransactor::updateEnergy<ftKineticEnergyController>(ftKineticEnergyController* energy, soModuleAccesser* acc);
template <> void ftPeachKineticTransactor::updateEnergy<ftKineticEnergyStop>(ftKineticEnergyStop* energy, soModuleAccesser* acc);
