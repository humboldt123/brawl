#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): it names the kind of
// the select screen that the character select screen reads.

#include <StaticAssert.h>
#include <gm/gm_global_mode_melee.h>
#include <types.h>

struct gmSelCharData {
    char _0[0x4];
    u8 m_selCharType;   // 0x04 the kind of the select screen (versus, a solo mode, a minigame ...)
    char _5[0xb3];
    gmPlayerInitData m_playersInitData[MAX_PLAYERS];
    char _0x33c[4];
};
static_assert(sizeof(gmSelCharData) == 0x340, "Class is wrong size!");
