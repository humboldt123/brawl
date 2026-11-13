#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): it names the kind of
// the select screen that the character select screen reads.

#include <StaticAssert.h>
#include <gm/gm_global_mode_melee.h>
#include <types.h>

struct gmSelCharData {
    union {
        struct {
            char _0[0x4];
            u8 m_selCharType;   // 0x04 the kind of the select screen (versus, a solo mode, a minigame ...)
            union {
                char _5[0xb3];
                struct {
#ifdef MU_SELCHAR_UNIT_DECLS
                    u8 unk5;
                    u8 _pad5[0x28];
#else
                    u8 _pad5[0x29];
#endif
                    // HYPOTHESIS: 3-bit difficulty setting at offset 0x2E
                    u8 unk2E_b7 : 3;
                    // HYPOTHESIS: 3-bit stock setting at offset 0x2E.
                    u8 unk2E_b4 : 3;
                    u8 _pad2F[0x89];
                };
            };
        };
        struct {
            u8 _padA[0x6];
            u8 unk6; // HYPOTHESIS: a flag byte set by sqNetFriendMelee::start
            u8 _padB[0x22];
            u8 unk29_b5 : 3; // HYPOTHESIS: the rule byte 1 of the net melee rule
            u8 unk29_rest : 5;
            u8 _padC[0x9];
            u8 unk33; // HYPOTHESIS: the rule byte 0 of the net melee rule
            u8 _padD[0xC];
            u32 unk40; // HYPOTHESIS: the rule byte 2 of the net melee rule
            u32 unk44; // HYPOTHESIS: the rule byte 3 of the net melee rule
            u8 _padE[0x70];
        };
    };
    gmPlayerInitData m_playersInitData[MAX_PLAYERS];
    char _0x33c[4];
};
static_assert(sizeof(gmSelCharData) == 0x340, "Class is wrong size!");
