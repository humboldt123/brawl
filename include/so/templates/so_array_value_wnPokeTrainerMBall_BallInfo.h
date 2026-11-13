#pragma once

#include <StaticAssert.h>
#include <types.h>

class Vec3f;

// Local copy layout used only by abstract array instantiations; SDK unchanged.
// HYPOTHESIS: field grouping follows copies; field meanings remain unknown.
// The fighter unit defines the full article instead (ft/poke/ft_poke_trainer_article_pools.h).
#ifndef FT_POKETRAINER_ARTICLE_POOLS
class wnPokeTrainerMBall {
public:
    // HYPOTHESIS: activation parameter types follow the trainer's article callback.
    void activate(int founderTaskId, int parentTaskId, u32 mdlResId, int kind, bool changeRequested, Vec3f* pos);
class BallInfo {
public:
    struct { u32 unk0, unk4; } unk0;
    u32 unk8;
    struct { u32 unk0, unk4; } unkc;
    u32 unk14;
    struct { u32 unk0, unk4; } unk18;
    u32 unk20;
    struct { u32 unk0, unk4; } unk24;
    u32 unk2c;
    u32 unk30;
    u32 unk34;
    float unk38;
};
};
#endif
static_assert(sizeof(wnPokeTrainerMBall::BallInfo) == 60, "Class is wrong size!");
