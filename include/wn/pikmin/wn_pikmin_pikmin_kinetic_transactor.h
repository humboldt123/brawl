#pragma once

#include <types.h>

class soModuleAccesser;
class soKineticEnergyNormal;
class wnKineticEnergyGravity;

// HYPOTHESIS: like the other article transactors, the original passes an empty one-byte object as the implicit
// this of the changeKineticSub* helpers; the helpers never read it. `pools` is the opaque energy-pool storage.
class wnPikminPikminKineticTransactor {
    u8 unk0;

public:
    static void addSpeed(float* speed, void* pools, soModuleAccesser* moduleAccesser);
    static void addSpeedOutside(int type, float* speed, void* pools, soModuleAccesser* moduleAccesser);
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);
    // The mediator hands the speed over untyped.
    static void addSpeed(void* speed, void* pools, soModuleAccesser* moduleAccesser) {
        addSpeed(static_cast<float*>(speed), pools, moduleAccesser);
    }
    static void addSpeedOutside(int type, void* speed, void* pools, soModuleAccesser* moduleAccesser) {
        addSpeedOutside(type, static_cast<float*>(speed), pools, moduleAccesser);
    }

    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub1(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub1Again(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub2(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub3(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub4(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub5(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub6(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub7(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub8(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub9(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub10(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub11(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub12(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub13(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub14(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub15(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub16(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub17(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub18(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub19(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub20(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub21(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub22(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub23(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub24(void* pools, soModuleAccesser* moduleAccesser);

    // Per-energy update policies. The generic one is the shared enabled-and-not-suspended gate; the normal and
    // gravity energies have their own mode-dependent updates.
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* moduleAccesser) {
        // Separate returns: one combined condition compiles the second test as beq/blr instead of a conditional return.
        if (energy->isEnable() != true) {
            return;
        }
        if (energy->isSuspend()) {
            return;
        }
        energy->updateEnergy(moduleAccesser);
    }
    static void updateEnergy(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy1(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy2(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy3(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy4(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy5(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy6(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy7(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy8(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy9(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy10(soKineticEnergyNormal* energy, soModuleAccesser* moduleAccesser);
    static void updateEnergy(wnKineticEnergyGravity* energy, soModuleAccesser* moduleAccesser);
    void updateEnergy12(wnKineticEnergyGravity* energy, soModuleAccesser* moduleAccesser);

    // The parameter-derived limits shared by several sub-modes (defined next to the gravity energy).
    static float getStableSpeed(soModuleAccesser* moduleAccesser);
    static float getBrake(soModuleAccesser* moduleAccesser);
    static float getLimitSpeed(soModuleAccesser* moduleAccesser);
    static float getAccel(soModuleAccesser* moduleAccesser);
};
