#pragma once

#include <ft/ft_common_data_accesser.h>
#include <so/collision/so_collision_shield_group.h>
#include <types.h>

struct wnSimpleData;

// Link's common data record: the shared ftData header followed by his own entries.
// HYPOTHESIS: +0x98 is the shield shape group of the Hylian Shield (ftLink::onCreate adds it to the shield module).
struct ftLinkData : ftData {
    u8 unk94[4];
    soCollisionShieldGroupData shieldGroup; // 0x98
    u8 unkA4[0x1C];
    const wnSimpleData* unkC0;
    wnSimpleData* m_hookshotTipData; // 0xC4 HYPOTHESIS: name; the hookshot activates its claw tip with this data
};
