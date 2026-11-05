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
#ifdef SO_GLOW_MODULE_ATTACK_API
    // Slots from the soGlowModuleSimple vtable (sora_melee lbl_27_data_17C58).
    virtual void clear();               // 0x10: refLinkNo = -1, glowAttack = 0, isGlow = false (HYPOTHESIS name)
    virtual void update();              // 0x14: empty in the simple module (HYPOTHESIS name)
    virtual s32 getGlowAttack();        // 0x18: returns the glow attack (HYPOTHESIS name)
    virtual void setGlowAttack(s32 attack); // 0x1C: stores the attack and sets the glow flag
    virtual bool isGlow();              // 0x20: the local glow flag, or the linked parent's (HYPOTHESIS name)
#endif
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
