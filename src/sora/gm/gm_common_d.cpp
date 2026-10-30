#include <types.h>
#include <string.h>
#include <gf/gf_scene.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>

// Callees owned by other clusters of this temporarily split unit.
// HYPOTHESIS: signatures come from the register use at the call sites; the names are from the sora map.
// gmResultInfo::gmSetSelStagePlayerModeResult(gmResultInfo*, gmGlobalModeMelee*)
extern "C" void fn_800555D8(gmResultInfo* result, gmGlobalModeMelee* melee);
// gmResultInfo::gmAddDiagramResultChara()
extern "C" void fn_80055058(gmResultInfo* result);
// gmResultInfo::gmAddDiagramResultName()
extern "C" void fn_800551C8(gmResultInfo* result);
// GameGlobal::getVsModeAutoHandiPlayerVal(int player, u8 ?): pointer to the player's auto handicap value
extern "C" u16* fn_8004DA14(GameGlobal* global, int player, u8 arg);
// muMenu::exchangeGmCharacterKindToMuCharKind: the mangled name says (int, int, int) but every caller passes one argument.
extern "C" int exchangeGmCharacterKindToMuCharKind__6muMenuFiii(int gmCharacterKind);

// Same records as gmPlayerResultInfo, seen from the start of gmResultInfo (the players start at 0x24): the compiler of
// the original addressed the players this way, with the fields it has no names for.
// MATCH-ONLY: a second view of m_playersResultInfo so that both loops address the players from the result pointer.
struct gmPlayerResultView {
    char _0[0x25];
    u8 m_state;         // 0x25
    char _26[3];
    u8 m_unk29;         // 0x29: third argument of the auto handicap lookup
    char _2A;
    char m_flags;       // 0x2B: bit 4 marks the players that take part in the coin payout (HYPOTHESIS)
    char _2C[7];
    u8 m_unk33;         // 0x33: HYPOTHESIS: zero for players that lost
    u32 m_koCount;      // 0x34
    char _38[4];
    u16 m_suicideCount; // 0x3C
    char _3E[0x1B6];
    u32 m_unk1F4;       // HYPOTHESIS: damage dealt, summed into the record's damage total
    char _1F8[0x58];
    int m_unk250;       // input of the coin rate: gmCountMeleeCoin(kind, m_unk250)
    char _254[0x58];
};
static_assert(sizeof(gmPlayerResultView) == sizeof(gmPlayerResultInfo), "Class is wrong size!");

// Result record fields that gmPlayerResultInfo has no names for.
static inline int& gmResultUnk22C(gmPlayerResultInfo* player) {
    return *reinterpret_cast<int*>(reinterpret_cast<u8*>(player) + 0x22C);
}
static inline u32 gmResultUnk1D0(gmPlayerResultInfo* player) {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(player) + 0x1D0);
}

// Record counters saturate: 99999999 for counts, 100000 hours in seconds for play times.
static const u32 kCountMax = 99999999;
static const u32 kTimeMax = 359999999;

void gmIncMeleeCount(u32 rule, u8 decisionKind, bool isStamina, bool isOnline) {
    u32* count = NULL;
    switch (decisionKind) {
    case 10:
    case 9:
        if (!isOnline) {
            count = g_GameGlobal->getGlobalRecordVsResetMeleeCountDatap();
        }
        break;
    default:
        if (isStamina) {
            count = g_GameGlobal->getGlobalRecordVsHpMeleeCountDatap();
        } else if (isOnline) {
            switch (rule) {
            case Game_Rule_Time:
            default:
                count = g_GameGlobal->getGlobalRecordOnlineVsTimeMeleeCountDatap();
                break;
            case Game_Rule_Stock:
                count = g_GameGlobal->getGlobalRecordOnlineVsStockMeleeCountDatap();
                break;
            case Game_Rule_Coin:
                count = g_GameGlobal->getGlobalRecordOnlineVsCoinMeleeCountDatap();
                break;
            }
        } else {
            switch (rule) {
            case Game_Rule_Time:
            default:
                count = g_GameGlobal->getGlobalRecordVsTimeMeleeCountDatap();
                break;
            case Game_Rule_Stock:
                count = g_GameGlobal->getGlobalRecordVsStockMeleeCountDatap();
                break;
            case Game_Rule_Coin:
                count = g_GameGlobal->getGlobalRecordVsCoinMeleeCountDatap();
                break;
            }
        }
        break;
    }
    if (count != NULL) {
        u32 value = *count;
        *count = value <= kCountMax - 1 ? value + 1 : kCountMax;
    }
}

void gmIncAnybodyWatchCount() {
    u32* count = g_GameGlobal->getGlobalRecordOnlineAnybodyWatchCountDatap();
    u32 value = *count;
    *count = value <= kCountMax - 1 ? value + 1 : kCountMax;
}

// HYPOTHESIS: the friend count is the byte after m_stagePlayCount[0x43] (0x32B); the header's m_stagePlayCount[0x44] spans it.
void gmSetFriendCount(int count) {
    if (count < 0) {
        count = 0;
    }
    if (count > 0x40) {
        count = 0x40;
    }
    *(reinterpret_cast<u8*>(g_GameGlobal->m_record) + 0x32B) = count;
}

u8 gmGetFriendCount() {
    return *(reinterpret_cast<u8*>(g_GameGlobal->m_record) + 0x32B);
}

// Adds to a record counter, saturating at max.
static inline void gmAddSat(u32* total, u32 add, u32 max) {
    bool overflow = false;
    if (add > max || *total > max - add) {
        overflow = true;
    }
    *total = overflow ? max : *total + add;
}
static inline void gmAddTime(u32* total, u32 time) {
    gmAddSat(total, time, kTimeMax);
}
// Same saturation with the opposite default (the all-players total is added from a product).
static inline void gmAddTimeProduct(u32* total, u32 time, u32 playerCount) {
    u32 add = time * playerCount;
    bool overflow = true;
    if (add <= kTimeMax && *total <= kTimeMax - add) {
        overflow = false;
    }
    *total = overflow ? kTimeMax : *total + add;
}
static inline void gmAddTotal(u32* total, u32 add) {
    gmAddSat(total, add, kCountMax);
}

// HYPOTHESIS: adds `time` seconds to the versus play time and `time * playerCount` to the per-player total.
void gmAddVsMeleeTimeAll(u32 time, u32 playerCount) {
    gmAddTime(g_GameGlobal->getGlobalRecordVsMeleeTimeCountDatap(), time);
    gmAddTimeProduct(g_GameGlobal->getGlobalRecordVsMeleeTimeTotalDatap(), time, playerCount);
}

void gmAddSingleTimeAll(u32 time, u32 playerCount) {
    gmAddTime(g_GameGlobal->getGlobalRecordSingleMeleeTimeCountDatap(), time);
    gmAddTimeProduct(g_GameGlobal->getGlobalRecordSingleMeleeTimeTotalDatap(), time, playerCount);
}

void gmAddOnlineFriendTimeCount(u32 time) {
    gmAddTime(g_GameGlobal->getGlobalRecordOnlineFriendTimeCountDatap(), time);
}

void gmAddOnlineAnybodyTimeCount(u32 time) {
    gmAddTime(g_GameGlobal->getGlobalRecordOnlineAnybodyTimeCountDatap(), time);
}

// MERGE NOTE: copy of the exact gmCountMeleeCoin of cluster B's range (gm_common_b.cpp), so that this unit inlines it
// into gmAddMeleeCoin as the original did; it disappears when the units are merged back into gm_common.cpp.
static u32 gmCountMeleeCoinInline(int kind, int playerCount) {
    // HYPOTHESIS: one coin rate per match kind.
    float rate[8] = { 1.0f, 0.8f, 0.8f, 0.6f, 1.0f, 0.9f, 0.8f, 0.7f };
    if (playerCount < 1) {
        playerCount = 1;
    }
    float r = rate[kind];
    return 10.0f * (r * (0.11f * playerCount));
}

// HYPOTHESIS: bit 4 of the byte at +0x7 marks the players that take part in the coin payout.
static inline bool gmResultPaysCoins(const char* flags) {
    return (*(const u8*)flags >> 4) & 1;
}

// Pays out the coins of the finished match: every human player (result state 0) with the flag at +0x7 bit 4 earns
// gmCountMeleeCoin coins, the rate depending on how many such players there are. The sum goes to the record's
// figure coin counter (capped at 99999) and to the total of collected coins (capped at u32 max).
void gmAddMeleeCoin() {
    GameGlobal* global = g_GameGlobal;
    gmResultInfo* result = global->m_resultInfo;
    gmPlayerResultView* view = reinterpret_cast<gmPlayerResultView*>(result);
    u32 coins = 0;
    int humans = 0;
    for (int i = 0; i < 4; i++) {
        if (view[i].m_state == 0 && gmResultPaysCoins(&result->m_playersResultInfo[i]._7[0])) {
            humans++;
        }
    }
    if (humans >= 1 && humans <= 4) {
        for (int i = 0; i < 4; i++) {
            if (view[i].m_state == 0 && gmResultPaysCoins(&result->m_playersResultInfo[i]._7[0])) {
                coins += gmCountMeleeCoinInline(humans + 3, view[i].m_unk250);
            }
        }
        u32* figCoin = g_GameGlobal->getGlobalRecordFigCoinDatap();
        u32* getFigCoin = g_GameGlobal->getGlobalRecordGetFigCoinDatap();
        u32 cur = *figCoin;
        *figCoin = (99999 - cur > coins) ? cur + coins : 99999;
        cur = *getFigCoin;
        *getFigCoin = (0xFFFFFFFF - cur > coins) ? cur + coins : 0xFFFFFFFF;
    }
}

// Auto handicap bookkeeping at the end of a match: results are only used when the set rule's byte at +0x5 is 1
// (HYPOTHESIS: auto handicap). Players whose flag byte at +0xF is zero get 20 more handicap (max 300), the others
// lose 20 (2 players) or 10 (3 or 4 players, except the lowest).
void gmSetAutoHandiModeResult() {
    gmResultInfo* result = g_GameGlobal->m_resultInfo;
    gmPlayerResultView* view = reinterpret_cast<gmPlayerResultView*>(result);
    if (*(reinterpret_cast<u8*>(g_GameGlobal->m_setRule) + 5) != 1) {
        return;
    }
    int players = 0;
    u16* handi[4] = { NULL, NULL, NULL, NULL };
    int order[4];
    int* orderEnd = order;
    for (int i = 0; i < 4; i++) {
        if (view[i].m_state != 3) {
            players++;
            handi[i] = fn_8004DA14(g_GameGlobal, i, view[i].m_unk29);
            if (view[i].m_unk33 != 0) {
                *orderEnd++ = i;
            }
        }
    }
    int last = -1;
    if (players == 4) {
        if (*handi[order[0]] < *handi[order[1]]) {
            int t = order[0];
            order[0] = order[1];
            order[1] = t;
        }
        if (*handi[order[0]] < *handi[order[2]]) {
            int t = order[0];
            order[0] = order[2];
            order[2] = t;
        }
        if (*handi[order[1]] < *handi[order[2]]) {
            int t = order[1];
            order[1] = order[2];
            order[2] = t;
        }
        if (*handi[order[0]] == *handi[order[1]] && view[order[0]].m_unk33 < view[order[1]].m_unk33) {
            int t = order[0];
            order[0] = order[1];
            order[1] = t;
        }
        if (*handi[order[0]] == *handi[order[2]] && view[order[0]].m_unk33 < view[order[2]].m_unk33) {
            int t = order[0];
            order[0] = order[2];
            order[2] = t;
        }
        if (*handi[order[1]] == *handi[order[2]] && view[order[1]].m_unk33 < view[order[2]].m_unk33) {
            int t = order[1];
            order[1] = order[2];
            order[2] = t;
        }
        last = order[2];
    }
    for (int i = 0; i < 4; i++) {
        u16* value;
        if (view[i].m_state != 3 && (value = handi[i]) != NULL) {
            u16 v = *value;
            if (view[i].m_unk33 == 0) {
                v += 20;
                if (v > 300) {
                    v = 300;
                }
            } else {
                switch (players) {
                case 2:
                    if (v >= 20) {
                        v -= 20;
                    } else {
                        v = 0;
                    }
                    break;
                case 3:
                case 4:
                    if (i != last) {
                        v = v >= 10 ? (u16)(v - 10) : 0;
                    }
                    break;
                }
            }
            *value = v;
        }
    }
}

// gmGlobalModeMelee seen from its start: the rule is stored in the top three bits of the byte at +0x9 (unsigned).
struct gmMeleeRuleView {
    char _0[9];
    u8 m_gameRule : 3;
};

// strcmp of a name against the name of the scene manager's current sequence.
static inline int gmCmpCurrentSequence(const char* name) {
    return strcmp(name, gfSceneManager::getInstance()->m_currentSequence->m_sequenceName);
}

// HYPOTHESIS: u16 totals with a halfword of padding each, starting at +0xC of the character selection data.
struct gmSelCharTotalView {
    char _0[0xC];
    struct {
        u16 m_total;
        u16 _2;
    } m_totals[4];
};

// HYPOTHESIS: the first parameter selects the stamina (HP) counter in gmIncMeleeCount, the second marks online matches.
// Books the finished match into the global record: coins, match counters per rule, character and stage play counts,
// play time and the kill/suicide/damage totals.
void gmMeleeDest(bool isStamina, bool isOnline) {
    gmResultInfo* result = g_GameGlobal->m_resultInfo;
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    gmPlayerResultView* view = reinterpret_cast<gmPlayerResultView*>(result);
    u8 decisionKind = result->m_decisionKind;

    if (decisionKind != 9 && decisionKind != 10) {
        gmSetAutoHandiModeResult();
        if (!isOnline) {
            fn_800555D8(result, melee);
        }
    }

    bool anyHuman = false;
    for (int i = 0; i < 4; i++) {
        if (melee->m_playersInitData[i].m_state == 0) {
            anyHuman = true;
        }
    }

    if (anyHuman) {
        gmAddMeleeCoin();
        if (!isOnline) {
            if (decisionKind != 9 && decisionKind != 10) {
                // MATCH-ONLY: the players are addressed from the start of the melee data.
                u8* player = reinterpret_cast<u8*>(melee);
                for (int i = 0; i < 4; i++, player += sizeof(gmPlayerInitData)) {
                    if (player[0x99] == 0 && decisionKind != 9 && decisionKind != 10) {
                        int muKind = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(player[0x98]);
                        gmGlobalRecord::CharaNumData* chara = g_GameGlobal->getGlobalRecordCharaNumDatap(muKind);
                        if (chara->unk98 < 0xFFFF) {
                            chara->unk98++;
                        }
                    }
                }
                // HYPOTHESIS: the u16 at +0xC of the result is the stage kind that was played.
                u8* stagePlayCount = g_GameGlobal->getGlobalRecordGetStagePlayCountDatap(*reinterpret_cast<u16*>(reinterpret_cast<u8*>(result) + 0xC));
                *stagePlayCount = *stagePlayCount <= 0xFE ? *stagePlayCount + 1 : 0xFF;
                if (gmCmpCurrentSequence("sqVsMelee") == 0) {
                    fn_80055058(result);
                    fn_800551C8(result);
                }
            }

            u32* partyPlayerCount = g_GameGlobal->getGlobalRecordVsPartyPlayerCountDatap();
            u32 humans = 0;
            for (int i = 0; i < 4; i++) {
                if (view[i].m_state == 0) {
                    humans++;
                }
            }
            gmAddTotal(partyPlayerCount, humans);
            u32 seconds = (u32)result->m_framesElapsed / 60;
            gmAddVsMeleeTimeAll(seconds, humans);
        }
        gmIncMeleeCount(reinterpret_cast<gmMeleeRuleView*>(melee)->m_gameRule, decisionKind, isStamina, isOnline);
    }

    if (gmCmpCurrentSequence("sqToMelee") != 0 &&
        gmCmpCurrentSequence("sqQuMelee") != 0) {
        // HYPOTHESIS: per-player accumulated KO counts (u16, stride 4 from +0xC) of the character selection data.
        GameGlobal* global = g_GameGlobal;
        gmResultInfo* res = global->m_resultInfo;
        gmPlayerResultView* resView = reinterpret_cast<gmPlayerResultView*>(res);
        gmSelCharTotalView* sel = reinterpret_cast<gmSelCharTotalView*>(global->m_selCharData);
        for (int i = 0; i < 4; i++) {
            if (resView[i].m_state == 0) {
                u16* total = &sel->m_totals[i].m_total;
                int add = (u16)resView[i].m_koCount;
                bool overflow = true;
                if (*total <= 0xFFFF - add) {
                    overflow = false;
                }
                *total = overflow ? 0xFFFF : (u16)(*total + add);
            }
        }
    }

    if (decisionKind != 9 && decisionKind != 10 && gmCmpCurrentSequence("sqVsMelee") == 0) {
        gmResultInfo* res = g_GameGlobal->m_resultInfo;
        gmPlayerResultView* resView = reinterpret_cast<gmPlayerResultView*>(res);
        for (int i = 0; i < 4; i++) {
            if (resView[i].m_state == 0) {
                u32* kills = g_GameGlobal->getGlobalRecordKillTotalCountDatap();
                u32* suicides = g_GameGlobal->getGlobalRecordSuicideTotalCountDatap();
                u32* damage = g_GameGlobal->getGlobalRecordDamageTotalCountDatap();
                gmAddTotal(kills, resView[i].m_koCount);
                gmAddTotal(suicides, resView[i].m_suicideCount);
                gmAddTotal(damage, resView[i].m_unk1F4);
            }
        }
    }
}
