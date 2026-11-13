// Local shadow of BrawlHeaders' gm/gm_stage_edit_data.h.
// The stage browser reads one word at 0x24A0; scEditSelect::exit fills the record from a saved stage or clears it
// for a new one. HYPOTHESIS: the names follow those writers.
// Size stays 0x24e4 so other units that only allocate the record are unchanged.
#pragma once

#include <StaticAssert.h>
#include <types.h>

struct gmStageEditData {
    u8 m_unk00[0x14];
    u8 m_unk14;
    u8 m_unk15;
    u8 m_unk16[2];
    u16 m_unk18;
    u8 m_unk1A[0x2470 - 0x1A];
    u8 m_unk2470[0x2C];
    bool m_isNew;   // 0x249C HYPOTHESIS: set when the browser was left with mode 0
    u8 m_unk249D[3];
    int m_cursor; // 0x24A0 HYPOTHESIS: the collection cursor the stage browser restores
    char m_name[0x24e4 - 0x24A4]; // 0x24A4
};
static_assert(sizeof(gmStageEditData) == 0x24e4, "Class is wrong size!");
static_assert(offsetof(gmStageEditData, m_cursor) == 0x24A0, "cursor offset");
