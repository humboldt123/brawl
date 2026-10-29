#pragma once

#include <StaticAssert.h>
#include <sora_adv_stage/st_adventure_info_basis.h>
#include <types.h>

// The Subspace enemy-corps counter shown on the HUD.
class stAdventureInfoEmCorps : public stAdventureInfoBasis {
public:
    virtual ~stAdventureInfoEmCorps() { }
    virtual void update();

    void init();
    // HYPOTHESIS: parameter type from the register use (passed on to addZakoCorps).
    void setEmPopCount(u32 count);
    void notifyDead();
    void remove();
};
static_assert(sizeof(stAdventureInfoEmCorps) == 0x8, "Class is wrong size!");
