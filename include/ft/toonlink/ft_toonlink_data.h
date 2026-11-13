#pragma once

#include <ft/ft_common_data_accesser.h>
#include <so/collision/so_collision_shield_group.h>
#include <types.h>

struct wnSimpleData;

// Toon Link's common data record: the shared ftData header followed by his own entries.
// HYPOTHESIS: +0x98 is the shield shape group of the Hylian Shield (ftToonLink::onCreate adds it to the shield module).
struct ftToonLinkData : ftData {
    u8 unk94[4];
    soCollisionShieldGroupData shieldGroup; // 0x98
    int m_boomerangArticleId; // 0xA4 HYPOTHESIS: name; the boomerang status removes the article with this id on exit
    u8 unkA8[0x18];
    const wnSimpleData* m_simpleArticle5Data; // 0xC0 HYPOTHESIS: name; data of the simple article with id 5
    wnSimpleData* m_hookshotTipData; // 0xC4 HYPOTHESIS: name; the hookshot activates its claw tip with this data
    const wnSimpleData* m_simpleArticle3Data; // 0xC8 HYPOTHESIS: name; data of the simple article with id 3
};
