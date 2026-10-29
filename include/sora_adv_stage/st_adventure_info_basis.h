#pragma once

#include <StaticAssert.h>
#include <sora_adv_stage/st_adventure_info_contact.h>
#include <types.h>

class Enemy;

// The enemy lookups shared by the boss, corps and zako info records.
class stAdventureInfoBasis : public stAdventureInfoContact {
public:
    // HYPOTHESIS: parameter types from the register use and the callers.
    stAdventureInfoIconParam getIconParamForEnemy(int createId, u32 taskId, int index, stAdventureInfoIconParam* prev);
    Enemy* getEnemy(int createId);
};
static_assert(sizeof(stAdventureInfoBasis) == 0x8, "Class is wrong size!");
