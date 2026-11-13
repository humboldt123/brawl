#pragma once

// The glow module of an enemy (soModuleAccesser slot glowModule). Enemy::resetGlowParam drives it.
// HYPOTHESIS: class name. Split out of em/em_enemy_modules.h so an article unit can embed it
// without including that header (it includes wn/wn_em_simple.h first).
#include <StaticAssert.h>
#include <types.h>

class soModuleAccesser;

class soGlowModuleImpl {
    u8 unk4[0x24 - 4];
public:
    soGlowModuleImpl(soModuleAccesser* acc);
    virtual ~soGlowModuleImpl();
};
static_assert(sizeof(soGlowModuleImpl) == 0x24, "Class is wrong size!");
