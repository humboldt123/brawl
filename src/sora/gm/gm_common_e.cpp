#include <types.h>
#include <gf/gf_file_io_handle.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <mt/mt_prng.h>
#include <string.h>

// Cluster E of the temporary gm_common split (0x80053F4C-0x800568C8): last-place and diagram bookkeeping of the
// melee result, the stage choice rule, event clear queries, the item switch tables and the figure and sticker lotteries.

// Saturating add as the game inlines it (the out-of-line copy is fn_8004F258 in the previous unit): the sum is
// clamped to max, and an addend above max clamps too.
static inline void addMaxCheck(u32* value, u32 add, u32 max) {
    bool overflow = add > max || *value > max - add;
    *value = overflow ? max : *value + add;
}

static inline void addMaxCheck(u16* value, u16 add, u16 max) {
    bool overflow = add > max || *value > max - add;
    *value = overflow ? max : *value + add;
}

static inline void addMaxCheck(u8* value, u32 add, u32 max) {
    u8 current = *value;
    bool overflow = add > max || current > max - add;
    *value = overflow ? max : current + add;
}

// HYPOTHESIS: the part of gmPlayerResultInfo past the named members is not decoded yet; these read it by offset.
template <class T>
static inline T playerStat(const gmPlayerResultInfo* player, int offset) {
    return *reinterpret_cast<const T*>(reinterpret_cast<const u8*>(player) + offset);
}

// HYPOTHESIS: team results of a team battle: one 0x30-byte record per team (red, blue, green) from 0x12D8 of
// gmResultInfo, the part read here sits at the end of the record.
struct gmTeamResultInfo {
    char _0[0x2C];
    u8 m_rank;      // 0x2C
    u8 m_unk2D;     // 0x2D: zero for the winning team
    u8 m_isPlaying; // 0x2E: nonzero if the team has players
    char _2F[1];
};
static_assert(sizeof(gmTeamResultInfo) == 0x30, "Class is wrong size!");

static inline gmTeamResultInfo* gmTeamResults(gmResultInfo* result) {
    return reinterpret_cast<gmTeamResultInfo*>(reinterpret_cast<u8*>(result) + 0x12D8);
}

static inline u8 teamRank(gmResultInfo* result, int team) {
    return reinterpret_cast<u8*>(result)[team * sizeof(gmTeamResultInfo) + 0x1304];
}

// HYPOTHESIS: "ketsu" (slang for the bottom) is last place; this is the worst rank number among the players or
// teams that took part (the diagram pages call the matches finished last "ketsu num").
int gmRetKetsuRankFromResult(gmResultInfo* result) {
    int rank = 0;
    if (result->m_isTeams == 1) {
        gmTeamResultInfo* teams = gmTeamResults(result);
        for (int i = 0; i < 3; i++) {
            if (teams[i].m_isPlaying != 0 && teams[i].m_rank > rank) {
                rank = teams[i].m_rank;
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            gmPlayerResultInfo* player = &result->m_playersResultInfo[i];
            if (player->m_state != 3 && player->m_rank > rank) {
                rank = player->m_rank;
            }
        }
    }
    return rank;
}

// The last-place player (4 if there is none). Players tied in last place are told apart by fewest KOs, most
// falls, then two more tallies of the result record; if they are still tied the lowest port wins.
int gmRetKetsuPlayerFromResult(gmResultInfo* result) {
    int rank = gmRetKetsuRankFromResult(result);
    gmPlayerResultInfo* players = result->m_playersResultInfo;
    int first = 4;
    bool tied = false;
    bool candidate[4];
    if (result->m_isTeams == 1) {
        for (int i = 0; i < 4; i++) {
            gmPlayerResultInfo* player = &players[i];
            if (player->m_state != 3 && teamRank(result, player->m_teamNo) == rank) {
                tied = true;
                first = i;
                candidate[i] = true;
            } else {
                candidate[i] = false;
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            gmPlayerResultInfo* player = &players[i];
            if (player->m_state != 3 && player->m_rank == rank) {
                if (first != 4) {
                    tied = true;
                }
                if (first == 4) {
                    first = i;
                }
                candidate[i] = true;
            } else {
                candidate[i] = false;
            }
        }
    }
    if (!tied) {
        return first;
    }

    // fewest KOs
    int best = 4;
    gmPlayerResultInfo* bestPlayer;
    for (int i = 0; i < 4; i++) {
        if (candidate[i]) {
            if (best == 4) {
                best = i;
            } else if (result->m_playersResultInfo[best].m_koCount > players[i].m_koCount) {
                best = i;
            }
            bestPlayer = &result->m_playersResultInfo[best];
        }
    }
    int same = 0;
    for (int i = 0; i < 4; i++) {
        if (candidate[i] && best != i) {
            if (players[i].m_koCount == bestPlayer->m_koCount) {
                same++;
            } else {
                candidate[i] = false;
            }
        }
    }
    if (same == 0) {
        return best;
    }

    // most falls
    best = 4;
    for (int i = 0; i < 4; i++) {
        if (candidate[i]) {
            if (best == 4) {
                best = i;
            } else if (result->m_playersResultInfo[best].m_deadCount < players[i].m_deadCount) {
                best = i;
            }
            bestPlayer = &result->m_playersResultInfo[best];
        }
    }
    same = 0;
    for (int i = 0; i < 4; i++) {
        if (candidate[i] && best != i) {
            if (players[i].m_deadCount == bestPlayer->m_deadCount) {
                same++;
            } else {
                candidate[i] = false;
            }
        }
    }
    if (same == 0) {
        return best;
    }

    // HYPOTHESIS: the third tally (fewest) at 0x1D0 of the player record
    best = 4;
    for (int i = 0; i < 4; i++) {
        if (candidate[i]) {
            if (best == 4) {
                best = i;
            } else if (playerStat<u32>(&result->m_playersResultInfo[best], 0x1D0) >
                       playerStat<u32>(&players[i], 0x1D0)) {
                best = i;
            }
            bestPlayer = &result->m_playersResultInfo[best];
        }
    }
    same = 0;
    for (int i = 0; i < 4; i++) {
        if (candidate[i] && best != i) {
            if (playerStat<u32>(&players[i], 0x1D0) == playerStat<u32>(bestPlayer, 0x1D0)) {
                same++;
            } else {
                candidate[i] = false;
            }
        }
    }
    if (same == 0) {
        return best;
    }

    // HYPOTHESIS: the fourth tally (most) at 0x28C of the player record
    best = 4;
    for (int i = 0; i < 4; i++) {
        if (candidate[i]) {
            if (best == 4) {
                best = i;
            } else if (playerStat<u32>(&result->m_playersResultInfo[best], 0x28C) <
                       playerStat<u32>(&players[i], 0x28C)) {
                best = i;
            }
            bestPlayer = &result->m_playersResultInfo[best];
        }
    }
    same = 0;
    for (int i = 0; i < 4; i++) {
        if (candidate[i] && best != i) {
            if (playerStat<u32>(&players[i], 0x28C) == playerStat<u32>(bestPlayer, 0x28C)) {
                same++;
            } else {
                candidate[i] = false;
            }
        }
    }
    if (same == 0) {
        return best;
    }

    if (candidate[0]) {
        return 0;
    }
    if (candidate[1]) {
        return 1;
    }
    if (candidate[2]) {
        return 2;
    }
    if (candidate[3]) {
        return 3;
    }
    return 0;
}

// HYPOTHESIS: the 0x50-byte block of lifetime battle statistics that the result adds into, one per character
// (gmGlobalRecord::CharaNumData at 0x48) and one per name tag (NameData at 0x4). The field order follows the
// diagram page getters of the menu (muDiagGet...), the players' source tallies are read by offset below.
struct gmDiagramResult {
    u16 m_suicide;       // 0x00
    char _2[2];
    u32 m_totalBeat;     // 0x04
    u32 m_totalFall;     // 0x08
    u32 m_giveDamage;    // 0x0C
    u32 m_takeDamage;    // 0x10
    u32 m_recoverDamage; // 0x14
    u16 m_maxDamage;     // 0x18
    u16 m_roundNum;      // 0x1A: matches played
    u16 m_champNum;      // 0x1C: matches won
    u16 m_ketsuNum;      // 0x1E: matches finished last
    u32 m_playTime;      // 0x20: seconds
    u32 m_joinTotal;     // 0x24: summed number of players in the matches
    u32 m_walkDistance;  // 0x28
    u32 m_jumpDistance;  // 0x2C
    u32 m_fallDistance;  // 0x30
    u32 m_blowDistance;  // 0x34
    u32 m_getCoin;       // 0x38
    u32 m_makeDropCoin;  // 0x3C
    u32 m_dropCoin;      // 0x40
    u32 m_fallButtocks;  // 0x44
    u32 m_swimDistance;  // 0x48
    u32 m_swimFall;      // 0x4C
};
static_assert(sizeof(gmDiagramResult) == 0x50, "Class is wrong size!");

// Adds the result of one player (index) to its diagram block.
void gmAddDiagramResultCommon(gmResultInfo* result, int index, gmDiagramResult* diagram) {
    gmPlayerResultInfo* player = &result->m_playersResultInfo[index];

    addMaxCheck(&diagram->m_suicide, player->m_suicideCount, 0xFFFF);
    addMaxCheck(&diagram->m_totalBeat, playerStat<u32>(player, 0x284), 0xFFFFFFFF);
    addMaxCheck(&diagram->m_totalFall, playerStat<u32>(player, 0x288), 0xFFFFFFFF);
    addMaxCheck(&diagram->m_giveDamage, playerStat<u32>(player, 0x1CC), 99999999);
    addMaxCheck(&diagram->m_takeDamage, playerStat<u32>(player, 0x1D0), 99999999);
    addMaxCheck(&diagram->m_recoverDamage, playerStat<u32>(player, 0x1D4), 99999999);

    u32 maxDamage = playerStat<u32>(player, 0x1D8);
    if (diagram->m_maxDamage < maxDamage) {
        diagram->m_maxDamage = maxDamage;
        if (diagram->m_maxDamage > 999) {
            diagram->m_maxDamage = 999;
        }
    }

    if (diagram->m_roundNum < 0xFFFF) {
        addMaxCheck(&diagram->m_roundNum, 1, 0xFFFF);
        u16 joined = 0;
        if (result->m_playersResultInfo[0].m_state == 0) {
            joined += 1;
        }
        if (result->m_playersResultInfo[1].m_state == 0) {
            joined += 1;
        }
        if (result->m_playersResultInfo[2].m_state == 0) {
            joined += 1;
        }
        if (result->m_playersResultInfo[3].m_state == 0) {
            joined += 1;
        }
        addMaxCheck(&diagram->m_joinTotal, joined, 0xFFFF);
    }

    // The winner of the match: the last team (or player) that is flagged as not defeated.
    if (result->m_isTeams == 1) {
        gmTeamResultInfo* teams = gmTeamResults(result);
        int winner = 0;
        if (teams[0].m_isPlaying != 0 && teams[0].m_unk2D == 0) {
            winner = 0;
        }
        if (teams[1].m_isPlaying != 0 && teams[1].m_unk2D == 0) {
            winner = 1;
        }
        if (teams[2].m_isPlaying != 0 && teams[2].m_unk2D == 0) {
            winner = 2;
        }
        if (player->m_teamNo == winner) {
            addMaxCheck(&diagram->m_champNum, 1, 0xFFFF);
        }
    } else {
        int winner = 0;
        if (result->m_playersResultInfo[0].m_state != 3 && playerStat<u8>(&result->m_playersResultInfo[0], 0xF) == 0) {
            winner = 0;
        }
        if (result->m_playersResultInfo[1].m_state != 3 && playerStat<u8>(&result->m_playersResultInfo[1], 0xF) == 0) {
            winner = 1;
        }
        if (result->m_playersResultInfo[2].m_state != 3 && playerStat<u8>(&result->m_playersResultInfo[2], 0xF) == 0) {
            winner = 2;
        }
        if (result->m_playersResultInfo[3].m_state != 3 && playerStat<u8>(&result->m_playersResultInfo[3], 0xF) == 0) {
            winner = 3;
        }
        if (index == winner) {
            addMaxCheck(&diagram->m_champNum, 1, 0xFFFF);
        }
    }

    if (gmRetKetsuPlayerFromResult(result) == index) {
        addMaxCheck(&diagram->m_ketsuNum, 1, 0xFFFF);
    }

    addMaxCheck(&diagram->m_playTime, result->m_framesElapsed / 60, 359999999);
    addMaxCheck(&diagram->m_walkDistance, playerStat<u32>(player, 0x28C), 99999999);
    addMaxCheck(&diagram->m_jumpDistance, playerStat<u32>(player, 0x290), 99999999);
    addMaxCheck(&diagram->m_fallDistance, playerStat<u32>(player, 0x294), 99999999);
    addMaxCheck(&diagram->m_blowDistance, playerStat<u32>(player, 0x1DC), 99999999);
    addMaxCheck(&diagram->m_swimDistance, playerStat<u32>(player, 0x298), 99999999);
    addMaxCheck(&diagram->m_swimFall, playerStat<u32>(player, 0x29C), 99999999);
    addMaxCheck(&diagram->m_fallButtocks, playerStat<u32>(player, 0x2A0), 99999999);

    if (result->m_gameRule == Game_Rule_Coin) {
        addMaxCheck(&diagram->m_getCoin, player->m_pickupCoins, 99999999);
        addMaxCheck(&diagram->m_makeDropCoin, playerStat<u32>(player, 0x228), 99999999);
        addMaxCheck(&diagram->m_dropCoin, player->m_lostCoins, 99999999);
    }
}

// HYPOTHESIS: stand-in with the real symbol; the game passes the character kind alone (the other two arguments
// of the declaration in mu_menu.cpp are not set by this caller).
extern "C" int exchangeGmCharacterKindToMuCharKind__6muMenuFiii(int kind);

// Adds the match to the per-character statistics: who beat whom (by character), how often each character was
// beaten, and the diagram block of the character.
void gmAddDiagramResultChara(gmResultInfo* result) {
    for (int i = 0; i < 4; i++) {
        gmPlayerResultInfo* player = &result->m_playersResultInfo[i];
        if (player->m_state != 0) {
            continue;
        }
        gmGlobalRecord::CharaNumData* chara = g_GameGlobal->getGlobalRecordCharaNumDatap(
            (u8)exchangeGmCharacterKindToMuCharKind__6muMenuFiii(playerStat<u8>(player, 0)));
        for (int j = 0; j < 4; j++) {
            gmPlayerResultInfo* other = &result->m_playersResultInfo[j];
            if (i == j || other->m_state == 3) {
                continue;
            }
            if (player->m_state == 1 && other->m_state == 1) {
                continue;
            }
            if (result->m_isTeams != 1 || player->m_teamNo != other->m_teamNo) {
                u16* beaten = reinterpret_cast<u16*>(chara->unk0);
                addMaxCheck(&beaten[(u8)exchangeGmCharacterKindToMuCharKind__6muMenuFiii(playerStat<u8>(other, 0))],
                            player->m_playerBeatCounts[j], 0xFFFF);
            }
            addMaxCheck(&chara->unk46, other->m_playerBeatCounts[i], 0xFFFF);
        }
        gmAddDiagramResultCommon(result, i, reinterpret_cast<gmDiagramResult*>(chara->unk48));
    }
}

// Same for the name tags: the NameData u16 pair at 0x0 and 0x2 are the beat counts, the block at 0x4 is the
// diagram, and the table at 0x54 holds the seconds played per character.
void gmAddDiagramResultName(gmResultInfo* result) {
    for (int i = 0; i < 4; i++) {
        gmPlayerResultInfo* player = &result->m_playersResultInfo[i];
        if (player->m_state != 0) {
            continue;
        }
        // HYPOTHESIS: byte 5 of the player record is the name tag, 0x78 means none.
        u8 nameTag = playerStat<u8>(player, 5);
        if (nameTag == 0x78) {
            continue;
        }
        gmGlobalRecord::NameData* name = g_GameGlobal->getGlobalRecordNameDatap(nameTag);
        for (int j = 0; j < 4; j++) {
            gmPlayerResultInfo* other = &result->m_playersResultInfo[j];
            if (i == j || other->m_state == 3) {
                continue;
            }
            if (player->m_state == 1 && other->m_state == 1) {
                continue;
            }
            if (result->m_isTeams != 1 || player->m_teamNo != other->m_teamNo) {
                addMaxCheck(&name->unk0, player->m_playerBeatCounts[j], 0xFFFF);
            }
            addMaxCheck(&name->unk2, other->m_playerBeatCounts[i], 0xFFFF);
        }
        u32 seconds = result->m_framesElapsed / 60;
        u8 charaIndex = exchangeGmCharacterKindToMuCharKind__6muMenuFiii(playerStat<u8>(player, 0));
        addMaxCheck(&name->unk54[charaIndex], seconds, 359999999);
        gmAddDiagramResultCommon(result, i, reinterpret_cast<gmDiagramResult*>(name->unk4));
    }
}


// Whether the clear getter of this index is open (state 2). HYPOTHESIS: the record keeps one state byte per getter at 0x1528.
bool isClearGetterOpen(int index) {
    return (reinterpret_cast<u8*>(g_GameGlobal->m_record) + index)[0x1528] == 2;
}

// Marks the clear getter as open in the clear-get app flags.
void setClearGetterOpen(int index) {
    u32* flags = reinterpret_cast<u32*>(g_GameGlobal->getClearGetAppFlag());
    flags[index / 32] |= 1 << (index % 32);
}

// HYPOTHESIS: the first bytes of gmGlobalModeMelee track the stage choice: the player who picks the next stage
// (-1 for none), and the loser of the last match.
struct gmStageChooseWork {
    s8 m_chooser; // 0x0
    s8 m_unk1;    // 0x1
    s8 m_loser;   // 0x2
};

// Applies the stage selection rule: sets who chooses the next stage and returns how the stage is picked.
// HYPOTHESIS: 0 = the chosen player picks, 1 and 2 = random modes without a chooser.
int gmSetRuleSelStage() {
    GameGlobal* global = g_GameGlobal;
    gmSetRule* rule = global->m_setRule;
    gmStageChooseWork* work = reinterpret_cast<gmStageChooseWork*>(global->m_modeMelee);
    gmSelCharData* sel = global->m_selCharData;

    switch (rule->m_stageChoice) {
    case 0:
    default:
        work->m_chooser = -1;
        return 0;
    case 1:
        work->m_chooser = -1;
        return 1;
    case 2: {
        // turns: the next player in port order that is a human
        s8 current = work->m_chooser;
        int start = (current == -1) ? 0 : current + 1;
        int next = start;
        bool found = false;
        for (int i = 0; i < 4; i++) {
            int index = (start + i) % 4;
            if (sel->m_playersInitData[index].m_state == 0) {
                next = index;
                found = true;
                break;
            }
        }
        if (!found) {
            next = -1;
        }
        work->m_chooser = next;
        return 0;
    }
    case 3:
        work->m_chooser = -1;
        return 2;
    case 4: {
        // loser: the loser of the last match, if that is still a human
        s8 loser = work->m_loser;
        int next = -1;
        if (loser != -1 && sel->m_playersInitData[loser].m_state == 0) {
            next = loser;
        }
        work->m_chooser = next;
        return 0;
    }
    }
}

// Picks the loser of the match: one of the players tied for the worst rank, at random, or -1.
void gmSetSelStagePlayerModeResult(gmResultInfo* result, gmGlobalModeMelee* melee) {
    int worst = -1;
    char candidates[4];
    memset(candidates, 0, sizeof(candidates));
    for (int i = 0; i < 4; i++) {
        gmPlayerResultInfo* player = &result->m_playersResultInfo[i];
        if (player->m_state == 0 && player->m_rank > worst) {
            worst = player->m_rank;
        }
    }
    int count = 0;
    char* out = candidates;
    for (int i = 0; i < 4; i++) {
        gmPlayerResultInfo* player = &result->m_playersResultInfo[i];
        if (player->m_state == 0 && worst == player->m_rank) {
            *out = i;
            count++;
            out++;
        }
    }
    gmStageChooseWork* work = reinterpret_cast<gmStageChooseWork*>(melee);
    if (count != 0) {
        work->m_loser = candidates[randi(count)];
    } else {
        work->m_loser = -1;
    }
}

extern "C" void fn_80055760(u32 arg0, s32 arg1) {
    if (arg0 == 5) {
        s32 v = 0;
        switch (arg1) {
        case 1:
            v = 3;
            break;
        case 2:
            v = 2;
            break;
        }
        if (v != 0) {
            g_GameGlobal->m_stageData->m_exSetting = (short)v;
        }
    }
}

// HYPOTHESIS: byte 0x28 of the app data holds flags; bit 0x10 marks the autosave as needed.
struct gmAppFlags {
    char _0[0x28];
    u8 m_unk28_7 : 1;
    u8 m_unk28_6 : 1;
    u8 m_unk28_5 : 1;
    u8 m_autosaveDirty : 1;
    u8 m_unk28_3 : 1;
    u8 m_unk28_2 : 1;
    u8 m_unk28_1 : 1;
    u8 m_unk28_0 : 1;
};

void gmSetAutosaveDirtyFlag() {
    reinterpret_cast<gmAppFlags*>(g_GameGlobal->m_appData)->m_autosaveDirty = 1;
}

void gmResetAutosaveDirtyFlag() {
    reinterpret_cast<gmAppFlags*>(g_GameGlobal->m_appData)->m_autosaveDirty = 0;
}

bool gmIsAutosaveDirtyFlag() {
    return reinterpret_cast<gmAppFlags*>(g_GameGlobal->m_appData)->m_autosaveDirty;
}

// HYPOTHESIS: the event high scores of one mode as a row of 51 events for each of the three difficulties; the top
// two bits of a score word are the clear state of the event.
struct gmEventScoreRow {
    u32 m_events[51];
};

struct gmEventScoreTable {
    gmEventScoreRow m_rows[3];
};

// Whether an event was cleared on the given difficulty or a harder one (type 1 = solo, 2 = co-op).
bool gmCheckEventClearFlag(int type, int index, int difficulty) {
    for (int d = difficulty; d < 3; d++) {
        gmGlobalRecord* record = g_GameGlobal->getGlobalRecordGameDatap();
        gmEventScoreTable* tables = reinterpret_cast<gmEventScoreTable*>(record->m_eventHiScoreData);
        gmEventScoreTable* table = &tables[type - 1];
        gmEventScoreRow* row = &table->m_rows[d];
        if ((row->m_events[index] >> 30) != 0) {
            return true;
        }
    }
    return false;
}

// How many of the 51 events are cleared on the given difficulty or a harder one.
int gmCheckEventClearCount(int type, int difficulty) {
    int count = 0;
    for (int i = 0; i < 51; i++) {
        if (gmCheckEventClearFlag(type, i, difficulty) == true) {
            count++;
        }
    }
    return count;
}

// Adds to the stage editor play count of the record, capped at 255.
void gmAddStageEditCount(u32 count) {
    gmGlobalRecord* record = g_GameGlobal->getGlobalRecordGameDatap();
    addMaxCheck(&record->m_stagePlayCount[0x42], count, 0xFF);
}

// HYPOTHESIS: the item switch record (gmItSwitch) as the four words the game copies it by: items, items,
// Pokemon, assist trophies. The first bit of a gmItSwitch field list is the most significant bit of its word.
struct gmItSwitchWords {
    u32 m_item[2];
    u32 m_pokemon;
    u32 m_assist;
};

// Bits of gmItSwitch in the words above: m_item.m_teamHealer, then the assist trophies m_assist.m_rayMKII,
// m_isaac, m_infantryTank, m_barbara, m_grayFox and m_shadow.
enum {
    ITEM_TEAM_HEALER = 0x00004000,
    ASSIST_RAY_MKII = 0x00000008,
    ASSIST_ISAAC = 0x00080000,
    ASSIST_INFANTRY_TANK = 0x00800000,
    ASSIST_BARBARA = 0x00000002,
    ASSIST_GRAY_FOX = 0x00000004,
    ASSIST_SHADOW = 0x00200000,
};

// The item switch settings handed out below live in one shared buffer (lbl_804977E0 of the data unit).
extern gmItSwitchWords lbl_804977E0;

// HYPOTHESIS: each table holds one default item switch (16 bytes) per stage / event / challenge.
extern const gmItSwitchWords lbl_8042C4B8[43];
extern const gmItSwitchWords lbl_8042C768[12];
extern const gmItSwitchWords lbl_8042C828[19];
extern const gmItSwitchWords lbl_8042C958[51];
extern const gmItSwitchWords lbl_8042CC88[51];
extern const gmItSwitchWords lbl_8042CFB8[14];
extern const gmItSwitchWords lbl_8042D098[1];

// HYPOTHESIS: stand-in for gmCheckAssistKindUseEnable (gmlib), true once the assist trophy is unlocked.
extern "C" bool fn_8004F3BC(int kind);

// Takes the unlocked state into account: the team healer needs a team battle, and each assist trophy needs its unlock.
static inline void gmRestrictItemSwitch(gmItSwitchWords* data) {
    if (g_GameGlobal->m_modeMelee->m_meleeInitData.m_isTeams != 1) {
        data->m_item[0] &= ~ITEM_TEAM_HEALER;
    }
    if (!fn_8004F3BC(0)) {
        data->m_assist &= ~ASSIST_RAY_MKII;
    }
    if (!fn_8004F3BC(1)) {
        data->m_assist &= ~ASSIST_ISAAC;
    }
    if (!fn_8004F3BC(2)) {
        data->m_assist &= ~ASSIST_INFANTRY_TANK;
    }
    if (!fn_8004F3BC(3)) {
        data->m_assist &= ~ASSIST_BARBARA;
    }
    if (!fn_8004F3BC(4)) {
        data->m_assist &= ~ASSIST_GRAY_FOX;
    }
    if (!fn_8004F3BC(5)) {
        data->m_assist &= ~ASSIST_SHADOW;
    }
}

gmItSwitch* gmGetItemSwitchData_NormalStage(int stage) {
    int index;
    switch (stage) {
    default:
        index = 0;
        break;
    case 0x1: // Stages::BattleField
        index = 1;
        break;
    case 0x2: // Stages::FinalDestination
        index = 2;
        break;
    case 0x3: // Stages::DelfinoPlaza
        index = 3;
        break;
    case 0x4: // Stages::LuigiMansion
        index = 4;
        break;
    case 0x5: // Stages::MushroomyKingdom
        index = 5;
        break;
    case 0x6: // Stages::MarioCircuit
        index = 6;
        break;
    case 0x7: // Stages::_75m
        index = 7;
        break;
    case 0x8: // Stages::RumbleFalls
        index = 8;
        break;
    case 0x9: // Stages::PirateShip
        index = 9;
        break;
    case 0xB: // Stages::Norfair
        index = 10;
        break;
    case 0xC: // Stages::FrigateOrpheon
        index = 11;
        break;
    case 0xD: // Stages::YoshiIsland
        index = 12;
        break;
    case 0xE: // Stages::Halberd
        index = 13;
        break;
    case 0x13: // Stages::LylatCruise
        index = 14;
        break;
    case 0x14: // Stages::PokemonStadium2
        index = 15;
        break;
    case 0x15: // Stages::SpearPillar
        index = 16;
        break;
    case 0x16: // Stages::PortTown
        index = 17;
        break;
    case 0x17: // Stages::Summit
        index = 18;
        break;
    case 0x18: // Stages::FlatZone2
        index = 19;
        break;
    case 0x19: // Stages::CastleSiege
        index = 20;
        break;
    case 0x1C: // Stages::WarioWare
        index = 21;
        break;
    case 0x1D: // Stages::DistantPlanet
        index = 22;
        break;
    case 0x1E: // Stages::Skyworld
        index = 23;
        break;
    case 0x1F: // Stages::MarioBros
        index = 24;
        break;
    case 0x20: // Stages::NewPorkCity
        index = 25;
        break;
    case 0x21: // Stages::Smashville
        index = 26;
        break;
    case 0x22: // Stages::ShadowMoses
        index = 27;
        break;
    case 0x23: // Stages::GreenHillZone
        index = 28;
        break;
    case 0x24: // Stages::PictoChat
        index = 29;
        break;
    case 0x25: // Stages::Hanenbow
        index = 30;
        break;
    case 0x29: // Stages::Temple
        index = 31;
        break;
    case 0x2A: // Stages::YoshiIslandMelee
        index = 32;
        break;
    case 0x2B: // Stages::JungleJapes
        index = 33;
        break;
    case 0x2C: // Stages::Onett
        index = 34;
        break;
    case 0x2D: // Stages::GreenGreens
        index = 35;
        break;
    case 0x2E: // Stages::PokemonStadium
        index = 36;
        break;
    case 0x2F: // Stages::RainbowCruise
        index = 37;
        break;
    case 0x30: // Stages::Corneria
        index = 38;
        break;
    case 0x31: // Stages::BigBlue
        index = 39;
        break;
    case 0x32: // Stages::Brinstar
        index = 40;
        break;
    case 0x33: // Stages::BridgeOfEldin
        index = 41;
        break;
    case 0x35: // Stages::Builder
        index = 42;
        break;
    }
    gmItSwitchWords* data = &lbl_804977E0;
    *data = lbl_8042C4B8[index];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

gmItSwitch* gmGetItemSwitchData_Simple(int kind) {
    // HYPOTHESIS: maps the simple mode kind to its row of the table
    int rows[14] = { 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 10, 11 };
    gmItSwitchWords* data = &lbl_804977E0;
    *data = lbl_8042C768[rows[kind]];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

gmItSwitch* gmGetItemSwitchData_Allstar(int stage) {
    gmItSwitchWords* data = &lbl_804977E0;
    *data = lbl_8042C828[stage];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

// kind 1 = solo events, 2 = co-op events
gmItSwitch* gmGetItemSwitchData_Event(int event, int kind) {
    gmItSwitchWords* data = &lbl_804977E0;
    if (kind == 1) {
        *data = lbl_8042C958[event];
    }
    if (kind == 2) {
        *data = lbl_8042CC88[event];
    }
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

gmItSwitch* gmGetItemSwitchData_Challenger(int challenger) {
    gmItSwitchWords* data = &lbl_804977E0;
    *data = lbl_8042CFB8[challenger];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

gmItSwitch* gmGetItemSwitchData_Kumite() {
    gmItSwitchWords* data = &lbl_804977E0;
    *data = lbl_8042D098[0];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

// HYPOTHESIS: a collection record packs the figure or sticker id and its rarity into one halfword.
struct gmCollectionData {
    u16 m_id : 10;
    u16 m_rarity : 4;
    u16 m_unk : 2;
};

extern const gmCollectionData lbl_8042D180[];
extern const gmCollectionData lbl_8042D5C0[];
extern const u16 lbl_8042DBC0[];
extern int lbl_8059CE40;
extern int lbl_8059CE44;
extern unsigned char lbl_805A00E8[1];

// HYPOTHESIS: the figure table is sorted by rarity class: the first counts[0] + counts[1] entries are not drawn by
// the lottery, then come the classes of the draw order in turn, each with counts[class] figures. The sticker table
// is grouped by sticker set; within a set class 4 (empty) comes first, then the classes in draw order (-1 skips a
// slot). NOTE: the original reaches these tables as fixed offsets of the figure table (+0x9B8, +0xA58 for
// figures; +0x440 sticker table, +0x9DC, +0xA80 for stickers), i.e. it addresses them through one base register;
// declaring them separately as here does not reproduce that and keeps the two lotteries from matching.
extern const int lbl_8042DB38[9];
extern const int lbl_8042DBD8[6];
extern const int lbl_8042DB5C[5][5];
extern const int lbl_8042DC00[6];

// MATCH-ONLY: gm_data_collection.o was its own translation unit in the original, so gm_lib.o calls these out of line.
#pragma push
#pragma auto_inline off

void gmMakeCollectionRareTable() {
    lbl_805A00E8[0] = 1;
}

u16 gmCheckFigureRarityToCollection(u32 id) {
    u16 rarity = 9;
    for (int i = 0; i < lbl_8059CE40; i++) {
        if (id == lbl_8042D180[i].m_id) {
            rarity = lbl_8042D180[i].m_rarity;
            break;
        }
    }
    return lbl_8042DBC0[rarity];
}

// Draws a figure at random among the rarity classes selected in the bit mask (bit i = i-th class in draw order).
u16 gmLotFigureToCollection(u32 rarityMask) {
    if (lbl_805A00E8[0] == 0) {
        return 0;
    }
    int total = 0;
    for (int i = 0; i < 6; i++) {
        if (rarityMask & (1 << i)) {
            total += lbl_8042DB38[lbl_8042DBD8[i]];
        }
    }
    int pick = randi(total);
    int index = lbl_8042DB38[0] + lbl_8042DB38[1];
    int sum = 0;
    for (int i = 0; i < 6; i++) {
        int count = lbl_8042DB38[lbl_8042DBD8[i]];
        if (rarityMask & (1 << i)) {
            if (sum + count > pick) {
                break;
            }
            sum += count;
        }
        index += count;
    }
    return lbl_8042D180[index + (pick - sum)].m_id;
}

// Same for stickers. HYPOTHESIS: the second argument is the 1-based column of the count table (the caller in
// gmLotSealToBitset passes rarity + 1).
u16 gmLotSealToCollection(u32 rarityMask, int column1) {
    if (lbl_805A00E8[0] == 0) {
        return 0;
    }
    int column = column1 - 1;
    int total = 0;
    for (int i = 0; i < 6; i++) {
        int rarity = lbl_8042DC00[i];
        if (rarity != -1 && (rarityMask & (1 << i))) {
            total += lbl_8042DB5C[rarity][column];
        }
    }
    int pick = randi(total);
    int index = 0;
    for (int s = 0; s < 5; s++) {
        if (s == column) {
            break;
        }
        for (int r = 0; r < 5; r++) {
            index += lbl_8042DB5C[r][s];
        }
    }
    index += lbl_8042DB5C[4][column];
    int sum = 0;
    for (int i = 0; i < 6; i++) {
        int rarity = lbl_8042DC00[i];
        if (rarity == -1) {
            continue;
        }
        int count = lbl_8042DB5C[rarity][column];
        if (rarityMask & (1 << i)) {
            if (sum + count > pick) {
                break;
            }
            sum += count;
        }
        index += count;
    }
    return lbl_8042D5C0[index + (pick - sum)].m_id;
}

int gmFig_Id2Index(int id) {
    if (id < 0) {
        return -1;
    }
    for (u16 i = 0; i < lbl_8059CE40; i++) {
        if (id == lbl_8042D180[i].m_id) {
            return i;
        }
    }
    return -1;
}

int gmFig_Index2Id(int index) {
    int id = -1;
    if (index >= 0 && index < lbl_8059CE40) {
        id = lbl_8042D180[index].m_id;
    }
    return id;
}

int gmSeal_Id2Index(int id) {
    if (id < 0) {
        return -1;
    }
    for (u16 i = 0; i < lbl_8059CE44; i++) {
        if (id == lbl_8042D5C0[i].m_id) {
            return i;
        }
    }
    return -1;
}

int gmSeal_Index2Id(int index) {
    int id = -1;
    if (index >= 0 && index < lbl_8059CE44) {
        id = lbl_8042D5C0[index].m_id;
    }
    return id;
}

int gmGetFigMaxCount() {
    return lbl_8059CE40;
}

int gmGetSealMaxCount() {
    return lbl_8059CE44;
}

#pragma pop

// The collection lists keep a file handle for loading; nothing else about it is known.
// HYPOTHESIS: the static object of gm_data_collection.cpp, constructed by this unit's __sinit_.
gfFileIOHandle g_gmDataCollectionFileHandle;
