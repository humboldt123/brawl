#pragma once

#include <mu/mu_object.h>
#include <mu/mu_proc_menu.h>
#include <types.h>

// What the screens of the main menu (sora_menu_main: muProc1p, muProcColl, ...) share on top of muProcMenu.

// HYPOTHESIS: the animation data that MuObject::m_unk18 points to; the screens only read its index (0x10).
struct MuAnimRefData {
    u8 unk0[0x10];
    u32 m_index;                // 0x10
};

// The number of the animation a menu object plays (-1: none): the data of m_unk18 wins over the one of setAnimIdx. The
// screens of the main menu read it to know which of their animations (come, wait, select ...) an icon is in.
// HYPOTHESIS: an inline helper of the screens (every screen expands it; MuObject itself declares nothing like it).
static inline u32 muGetAnimIdx(const MuObject* object) {
    if (object->m_unk18 != 0) {
        return reinterpret_cast<const MuAnimRefData*>(object->m_unk18)->m_index;
    }
    if (object->m_animIdxData != NULL) {
        return object->m_animIdxData->m_index;
    }
    return -1;
}

