// Local shadow of BrawlHeaders' gm/gm_set_rule.h (see AGENTS.md section 6): the rule enum bit field is a plain byte bit
// field and three of the on/off settings are bool, which is how gmSetRuleToGlobalModeMelee reads them (it copies them
// into bit fields without a normalisation step). Layout is unchanged.
#pragma once

#include <StaticAssert.h>
#include <gm/gm_result_info.h>
#include <types.h>

struct gmSetRule {
    char _0[0x2];
    u8 _1 : 5;
    u8 m_rule : 3; // GameRule
    u8 m_timeMinutes;
    u8 m_stockCount;
    u8 m_handicap;
    u8 m_damageRatio; // Multiplied by 10
    u8 m_stageChoice;
    u8 m_stockTimeMinutes;
    bool m_isTeamAttack;
    bool m_allowPause;
    bool m_showScoreDisplay;
    u8 m_showDamageGauge;
    char _13[11];
    u8 m_spMeleeSetting1; // 0x18
    u8 m_spMeleeSetting2; // 0x19
    u8 m_spMeleeSetting3; // 0x1A
    u8 m_spMeleeSetting4; // 0x1B
    u8 m_spMeleeSetting5; // 0x1C
    u8 m_spMeleeSetting6; // 0x1D
    u8 m_spMeleeSetting7; // 0x1E
    u8 m_spMeleeSetting8; // 0x1F
    char _32[0x4];        // 0x20
    int m_ctrlNamePlayerNo;     // 0x24 HYPOTHESIS: the player number of the name whose controls are set (muProcOptBtn stores the
                                // muNameTask::m_playerNo of the name chosen before the Controls scene opens)
    u8 m_ctrlNameNo;            // 0x28 HYPOTHESIS: the name whose controls are set (muProcOptBtn stores the chosen name)
    char _29[0x7];
    char _30[0x58];       // 0x30
};
static_assert(sizeof(gmSetRule) == 0x88, "Class is wrong size!");
