#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <types.h>

// HYPOTHESIS: The moving-ground base of the ring lift (sora_melee gr_adventure_move_ground.o, not decompiled yet). Only the
// layout and the virtual the derived gimmicks override are declared here.
class grAdventureMoveGround : public grYakumono {
protected:
    float* unk150; // 0x150, HYPOTHESIS: the motion-path speed that the derived gimmicks set per difficulty level

public:
    virtual void createMoveGroundMotionPath();
};
static_assert(sizeof(grAdventureMoveGround) == 0x154, "Class is wrong size!");
