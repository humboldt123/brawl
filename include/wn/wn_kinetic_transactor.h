#pragma once

class soModuleAccesser;

// HYPOTHESIS: the native policy accepts an opaque energy-pool pointer; its
// dispatch currently uses only the kinetic kind and module accesser.
class wnKineticTransactor {
public:
    static void changeKinetic(int kineticType, void* pools, soModuleAccesser* acc);
    // sora_melee: empty bodies shared by the article mediators that add no speed of their own.
    static void addSpeed(void* speed, void* pools, soModuleAccesser* acc);
    static void addSpeedOutside(int type, void* speed, void* pools, soModuleAccesser* acc);
#ifdef WN_KINETIC_GENERIC_BUILDER
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() != true || energy->isSuspend()) return;
        energy->updateEnergy(acc);
    }
#endif
};
