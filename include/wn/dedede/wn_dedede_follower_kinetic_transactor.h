#pragma once

#include <types.h>
class soModuleAccesser;

class wnDededeFollowerKineticTransactor {
public:
    // HYPOTHESIS: the pool and speed arguments keep the untyped form of the Diddy Kong barrel transactor.
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);
    static void addSpeed(void* speed, void* pools, soModuleAccesser* moduleAccesser);
    static void addSpeedOutside(int type, void* speed, void* pools, soModuleAccesser* moduleAccesser);
};
