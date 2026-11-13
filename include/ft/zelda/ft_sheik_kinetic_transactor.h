#pragma once

#include <ft/builder/ft_builder_kinetic.h>
#include <types.h>

// Sheik's own kinetic modes 0x64..0x68 (ft_sheik_kinetic_transactor.cpp); the lower modes are the common ones.
class ftSheikKineticTransactor : public ftKineticTransactor {
public:
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    static void changeKineticSub(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub1(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub2(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
    static void changeKineticSub3(bool* result, void* pools, Vec2f* speed, soModuleAccesser* acc);
};
