#pragma once

#include <types.h>
class soModuleAccesser;

class wnDededeFollowerKineticTransactor {
    // HYPOTHESIS: a zeroed one-byte temporary is observed; its field meaning is unknown, and the helpers never read this.
    u8 unk0;
public:
    // HYPOTHESIS: the numbered launch routines selected by changeKinetic modes 0x20..0x30 (names follow the map).
    void changeKineticSub(void* pools, soModuleAccesser* moduleAccesser);
    void changeKineticSub1(void* pools, soModuleAccesser* moduleAccesser);
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

    // HYPOTHESIS: the pool and speed arguments keep the untyped form of the Diddy Kong barrel transactor.
    static void changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser);
    static void addSpeed(void* speed, void* pools, soModuleAccesser* moduleAccesser);
    static void addSpeedOutside(int type, void* speed, void* pools, soModuleAccesser* moduleAccesser);
};
