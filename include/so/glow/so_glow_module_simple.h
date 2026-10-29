#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <types.h>

class soModuleAccesser;

// The initial slots are verified against the Simple vtable and its callers.
// The remaining glow API is intentionally undeclared until its argument types
// are recovered; this declaration is used for construction and base teardown.
class soGlowModule : public soNullable {
public:
    virtual ~soGlowModule() { }
};

class soGlowModuleSimple : public soGlowModule {
    soModuleAccesser* m_moduleAccesser; // +0x8
    // HYPOTHESIS: original field spelling. The reference selects a parent link;
    // -1 uses the local attack value and glow flag instead of forwarding them.
    s32 m_refLinkNo;                   // +0xC
    s32 m_glowAttack;                  // +0x10: reset to 12
    bool m_isGlow;                     // +0x14: set by setGlowAttack
    u32 unk18;                        // preserved extent of the article module
public:
    soGlowModuleSimple(soModuleAccesser* acc);
    virtual ~soGlowModuleSimple();
};
static_assert(sizeof(soGlowModule) == 0x8, "Glow module base size");
static_assert(sizeof(soGlowModuleSimple) == 0x1C, "Simple glow module size");
