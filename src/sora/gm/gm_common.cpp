#include <types.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <string.h>
#include <stdio.h>
#include <it/it_create.h>
#include <mt/mt_prng.h>
#include <gm/gm_global_corps.h>
#include <gf/gf_scene.h>
#include <gf/gf_file_io_handle.h>

// ---- gmlib 0x8004F294-0x80050BB0 ----

// HYPOTHESIS: a movie record is its hidden-movie id and its file name.
struct gmMovieData {
    u8 m_hideId;
    const char* m_name;
};

// HYPOTHESIS: one record per hidden character: hidden kind, character kind, interface kind and menu kind.
struct gmHideCharaData {
    u8 m_hideKind;
    u8 m_charaKind;
    u8 m_ifKind;
    u8 m_menuKind;
};

extern const char* lbl_8042B990[];
extern const char* lbl_8042BC18[];
extern const gmMovieData lbl_804069A0[];
extern const char* lbl_8059CA80;
extern const char* lbl_8059CA88[2];
extern const gmHideCharaData lbl_80406968[14];
extern const u32 lbl_8042BDC0[102];
extern const u32 lbl_8042B858[];
extern const srStageKind lbl_8042C0A0[12];

// HYPOTHESIS: the background music that is tied to a stage: its id and the stage that has to be unlocked for it.
struct gmStageBgmData {
    u32 m_bgmId;
    srStageKind m_stageKind;
};
extern const gmStageBgmData lbl_8042BF98[31];

// The record sections hold bit sets; GameGlobal hands out their words (names from the sora map).
// HYPOTHESIS: the return types are word pointers because the callers index them by 32-bit word.

// The menu's fighter numbering: muMenu::exchangeGmCharacterKindToMuCharKind (mu_menu.cpp). The native callers pass the
// fighter in r3 only (the mangled name says three ints), so this stand-in is declared with one parameter.
// HYPOTHESIS: MATCH-ONLY declaration; the real header declaration is unknown.
extern "C" int exchangeGmCharacterKindToMuCharKind__6muMenuFiii(int kind);

// Callees that belong to other units / clusters (names from the sora map).
// HYPOTHESIS: parameter and return types come from the call sites only.

// The result of gmCharaFlag::setGlobalCharaData: a class with a copy constructor, so it comes back through a hidden pointer.
struct gmCharaFlagResult : gmCharaFlagWords {
    gmCharaFlagResult() {}
    gmCharaFlagResult(const gmCharaFlagResult& other) {
        m_word0 = other.m_word0;
        m_word1 = other.m_word1;
    }
};
extern "C" gmCharaFlagResult fn_80051F28();                                   // gmCharaFlag::setGlobalCharaData

// HYPOTHESIS: gmAppData flag byte 0x28; bit 7 is a one-shot request that gmInitMeleeDataDefault moves into the melee data.
struct gmAppDataFlags {
    char _0[0x28];
    bool m_oneShot28_7 : 1;
    bool m_28_6 : 1;
    bool m_28_5 : 1;
    bool m_28_4 : 1;
    bool m_28_3 : 1;
    bool m_28_2 : 1;
    bool m_28_1 : 1;
    bool m_28_0 : 1;
};

// Bit number to (word, mask) with signed arithmetic, as written in all of the callers.
static inline u32 gmCheckBit(const u32* bits, int bit) {
    return (bits[bit / 32] & (1 << (bit % 32))) != 0;
}

static inline void gmSetBit(u32* bits, int bit) {
    bits[bit / 32] |= 1 << (bit % 32);
}

static inline void gmResetBit(u32* bits, int bit) {
    bits[bit / 32] &= ~(1 << (bit % 32));
}

// Saturating counters of the record. This function sits at 0x8004F258, just in front of this unit's address
// range (it belongs to the preceding translation unit); it is inlined into the total counters below.
// HYPOTHESIS: the name is the map's gmlib::addMaxCheck, parameter roles come from its use.
static inline void addMaxCheck(u32* dst, u32 add, u32 max) {
    bool over = add > max || *dst > max - add;
    u32 result = max;
    if (!over) {
        result = *dst + add;
    }
    *dst = result;
}

// HYPOTHESIS: the largest value of a play count in the record.
#define GM_COUNT_MAX 99999999

const char* getChKindNameString(int kind) {
    return lbl_8042B990[kind];
}

const char* getStageNameString(int kind) {
    return lbl_8042BC18[kind];
}

int gmRetHideMovieID(GmMovieID id) {
    return lbl_804069A0[id].m_hideId;
}

void gmGetMovieFileName(char* buf, GmMovieID id) {
    sprintf(buf, "%s/%s.%s", lbl_8059CA80, lbl_804069A0[id].m_name, lbl_8059CA88[0]);
}

void gmGetMovieFileNameNoPath(char* buf, GmMovieID id) {
    sprintf(buf, "%s", lbl_804069A0[id].m_name);
}

gmCharacterKind gmRetHideKindToCharacterKind(u32 hideKind) {
    for (int i = 0; i < 14; i++) {
        if (hideKind == lbl_80406968[i].m_hideKind) {
            return (gmCharacterKind)lbl_80406968[i].m_charaKind;
        }
    }
    return Character_SelectNone;
}

u32 gmRetIfKindMenukind(int ifKind) {
    for (int i = 0; i < 14; i++) {
        if (ifKind == lbl_80406968[i].m_ifKind) {
            return lbl_80406968[i].m_menuKind;
        }
    }
    return 0x80;
}

bool gmCheckAssistKindUseEnable(int kind) {
    return gmCheckBit(g_GameGlobal->getAssistAppFlag(), kind);
}

void gmInitAssistKindUseEnable() {
    u32* flags = g_GameGlobal->getAssistAppFlag();
    *flags = 0;
}

// The event counters that start out opened: 10 solo events, 8 co-op events.
// HYPOTHESIS: index 0 is the solo list and 1 the co-op list (roles from the event menu).
void gmInitEventOpened() {
    *g_GameGlobal->getGlobalRecordGetOpenedEventDatap(0) = 10;
    *g_GameGlobal->getGlobalRecordGetOpenedEventDatap(1) = 8;
}

// The hidden-fighter table is searched by the menu's fighter number; a fighter that is not in it is always usable (14).
// Every hidden fighter owns three bits in the fighter bit set: 3 * hide + 1 is "usable", + 2 is "has been looked at".
static inline u8 gmGetHideKindFromChKind(gmCharacterKind kind) {
    int muChar = (u8)exchangeGmCharacterKindToMuCharKind__6muMenuFiii((u8)kind);
    for (int i = 0; i < 14; i++) {
        if (muChar == lbl_80406968[i].m_ifKind) {
            return lbl_80406968[i].m_hideKind;
        }
    }
    return 14;
}

bool gmCheckChKindUseEnable(gmCharacterKind kind) {
    u32* bits = g_GameGlobal->getCharaAppFlag();
    u8 hide = gmGetHideKindFromChKind(kind);
    if (hide == 14 || gmCheckBit(bits, hide * 3 + 1)) {
        return true;
    }
    return false;
}

void gmSetChKindUseEnable(gmCharacterKind kind) {
    u32* bits = g_GameGlobal->getCharaAppFlag();
    u8 hide = gmGetHideKindFromChKind(kind);
    if (hide != 14) {
        gmSetBit(bits, hide * 3 + 1);
    }
}

bool gmCheckChKindUseEnableAll() {
    u32* bits = g_GameGlobal->getCharaAppFlag();
    for (int i = 0; i < 14; i++) {
        if (!gmCheckBit(bits, i * 3 + 1)) {
            return false;
        }
    }
    return true;
}

void gmInitChKindUseEnableAll() {
    u32* bits = g_GameGlobal->getCharaAppFlag();
    bits[0] = 0;
    bits[1] = 0;
}

// A stage in the table of lockable stages owns the bit of its table index in the stage bit set.
// A stage that is not in the table is always available.
bool gmCheckSrStageKind(srStageKind stage) {
    bool usable = true;
    for (int i = 0; i < 12; i++) {
        if (stage == lbl_8042C0A0[i]) {
            usable = gmCheckBit(g_GameGlobal->getStageAppFlag(), i);
            break;
        }
    }
    return usable;
}

void gmSetStageUseEnable(srStageKind stage) {
    for (int i = 0; i < 12; i++) {
        if (stage == lbl_8042C0A0[i]) {
            gmSetBit(g_GameGlobal->getStageAppFlag(), i);
        }
    }
}

void gmInitStageUseEnableAll() {
    u32* flags = g_GameGlobal->getStageAppFlag();
    *flags = 0;
}

// HYPOTHESIS: name and signature; the search over the 102 hideable songs (index = GmHideBGMKind) is the inverse of
// gmGetBgmIDToHideBGMKind. 102 means that the song is not hideable.
int gmGetHideBGMKindFromBgmID(u32 bgmId) {
    for (int i = 0; i < 102; i++) {
        if (bgmId == lbl_8042BDC0[i]) {
            return i;
        }
    }
    return 102;
}

u32 gmGetBgmIDToHideBGMKind(GmHideBGMKind kind) {
    return lbl_8042BDC0[kind];
}

// Every hideable song owns two bits in the song bit set; the lower one means "opened".
// A song that is tied to a stage is available once the stage is.
bool gmCheckBGMIDUseEnable(u32 bgmId) {
    int hide = gmGetHideBGMKindFromBgmID(bgmId);
    if (hide != 102) {
        return gmCheckBit((u32*)g_GameGlobal->getBGMAppFlag(), hide * 2);
    }
    for (int i = 0; i < 31; i++) {
        if (bgmId == lbl_8042BF98[i].m_bgmId) {
            if (!gmCheckSrStageKind(lbl_8042BF98[i].m_stageKind)) {
                return false;
            }
        }
    }
    return true;
}

void gmSetBGMIDUseEnable(u32 bgmId) {
    int hide = gmGetHideBGMKindFromBgmID(bgmId);
    if (hide != 102) {
        gmSetBit((u32*)g_GameGlobal->getBGMAppFlag(), hide * 2);
    }
}

void gmResetBGMIDUseEnable(u32 bgmId) {
    int hide = gmGetHideBGMKindFromBgmID(bgmId);
    if (hide != 102) {
        gmResetBit((u32*)g_GameGlobal->getBGMAppFlag(), hide * 2);
    }
}

u32 gmCountHideBGMOpend() {
    int count = 0;
    for (int i = 0; i < 102; i++) {
        if (gmCheckBit((u32*)g_GameGlobal->getBGMAppFlag(), i * 2) == 1) {
            count++;
        }
    }
    return count;
}

// HYPOTHESIS: 125 songs are always available (the count of the songs that are not hideable and not tied to a stage).
u32 gmCountBGMOpend() {
    int count = 125;
    for (int i = 0; i < 102; i++) {
        if (gmCheckBit((u32*)g_GameGlobal->getBGMAppFlag(), i * 2) == 1) {
            count++;
        }
    }
    for (int i = 0; i < 31; i++) {
        if (gmCheckSrStageKind(lbl_8042BF98[i].m_stageKind) == 1) {
            count++;
        }
    }
    return count;
}

void gmInitPlayerDataDefault(gmPlayerInitData* player, int playerNo) {
    memset(player, 0, sizeof(gmPlayerInitData));
    // MATCH-ONLY: the native code stores the fighter slot as a plain byte, not as an enum bit field.
    *reinterpret_cast<u8*>(player) = Character_SelectNone;
    player->m_state = 3;
    player->m_nameIndex = 0x78;
    player->m_playerId = 0;
    player->m_name[0] = 0;
    player->m_teamNo = 0;
    player->m_colorFileNo = 0;
    player->m_colorNo = 0;
    player->m_0xA = 0;
    player->m_0x20 = 0;
    player->m_0x9 = 0;
    player->m_0x50 = 0.0f;
    player->m_0x54 = 1.0f;
    player->m_0x58 = 0.0f;
    player->m_scale = 1.0f;
    player->m_visibilityScale = 1.0f;
    player->m_startDamage = 0;
    player->m_hitPointMax = 0;
    player->m_0x3C = 0;
    player->m_gravity = 1.0f;
    player->m_isMetal = false;
    player->m_0x1B_3 = false;
    player->m_isSpycloak = false;
    player->m_0x1B_6 = false;
    player->m_isStamina = false;
    player->m_isRabbitCap = false;
    player->m_isFlower = false;
    player->m_isCurry = false;
    player->m_isReflector = false;
    player->m_isRebirthXlu = true;
    player->m_0x3F = playerNo;
    player->m_cpuType = 0;
    player->m_cpuRank = 100;
    player->m_startPointIdx = 0;
    player->m_controllerNo = 0;
}

// HYPOTHESIS: the defaults of one match: 2 minute time limit, four players, all items on, medium item frequency.
void gmInitMeleeDataDefault(gmMeleeInitData* melee) {
    memset(melee, 0, sizeof(gmMeleeInitData));
    melee->m_0x2_7 = true;
    melee->m_0x2_1 = false;
    melee->m_gameRule = Game_Rule_Time;
    melee->m_numPlayers = 4;
    melee->m_0x1_1 = true;
    melee->m_0x1_0 = false;
    melee->m_timeLimitFrames = 7200;
    melee->m_isTeams = false;
    melee->m_stageKind = 0;
    melee->m_subStageKind = 0;
    melee->m_damageRatio = 1.0f;
    melee->m_suicideScoreMultiplier = -1;
    melee->m_gameSpeed = 1.0f;
    melee->m_0x5_5 = false;
    melee->m_0x5_4 = false;
    melee->m_0x5_3 = false;
    melee->m_corpsKind = Corps_None;
    melee->m_allowPause = true;
    melee->m_0x3_2 = true;
    melee->m_hideDamageGauge = false;
    melee->m_0x3_0 = false;
    melee->m_scoreDisplay = false;
    // MATCH-ONLY: the native code sets the three item switches as one doubleword and two words.
    *reinterpret_cast<u64*>(&melee->m_itSwitch.m_item) = 0xFFFFFFFFFFFFFFFFULL;
    *reinterpret_cast<u32*>(&melee->m_itSwitch.m_pokemon) = 0xFFFFFFFF;
    *reinterpret_cast<u32*>(&melee->m_itSwitch.m_assist) = 0xFFFFFFFF;
    melee->m_charaFlags = fn_80051F28();
    melee->m_itemFrequency = gmItSwitch::Frequency_Medium;
    melee->m_0x10 = 0x78;
    melee->m_cameraShakeScale = 1.0f;
    melee->m_0x5a_7 = g_GameGlobal->getGlobalRecordGameDatap()->m_unk808_7;
    gmAppDataFlags* app = reinterpret_cast<gmAppDataFlags*>(g_GameGlobal->m_appData);
    melee->m_0x5a_6 = app->m_oneShot28_7;
    app = reinterpret_cast<gmAppDataFlags*>(g_GameGlobal->m_appData);
    app->m_oneShot28_7 = false;
    melee->m_0x50 = 1;
}

// Initialises the seven player slots of the global melee setup from the character selection.
static inline void gmInitPlayersFromSelect(gmGlobalModeMelee* global) {
    gmInitMeleeDataDefault(&global->m_meleeInitData);
    gmSelCharData* select = g_GameGlobal->m_selCharData;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        gmPlayerInitData* player = &global->m_playersInitData[i];
        gmInitPlayerDataDefault(player, i);
        global->m_playersInitData[i].m_cpuType = 0;
        global->m_playersInitData[i].m_cpuRank = lbl_8042B858[select->m_playersInitData[i].m_cpuRank];
        global->m_playersInitData[i].m_playerId = i;
        global->m_playersInitData[i].m_startPointIdx = i;
        global->m_playersInitData[i].m_controllerNo = select->m_playersInitData[i].m_controllerNo;
        u8 nameIndex = select->m_playersInitData[i].m_nameIndex;
        global->m_playersInitData[i].m_nameIndex = nameIndex;
        if (nameIndex != 0x78) {
            g_GameGlobal->getNameUTF16(nameIndex, player->m_name);
        }
    }
    gmGlobalRecord* record = g_GameGlobal->getGlobalRecordGameDatap();
    g_GameGlobal->m_modeMelee->m_meleeInitData.m_0x58 = record->m_unk2E4;
    record = g_GameGlobal->getGlobalRecordGameDatap();
    g_GameGlobal->m_modeMelee->m_meleeInitData.m_0x59 = record->m_unk2E5;
}

void gmInitVSMelee(gmGlobalModeMelee* global) {
    gmInitPlayersFromSelect(global);
    g_GameGlobal->m_handicapCounts[0] = 0;
    g_GameGlobal->m_handicapCounts[1] = 0;
    g_GameGlobal->m_handicapCounts[2] = 0;
    g_GameGlobal->m_handicapCounts[3] = 0;
    global->_0[0] = -1;
    global->_0[1] = 1;
    global->_0[2] = -1;
}

// Clears the handicap that was carried over from the previous match for the four human players.
void gmResetHandiSetting() {
    for (int i = 0; i < 4; i++) {
        g_GameGlobal->m_handicapCounts[i] = 0;
        gmPlayerInitData* select = &g_GameGlobal->m_selCharData->m_playersInitData[i];
        int nameIndex = select->m_nameIndex;
        if (nameIndex != 0x78) {
            u16* handicap = g_GameGlobal->getVsModeAutoHandiPlayerVal(i, nameIndex);
            if (handicap != nullptr) {
                *handicap = 0;
            }
        }
        g_GameGlobal->m_selCharData->m_playersInitData[i].m_handicap = 0;
    }
}

void gmInitGlobalModeMelee(gmGlobalModeMelee* global) {
    gmInitPlayersFromSelect(global);
}

// Copies the rule screen's settings (gmSetRule) into the global melee setup.
// HYPOTHESIS: rule 1 is stock, 2 is coin and everything else is time; stock matches have their own time limit.
void gmSetRuleToGlobalModeMelee(gmGlobalModeMelee* global) {
    // The native code calls this accessor and drops the result (a leftover of a removed use of the menu data).
    gmGlobalRecord::MenuData* menu = g_GameGlobal->getGlobalRecordMenuDatap();
    gmSetRule* rule = g_GameGlobal->m_setRule;
    gmSelCharData* select = g_GameGlobal->m_selCharData;
    gmMeleeInitData* melee = &global->m_meleeInitData;
    melee->m_gameRule = rule->m_rule;
    switch (melee->m_gameRule) {
    case Game_Rule_Time:
    default:
        melee->m_0x1_1 = true;
        melee->m_timeLimitFrames = rule->m_timeMinutes * 3600;
        break;
    case Game_Rule_Stock:
        melee->m_0x1_1 = true;
        melee->m_timeLimitFrames = rule->m_stockTimeMinutes * 3600;
        break;
    case Game_Rule_Coin:
        melee->m_0x1_1 = true;
        melee->m_timeLimitFrames = rule->m_timeMinutes * 3600;
        break;
    }
    melee->m_0x2_7 = melee->m_timeLimitFrames != 0;
    for (int i = 0; i < 4; i++) {
        switch (rule->m_handicap) {
        case 0:
        default:
            global->m_playersInitData[i].m_handicap = 0;
            break;
        case 1: {
            u16* handicap = g_GameGlobal->getVsModeAutoHandiPlayerVal(i, select->m_playersInitData[i].m_nameIndex);
            global->m_playersInitData[i].m_handicap = handicap != nullptr ? *handicap : 0;
            break;
        }
        case 2:
            global->m_playersInitData[i].m_handicap = select->m_playersInitData[i].m_handicap;
            break;
        }
        u16 handicap = global->m_playersInitData[i].m_handicap;
        global->m_playersInitData[i].m_startDamage = handicap;
    }
    float ratio = rule->m_damageRatio / 10.0f;
    melee->m_damageRatio = ratio;
    global->m_playersInitData[0].m_damageReactionMul = ratio;
    global->m_playersInitData[1].m_damageReactionMul = ratio;
    global->m_playersInitData[2].m_damageReactionMul = ratio;
    global->m_playersInitData[3].m_damageReactionMul = ratio;
    melee->m_isTeamAttack = rule->m_isTeamAttack;
    melee->m_allowPause = rule->m_allowPause;
    switch (melee->m_gameRule) {
    case Game_Rule_Time:
    default:
        melee->m_0x3_2 = true;
        melee->m_scoreDisplay = rule->m_showScoreDisplay;
        break;
    case Game_Rule_Stock:
        melee->m_0x3_2 = false;
        melee->m_scoreDisplay = true;
        break;
    case Game_Rule_Coin:
        melee->m_0x3_2 = true;
        melee->m_scoreDisplay = true;
        break;
    }
    melee->m_hideDamageGauge = !rule->m_showDamageGauge;
}

u32 gmGetDateCount() {
    u64 count = OS_TICKS_TO_SEC(OSGetTime());
    if (count > 0xFFFFFFFF) {
        count = 0xFFFFFFFF;
    }
    return count;
}

void gmGetCalendarTime(OSCalendarTime* time, u32 count) {
    OSTicksToCalendarTime(OS_SEC_TO_TICKS((u64)count), time);
}

u32 gmGetOfflineVsMeleeTotalCount();
u32 gmGetOnlineVsMeleeTotalCount();

// The total of the vs and single-player match time counters, 0xFFFFFFFF when it would overflow.
u32 gmGetAllMeleeTimeTotal() {
    GameGlobal* global = g_GameGlobal;
    u32 vsTime = *global->getGlobalRecordVsMeleeTimeCountDatap();
    u32 singleTime = *global->getGlobalRecordSingleMeleeTimeCountDatap();
    u32 total = 0xFFFFFFFF;
    if (vsTime <= 0xFFFFFFFF - singleTime) {
        total = vsTime + singleTime;
    }
    return total;
}

// HYPOTHESIS: the total of the offline and online vs match counters is saturated at GM_COUNT_MAX.
u32 gmGetVsMeleeTotalCount() {
    u32 total = 0;
    addMaxCheck(&total, gmGetOfflineVsMeleeTotalCount(), GM_COUNT_MAX);
    addMaxCheck(&total, gmGetOnlineVsMeleeTotalCount(), GM_COUNT_MAX);
    return total;
}

u32 gmGetOfflineVsMeleeTotalCount() {
    u32 total = 0;
    addMaxCheck(&total, *g_GameGlobal->getGlobalRecordVsTimeMeleeCountDatap(), GM_COUNT_MAX);
    addMaxCheck(&total, *g_GameGlobal->getGlobalRecordVsStockMeleeCountDatap(), GM_COUNT_MAX);
    addMaxCheck(&total, *g_GameGlobal->getGlobalRecordVsCoinMeleeCountDatap(), GM_COUNT_MAX);
    addMaxCheck(&total, *g_GameGlobal->getGlobalRecordVsHpMeleeCountDatap(), GM_COUNT_MAX);
    return total;
}

u32 gmGetOnlineVsMeleeTotalCount() {
    u32 total = 0;
    addMaxCheck(&total, *g_GameGlobal->getGlobalRecordOnlineVsTimeMeleeCountDatap(), GM_COUNT_MAX);
    addMaxCheck(&total, *g_GameGlobal->getGlobalRecordOnlineVsStockMeleeCountDatap(), GM_COUNT_MAX);
    addMaxCheck(&total, *g_GameGlobal->getGlobalRecordOnlineVsCoinMeleeCountDatap(), GM_COUNT_MAX);
    return total;
}

// ---- gmlib 0x80050BB0-0x80051F28 ----

// gm_common_b: gmlib melee item switch lookup, trophy coin counters, collection rarity bits and the trophy, sticker and
// CD lotteries (0x80050BB0-0x80051F28 of main.dol). The tables that are read here are in the data of gm_common.cpp:
// lbl_8042B858 is a larger data block of which the song ids start at word 0x15A, the hidden-song stage keys at 0x1C0 and
// the stage list at 0x212 (the offsets are what the original code adds to that base).

extern const u32 lbl_8042B858[];
extern const int lbl_8042BF58[];

// Stand-ins for main.dol functions of the neighbouring gm_common clusters.
// gmdatacollection::gmLotFigureToCollection(rare bit set): picks a figure id (HYPOTHESIS: parameter and return type).
extern "C" u16 fn_8005630C(u8 rareBits);
// gmdatacollection::gmLotSealToCollection(rare bit set, rarity): picks a sticker id (HYPOTHESIS: parameter and return type).
extern "C" u16 fn_80056518(u8 rareBits, u8 rarity);
// GameGlobal::getStageAppFlag(): the bit set of the unlocked stages (the map names it getStageAppFlag).

// HYPOTHESIS: the item switch of a match is three bit tables. 0 is the item bit set (64 bits, the high word comes
// first in memory), 1 the Pokemon word and 2 the assist trophy word. A switch bit of -1 means that the item has no switch.
enum ItemSwitchTable {
    ItemSwitchTable_Item = 0,
    ItemSwitchTable_Pokemon = 1,
    ItemSwitchTable_Assist = 2,
};

// MATCH-ONLY: gmItSwitch has byte alignment in the shared header, so passing it by value copies it byte by byte. The
// original copies four words; this word view of it (same 0x10 bytes) reproduces that.
struct gmItSwitchWords {
    u64 m_item;
    u32 m_pokemon;
    u32 m_assist;
    gmItSwitchWords(const gmItSwitchWords& other) : m_item(other.m_item), m_pokemon(other.m_pokemon), m_assist(other.m_assist) {
    }
};

// Whether the item may appear in a match with the given item switch (gmItSwitch). Items that have no switch bit are always allowed.
bool gmCheckItemSwitch(gmItSwitchWords itemSwitch, itKind kind) {
    int bit = -1;
    int table;
    switch (kind) {
    case Item_AssistTrophy: bit = 1; table = ItemSwitchTable_Item; break; // m_assistTrophy
    case Item_FranklinBadge: bit = 47; table = ItemSwitchTable_Item; break; // m_franklinBadge
    case Item_BananaPeel: bit = 41; table = ItemSwitchTable_Item; break; // m_bananaPeel
    case Item_Barrel: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_BeamSword: bit = 19; table = ItemSwitchTable_Item; break; // m_beamSword
    case Item_Bill: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_BobOmb: bit = 30; table = ItemSwitchTable_Item; break; // m_bobOmb
    case Item_Crate: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_Bumper: bit = 42; table = ItemSwitchTable_Item; break; // m_bumper
    case Item_Capsule: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_RollingCrate: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_CD: bit = 51; table = ItemSwitchTable_Item; break; // m_cd
    case Item_GooeyBomb: bit = 32; table = ItemSwitchTable_Item; break; // m_gooeyBomb
    case Item_CrackerLauncher: bit = 29; table = ItemSwitchTable_Item; break; // m_crackerLauncher
    case Item_CrackerLauncher_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Coin: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_SuperspicyCurry: bit = 16; table = ItemSwitchTable_Item; break; // m_superspicyCurry
    case Item_SuperspicyCurry_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_DekuNut: bit = 34; table = ItemSwitchTable_Item; break; // m_dekuNut
    case Item_MrSaturn: bit = 39; table = ItemSwitchTable_Item; break; // m_mrSaturn
    case Item_Dragoon_Part: bit = 9; table = ItemSwitchTable_Item; break; // m_dragoonParts
    case Item_Dragoon_Set: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Dragoon_Sight: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Trophy: bit = 50; table = ItemSwitchTable_Item; break; // m_trophy
    case Item_FireFlower: bit = 28; table = ItemSwitchTable_Item; break; // m_fireFlower
    case Item_FireFlower_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Freezie: bit = 35; table = ItemSwitchTable_Item; break; // m_freezie
    case Item_GoldenHammer: bit = 25; table = ItemSwitchTable_Item; break; // m_goldenHammer
    case Item_GreenShell: bit = 40; table = ItemSwitchTable_Item; break; // m_greenShell
    case Item_Hammer: bit = 24; table = ItemSwitchTable_Item; break; // m_hammer
    case Item_Hammer_Head: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Fan: bit = 21; table = ItemSwitchTable_Item; break; // m_fan
    case Item_HeartContainer: bit = 8; table = ItemSwitchTable_Item; break; // m_heartContainer
    case Item_HomeRunBat: bit = 20; table = ItemSwitchTable_Item; break; // m_homeRunBat
    case Item_PartyBall: bit = 3; table = ItemSwitchTable_Item; break; // m_containers
    case Item_Manaphy_Heart: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_MaximTomato: bit = 7; table = ItemSwitchTable_Item; break; // m_maximTomato
    case Item_PoisonMushroom: bit = 11; table = ItemSwitchTable_Item; break; // m_poisonMushroom
    case Item_SuperMushroom: bit = 10; table = ItemSwitchTable_Item; break; // m_superMushroom
    case Item_MetalBox: bit = 14; table = ItemSwitchTable_Item; break; // m_metalBox
    case Item_Hothead: bit = 38; table = ItemSwitchTable_Item; break; // m_hothead
    case Item_Pitfall: bit = 37; table = ItemSwitchTable_Item; break; // m_pitfall
    case Item_PokeBall: bit = 2; table = ItemSwitchTable_Item; break; // m_pokeBall
    case Item_BlastBox: bit = 4; table = ItemSwitchTable_Item; break; // m_blastBox
    case Item_RayGun: bit = 27; table = ItemSwitchTable_Item; break; // m_rayGun
    case Item_RayGun_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_LipStick: bit = 22; table = ItemSwitchTable_Item; break; // m_lipStick
    case Item_LipStick_Flower: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_LipStick_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Sandbag: bit = 5; table = ItemSwitchTable_Item; break; // m_sandBag
    case Item_ScrewAttack: bit = 48; table = ItemSwitchTable_Item; break; // m_screwAttack
    case Item_Sticker: bit = 49; table = ItemSwitchTable_Item; break; // m_sticker
    case Item_MotionSensorBomb: bit = 31; table = ItemSwitchTable_Item; break; // m_motionSensorBomb
    case Item_Timer: bit = 17; table = ItemSwitchTable_Item; break; // m_timer
    case Item_SmartBomb: bit = 33; table = ItemSwitchTable_Item; break; // m_smartBomb
    case Item_SmashBall: bit = 0; table = ItemSwitchTable_Item; break; // m_smashBall
    case Item_SmokeBall: bit = 36; table = ItemSwitchTable_Item; break; // m_smokeBall
    case Item_Spring: bit = 43; table = ItemSwitchTable_Item; break; // m_spring
    case Item_StarRod: bit = 23; table = ItemSwitchTable_Item; break; // m_starRod
    case Item_StarRod_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_SoccerBall: bit = 45; table = ItemSwitchTable_Item; break; // m_soccerBall
    case Item_SuperScope: bit = 26; table = ItemSwitchTable_Item; break; // m_superScope
    case Item_SuperScope_Shot: bit = -1; table = ItemSwitchTable_Item; break; // no switch bit
    case Item_Star: bit = 13; table = ItemSwitchTable_Item; break; // m_starman
    case Item_Food: bit = 6; table = ItemSwitchTable_Item; break; // m_food
    case Item_TeamHealer: bit = 46; table = ItemSwitchTable_Item; break; // m_teamHealer
    case Item_Lightning: bit = 18; table = ItemSwitchTable_Item; break; // m_lightning
    case Item_Unira: bit = 44; table = ItemSwitchTable_Item; break; // m_unira
    case Item_BunnyHood: bit = 15; table = ItemSwitchTable_Item; break; // m_bunnyHood
    case Item_WarpStar: bit = 12; table = ItemSwitchTable_Item; break; // m_warpStar
    case Item_Stage_Vegetable: bit = 6; table = ItemSwitchTable_Item; break; // m_food
    case Item_DiddyKong_Peanut_Seed: bit = 6; table = ItemSwitchTable_Item; break; // m_food
    case Item_Pokemon_Torchic: bit = 0; table = ItemSwitchTable_Pokemon; break; // m_torchic
    case Item_Pokemon_Celebi: bit = 1; table = ItemSwitchTable_Pokemon; break; // m_celebi
    case Item_Pokemon_Chikorita: bit = 2; table = ItemSwitchTable_Pokemon; break; // m_chikorita
    case Item_Pokemon_Chikorita_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_Entei: bit = 3; table = ItemSwitchTable_Pokemon; break; // m_entei
    case Item_Pokemon_Moltres: bit = 4; table = ItemSwitchTable_Pokemon; break; // m_moltres
    case Item_Pokemon_Munchlax: bit = 5; table = ItemSwitchTable_Pokemon; break; // m_munchlax
    case Item_Pokemon_Deoxys: bit = 6; table = ItemSwitchTable_Pokemon; break; // m_deoxys
    case Item_Pokemon_Groudon: bit = 7; table = ItemSwitchTable_Pokemon; break; // m_groudon
    case Item_Pokemon_Gulpin: bit = 8; table = ItemSwitchTable_Pokemon; break; // m_gulpin
    case Item_Pokemon_Staryu: bit = 9; table = ItemSwitchTable_Pokemon; break; // m_staryu
    case Item_Pokemon_Staryu_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_HoOh: bit = 10; table = ItemSwitchTable_Pokemon; break; // m_hoOh
    case Item_Pokemon_HoOh_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_Jirachi: bit = 11; table = ItemSwitchTable_Pokemon; break; // m_jirachi
    case Item_Pokemon_Snorlax: bit = 12; table = ItemSwitchTable_Pokemon; break; // m_snorlax
    case Item_Pokemon_Bellossom: bit = 13; table = ItemSwitchTable_Pokemon; break; // m_bellosom
    case Item_Pokemon_Kyogre: bit = 14; table = ItemSwitchTable_Pokemon; break; // m_kyogre
    case Item_Pokemon_Kyogre_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_LatiasLatios: bit = 15; table = ItemSwitchTable_Pokemon; break; // m_latiasLatios
    case Item_Pokemon_Lugia: bit = 16; table = ItemSwitchTable_Pokemon; break; // m_lugia
    case Item_Pokemon_Lugia_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_Manaphy: bit = 17; table = ItemSwitchTable_Pokemon; break; // m_manaphy
    case Item_Pokemon_Weavile: bit = 18; table = ItemSwitchTable_Pokemon; break; // m_weavile
    case Item_Pokemon_Electrode: bit = 19; table = ItemSwitchTable_Pokemon; break; // m_electrode
    case Item_Pokemon_Metagross: bit = 20; table = ItemSwitchTable_Pokemon; break; // m_metagross
    case Item_Pokemon_Mew: bit = 21; table = ItemSwitchTable_Pokemon; break; // m_mew
    case Item_Pokemon_Meowth: bit = 22; table = ItemSwitchTable_Pokemon; break; // m_meowth
    case Item_Pokemon_Meowth_Shot: bit = -1; table = ItemSwitchTable_Pokemon; break; // no switch bit
    case Item_Pokemon_Piplup: bit = 23; table = ItemSwitchTable_Pokemon; break; // m_piplup
    case Item_Pokemon_Togepi: bit = 24; table = ItemSwitchTable_Pokemon; break; // m_togepi
    case Item_Pokemon_Goldeen: bit = 25; table = ItemSwitchTable_Pokemon; break; // m_goldeen
    case Item_Pokemon_Gardevoir: bit = 26; table = ItemSwitchTable_Pokemon; break; // m_gardevoir
    case Item_Pokemon_Wobbuffet: bit = 27; table = ItemSwitchTable_Pokemon; break; // m_wobuffet
    case Item_Pokemon_Suicune: bit = 28; table = ItemSwitchTable_Pokemon; break; // m_suicune
    case Item_Pokemon_Bonsly: bit = 29; table = ItemSwitchTable_Pokemon; break; // m_bonsly
    case Item_Assist_Andross: bit = 0; table = ItemSwitchTable_Assist; break; // m_andross
    case Item_Assist_Andross_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Barbara: bit = 1; table = ItemSwitchTable_Assist; break; // m_barbara
    case Item_Assist_GrayFox: bit = 2; table = ItemSwitchTable_Assist; break; // m_grayFox
    case Item_Assist_RayMKII: bit = 3; table = ItemSwitchTable_Assist; break; // m_rayMKII
    case Item_Assist_RayMKII_Bomb: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_RayMKII_Gun: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_SamuraiGoroh: bit = 4; table = ItemSwitchTable_Assist; break; // m_samuraiGoroh
    case Item_Assist_Devil: bit = 5; table = ItemSwitchTable_Assist; break; // m_devil
    case Item_Assist_Excitebike: bit = 6; table = ItemSwitchTable_Assist; break; // m_excitebike
    case Item_Assist_Jeff: bit = 7; table = ItemSwitchTable_Assist; break; // m_jeff
    case Item_Assist_Jeff_PencilBullet: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Jeff_PencilRocket: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Lakitu: bit = 8; table = ItemSwitchTable_Assist; break; // m_lakitu
    case Item_Assist_KnuckleJoe: bit = 9; table = ItemSwitchTable_Assist; break; // m_knuckleJoe
    case Item_Assist_KnuckleJoe_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_HammerBro: bit = 10; table = ItemSwitchTable_Assist; break; // m_hammerBro
    case Item_Assist_HammerBro_Hammer: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Helirin: bit = 11; table = ItemSwitchTable_Assist; break; // m_helirin
    case Item_Assist_KatAna: bit = 12; table = ItemSwitchTable_Assist; break; // m_katAna
    case Item_Assist_KatAna_Ana: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_JillDozer: bit = 13; table = ItemSwitchTable_Assist; break; // m_jillDozer
    case Item_Assist_Lyn: bit = 14; table = ItemSwitchTable_Assist; break; // m_lyn
    case Item_Assist_LittleMac: bit = 15; table = ItemSwitchTable_Assist; break; // m_littleMac
    case Item_Assist_Metroid: bit = 16; table = ItemSwitchTable_Assist; break; // m_metroid
    case Item_Assist_Nintendog: bit = 17; table = ItemSwitchTable_Assist; break; // m_nintendog
    case Item_Assist_Nintendog_Full: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_MrResetti: bit = 18; table = ItemSwitchTable_Assist; break; // m_mrResetti
    case Item_Assist_Isaac: bit = 19; table = ItemSwitchTable_Assist; break; // m_isaac
    case Item_Assist_Isaac_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Saki: bit = 20; table = ItemSwitchTable_Assist; break; // m_saki
    case Item_Assist_Saki_Shot1: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Saki_Shot2: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Shadow: bit = 21; table = ItemSwitchTable_Assist; break; // m_shadow
    case Item_Assist_War_Infantry: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_War_Infantry_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Starfy: bit = 22; table = ItemSwitchTable_Assist; break; // m_starfy
    case Item_Assist_War_Tank: bit = 23; table = ItemSwitchTable_Assist; break; // m_infantryTank
    case Item_Assist_War_Tank_Shot: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Tingle: bit = 24; table = ItemSwitchTable_Assist; break; // m_tingle
    case Item_Assist_Lakitu_Spiny: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    case Item_Assist_Waluigi: bit = 25; table = ItemSwitchTable_Assist; break; // m_waluigi
    case Item_Assist_DrWright: bit = 26; table = ItemSwitchTable_Assist; break; // m_drWright
    case Item_Assist_DrWright_Building: bit = -1; table = ItemSwitchTable_Assist; break; // no switch bit
    }
    bool enabled = true;
    if (bit != -1) {
        u32 value = 0;
        switch (table) {
        case ItemSwitchTable_Item:
            if (bit < 32) {
                value = static_cast<u32>(itemSwitch.m_item);
            } else {
                value = static_cast<u32>(itemSwitch.m_item >> 32);
                bit -= 32;
            }
            break;
        case ItemSwitchTable_Pokemon:
            value = itemSwitch.m_pokemon;
            break;
        case ItemSwitchTable_Assist:
            value = itemSwitch.m_assist;
            break;
        }
        enabled = (value & (1 << bit)) != 0;
    }
    return enabled;
}

// gmCheckGlobalItemSwitch: whether the item may appear in the match that is being set up (the item switch of the melee
// settings).
extern "C" bool fn_8005136C(itKind kind) {
    return gmCheckItemSwitch(*reinterpret_cast<gmItSwitchWords*>(&g_GameGlobal->m_modeMelee->m_meleeInitData.m_itSwitch), kind);
}

// The figure coin balance of the record (HYPOTHESIS: the coins of the coin shooter).
u32 gmGetCoinNum() {
    return *g_GameGlobal->getGlobalRecordFigCoinDatap();
}

// Sets the coin balance. HYPOTHESIS: the second counter is the highest balance reached, so it is raised when the new
// balance is above it.
u32 gmSetCoinNum(u32 num) {
    u32* coin = g_GameGlobal->getGlobalRecordFigCoinDatap();
    u32* gotCoin = g_GameGlobal->getGlobalRecordGetFigCoinDatap();
    if (num > *gotCoin) {
        *gotCoin = num;
    }
    *coin = num;
    return num;
}

// gmAddCoin: adds coins to the balance (it stops at 99999) and to the second counter (it stops at the u32 maximum).
// HYPOTHESIS: the second counter is the total of all coins gained. Callers declare this as fn_80051440.
extern "C" u32 fn_80051440(u32 num) {
    u32* coin = g_GameGlobal->getGlobalRecordFigCoinDatap();
    u32* gotCoin = g_GameGlobal->getGlobalRecordGetFigCoinDatap();
    *coin = 99999 - *coin > num ? *coin + num : 99999;
    *gotCoin = 0xFFFFFFFF - *gotCoin > num ? *gotCoin + num : 0xFFFFFFFF;
    return *coin;
}

// Takes coins off the balance; it does not go below zero.
u32 gmSubCoin(u32 num) {
    u32* coin = g_GameGlobal->getGlobalRecordFigCoinDatap();
    u32 balance = *coin;
    u32 result = balance >= num ? balance - num : 0;
    *coin = result;
    return result;
}

// How many coins a match gives (the coin shooter pays them out): a rate per match kind times the player count.
u32 gmCountMeleeCoin(int kind, int playerCount) {
    // HYPOTHESIS: one coin rate per match kind.
    float rate[8] = { 1.0f, 0.8f, 0.8f, 0.6f, 1.0f, 0.9f, 0.8f, 0.7f };
    if (playerCount < 1) {
        playerCount = 1;
    }
    float r = rate[kind];
    return 10.0f * (r * (0.11f * playerCount));
}

// HYPOTHESIS: the bytes 0x2E4 and 0x2E5 of the record are rarity bit sets for the trophy and for the sticker lottery;
// the melee settings keep a copy of each (bytes 0x60 and 0x61 of the melee data). They are not declared in the headers.
static inline u8& gmRecordFigureRareBit(gmGlobalRecord* record) {
    return reinterpret_cast<u8*>(record)[0x2E4];
}

static inline u8& gmRecordSealRareBit(gmGlobalRecord* record) {
    return reinterpret_cast<u8*>(record)[0x2E5];
}

static inline u8& gmMeleeFigureRareBit(gmGlobalModeMelee* melee) {
    return reinterpret_cast<u8*>(melee)[0x60];
}

static inline u8& gmMeleeSealRareBit(gmGlobalModeMelee* melee) {
    return reinterpret_cast<u8*>(melee)[0x61];
}

u8 gmGetRecFigureRareBit() {
    return gmRecordFigureRareBit(g_GameGlobal->getGlobalRecordGameDatap());
}

// gmSetupMeleeFigureRareBit: copies the trophy rarity bits of the record to the melee settings.
extern "C" void fn_800515E8() {
    u8 bits = gmRecordFigureRareBit(g_GameGlobal->getGlobalRecordGameDatap());
    gmMeleeFigureRareBit(g_GameGlobal->m_modeMelee) = bits;
}

void gmSetRecFigureRareBit(u8 bit) {
    gmRecordFigureRareBit(g_GameGlobal->getGlobalRecordGameDatap()) |= bit;
}

u8 gmGetRecSealRareBit() {
    return gmRecordSealRareBit(g_GameGlobal->getGlobalRecordGameDatap());
}

// gmSetupMeleeSealRareBit: the same for the sticker rarity bits.
extern "C" void fn_80051680() {
    u8 bits = gmRecordSealRareBit(g_GameGlobal->getGlobalRecordGameDatap());
    gmMeleeSealRareBit(g_GameGlobal->m_modeMelee) = bits;
}

void gmSetRecSealRareBit(u8 bit) {
    gmRecordSealRareBit(g_GameGlobal->getGlobalRecordGameDatap()) |= bit;
}

// gmLotFigure: lotteries a trophy with the rarity bits of the match.
extern "C" u16 fn_800516F0() {
    return fn_8005630C(gmMeleeFigureRareBit(g_GameGlobal->m_modeMelee));
}

u16 gmLotFigureForCoinshooter() {
    return fn_8005630C(gmMeleeFigureRareBit(g_GameGlobal->m_modeMelee) | 1);
}

u16 gmCheckFigureRarity(u32 id) {
    return gmCheckFigureRarityToCollection(id);
}

// HYPOTHESIS: rows are the sticker rarities 1-5, columns the lottery kind (the callers pass 0-2, the sticker album
// reset passes 2); the entries are chances per hundred.
u16 gmLotSealToBitset(u8 rareBits, u8 kind) {
    int chance[5][5] = {
        { 70, 43, 30, 23, 10 },
        { 30, 35, 30, 25, 15 },
        { 0, 15, 20, 25, 30 },
        { 0, 5, 15, 20, 30 },
        { 0, 2, 5, 7, 10 },
    };
    int total = 0;
    for (int i = 0; i < 5; i++) {
        total += chance[i][kind];
    }
    int roll = randi(total);
    int rarity = 0;
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += chance[i][kind];
        if (roll < sum) {
            rarity = i;
            break;
        }
    }
    return fn_80056518(rareBits, rarity + 1);
}

// gmLotSeal: lotteries a sticker with the rarity bits of the match.
extern "C" u16 fn_80051844(u8 kind) {
    return gmLotSealToBitset(gmMeleeSealRareBit(g_GameGlobal->m_modeMelee), kind);
}

// A bit of the CD (song) acquired flags or stage flags: 32 bits per word.
static inline bool gmTestFlagBit(const u32* flags, int bit) {
    return (flags[bit / 32] & (1 << (bit % 32))) != 0;
}

// HYPOTHESIS: the song table has 0x40 entries, two flag bits each in the BGM app flags (acquired: the even bit); the
// last 16 are hidden songs that belong to stages. Entry stageKeys[i - 0x30] names the stage that unlocks hidden song i
// and the stage list gives the bit of that stage in the stage app flags.
// Picks a random CD for the CD item: one that the player does not have yet; when all are there, any one.
u32 gmLotCD() {
    int allIds[102];
    int freeIds[102];
    int freeCount = 0;
    int allCount = 0;
    int* freeNext = freeIds;
    int* allNext = allIds;
    int i;
    int j;
    for (i = 0; i <= 0x2F; i++) {
        if (!gmTestFlagBit(reinterpret_cast<u32*>(g_GameGlobal->getBGMAppFlag()), i * 2)) {
            *freeNext++ = lbl_8042B858[0x15A + i];
            freeCount++;
        }
        *allNext++ = lbl_8042B858[0x15A + i];
        allCount++;
    }
    for (i = 0x30; i <= 0x3F; i++) {
        u32 available = 1;
        for (j = 0; j < 12; j++) {
            if (static_cast<int>(lbl_8042B858[0x1C0 + i - 0x30]) == static_cast<int>(lbl_8042B858[0x212 + j])) {
                available = gmTestFlagBit(g_GameGlobal->getStageAppFlag(), j);
                break;
            }
        }
        if (available == 1) {
            if (!gmTestFlagBit(reinterpret_cast<u32*>(g_GameGlobal->getBGMAppFlag()), i * 2)) {
                freeIds[freeCount++] = lbl_8042B858[0x15A + i];
            }
            allIds[allCount++] = lbl_8042B858[0x15A + i];
        }
    }
    if (freeCount != 0) {
        return freeIds[randi(freeCount)];
    }
    return allIds[randi(allCount)];
}

// Whether every CD that can be found has been acquired (hidden songs of locked stages do not count).
// gmCheckRandomCDAllAcquired
extern "C" bool fn_80051A6C() {
    int i;
    int j;
    int bit = 0;
    u32 available;
    for (i = 0; i <= 0x2F; i++, bit += 2) {
        if (!gmTestFlagBit(reinterpret_cast<u32*>(g_GameGlobal->getBGMAppFlag()), bit)) {
            return false;
        }
    }
    bit = 0x60;
    for (i = 0x30; i <= 0x3F; i++, bit += 2) {
        available = 1;
        const srStageKind* stage = lbl_8042C0A0;
        for (j = 0; j < 12; j++, stage++) {
            if (lbl_8042BF58[i - 0x30] == *stage) {
                available = gmTestFlagBit(g_GameGlobal->getStageAppFlag(), j);
                break;
            }
        }
        if (available == 1) {
            if (!gmTestFlagBit(reinterpret_cast<u32*>(g_GameGlobal->getBGMAppFlag()), bit)) {
                return false;
            }
        }
    }
    return true;
}

// HYPOTHESIS: one halfword per figure or sticker in the record (figures from 0x842, stickers from 0xD58): bit 15
// 'owned', bits 12-14 the viewed/info flags and bits 0-9 the number owned (stops at 999). This is a view of the part of
// the record that is not declared in the header yet.
struct gmRecordCollectionView {
    char _0[0x842];
    u16 m_figureFlags[0x28B];
    u16 m_sealFlags[1]; // 0xD58, more entries follow
};

static inline gmRecordCollectionView* gmRecordCollection(gmGlobalRecord* record) {
    return reinterpret_cast<gmRecordCollectionView*>(record);
}

static inline void gmSetRecordFigureRareBit(u8 bits) {
    gmRecordFigureRareBit(g_GameGlobal->getGlobalRecordGameDatap()) = bits;
}

static inline void gmSetRecordSealRareBit(u8 bits) {
    gmRecordSealRareBit(g_GameGlobal->getGlobalRecordGameDatap()) = bits;
}

// Resets the trophy collection and gives the player one random trophy.
void gmRecFigureAllInit() {
    int count = gmGetFigMaxCount();
    for (int i = 0; i < count; i++) {
        gmRecordCollection(g_GameGlobal->m_record)->m_figureFlags[i] = 0;
    }
    reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordGameDatap())[0x2E4] = 2;
    int index = gmFig_Id2Index(fn_8005630C(2));
    u16 entry = gmRecordCollection(g_GameGlobal->m_record)->m_figureFlags[index];
    int number = (entry & 0x3FF) + 1;
    if (number >= 1000) {
        number = 999;
    }
    entry = (entry & 0xF000) | 0x8000;
    entry |= number;
    gmRecordCollection(g_GameGlobal->m_record)->m_figureFlags[index] = entry;
}

// Resets the sticker collection and gives the player one random sticker.
void gmRecSealAllInit() {
    int count = gmGetSealMaxCount();
    for (int i = 0; i < count; i++) {
        gmRecordCollection(g_GameGlobal->m_record)->m_sealFlags[i] = 0;
    }
    reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordGameDatap())[0x2E5] = 4;
    int index = gmSeal_Id2Index(gmLotSealToBitset(4, 2));
    u16 entry = gmRecordCollection(g_GameGlobal->m_record)->m_sealFlags[index];
    int number = (entry & 0x3FF) + 1;
    if (number >= 1000) {
        number = 999;
    }
    entry = (entry & 0xF000) | 0x8000;
    entry |= number;
    gmRecordCollection(g_GameGlobal->m_record)->m_sealFlags[index] = entry;
}

u8 gmGetMeleeCPULevel(u8 level) {
    return lbl_8042B858[level];
}

// ---- gmlib 0x80051F28-0x80053078 ----

// HYPOTHESIS: stand-in for GameGlobal::getCharaAppFlag (sora.map 8004e580): the character unlock bitset, 3 bits per hidden fighter.

// Copy of the exact definition in gm_common.cpp (cluster A's file): used inline below.
static inline gmCharacterKind hideKindToCharacterKind(u32 hideKind) {
    for (int i = 0; i < 14; i++) {
        if (hideKind == lbl_80406968[i].m_hideKind) {
            return (gmCharacterKind)lbl_80406968[i].m_charaKind;
        }
    }
    return Character_SelectNone;
}

// HYPOTHESIS: a 64-bit set of selectable characters, bit n = gmCharacterKind n.
struct gmCharaFlag {
    u32 m_bits[2];

    void set(int kind) {
        m_bits[kind / 32] |= 1 << (kind % 32);
    }
    void clear(int kind) {
        m_bits[kind / 32] &= ~(1 << (kind % 32));
    }
    void setGlobalCharaData();
};

// Builds the set of fighters that may be picked: every base fighter, minus the hidden fighters that are still locked.
void gmCharaFlag::setGlobalCharaData() {
    m_bits[0] = 0;
    m_bits[1] = 0;
    for (int i = 0; i < 50; i++) {
        set(i);
    }
    u32* unlocked = g_GameGlobal->getCharaAppFlag();
    for (int i = 0; i < 14; i++) {
        int bit = 1 + i * 3;
        if (!(unlocked[bit / 32] & (1 << (bit % 32)))) {
            clear(hideKindToCharacterKind((u8)i));
        }
    }
}

// Not renamed: muWifiInterfaceTask::getComposedCharaAcceptFlag calls it as fn_80052250.
extern "C" gmCharaFlag fn_80052250() {
    gmCharaFlag flag;
    flag.setGlobalCharaData();
    return flag;
}

// HYPOTHESIS: the set is passed as one 64-bit value (r3/r4 pair), as muWifiInterfaceTask's composed accept flag is.
bool gmCheckCharaAccept(u64 flag, int kind) {
    union {
        u64 whole;
        u32 words[2];
    } set;
    set.whole = flag;
    return (set.words[kind / 32] & (1 << (kind % 32))) != 0;
}

// HYPOTHESIS: turns an all-star list into a corps list. Byte 4 == 1 marks an all-star setup whose first player
// record carries the fighter count in its stock field; the first player's fighter/cpu/reaction data moves to the
// corps header, the per-player costume ids are packed into the byte array at 0x24 and repeated up to the count.
void gmGlobalCorpsConvertAllStarToCorps(gmGlobalCorps* corps) {
    u8* raw = reinterpret_cast<u8*>(corps);
    if (raw[0x4] == 1) {
        int count = 100;
        if (corps->m_playersInitData[0].m_stockCount < 100) {
            count = corps->m_playersInitData[0].m_stockCount;
        }
        if (count > 1) {
            int period = raw[0x1];
            u8 kind = raw[0x24];
            u8 cpuType = raw[0x27];
            u8 cpuRank = raw[0x28];
            float attackMul = corps->m_playersInitData[0].m_attackReactionMul;
            float damageMul = corps->m_playersInitData[0].m_damageReactionMul;
            float scale = corps->m_playersInitData[0].m_scale;
            raw[0x4] = 0;
            raw[0xA] = kind;
            raw[0xB] = cpuType;
            raw[0xC] = cpuRank;
            *reinterpret_cast<float*>(raw + 0x10) = attackMul;
            *reinterpret_cast<float*>(raw + 0x14) = damageMul;
            *reinterpret_cast<float*>(raw + 0x18) = scale;
            int packed = count;
            if (period < count) {
                packed = period;
            }
            int n = 0;
            gmPlayerCorpsInitData* src = corps->m_playersInitData;
            while (n < packed) {
                raw[0x24 + n] = src->m_costumeId;
                n++;
                src++;
            }
            for (int i = period; i < count; i++) {
                raw[0x24 + i] = raw[0x24 + i % period];
            }
            raw[0x5] |= 0x40;
            raw[0x1] = count;
            raw[0x3] = count;
        }
    }
}

// HYPOTHESIS: byte 0x808 of the global record bit 7 and byte 0x62 of the melee init data bit 7 mark Risetsan use,
// byte 0x62 bit 6 marks Dragoon use, byte 0x28 of gmAppData bit 7 is the Dragoon use flag.
struct gmRecordUseFlags {
    u8 m_risetsan : 1;
    u8 : 7;
};
struct gmMeleeUseFlags {
    u8 m_risetsan : 1;
    u8 m_dragoon : 1;
    u8 : 6;
};
struct gmAppUseFlags {
    u8 m_dragoon : 1;
    u8 : 7;
};

void gmSetUseRisetsan() {
    bool on = true;
    reinterpret_cast<gmRecordUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordGameDatap()) + 0x808)->m_risetsan = on;
    reinterpret_cast<gmMeleeUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0x62)->m_risetsan = 1;
}

// Not renamed: it_manager.cpp calls it as fn_80052694.
extern "C" void fn_80052694() {
    reinterpret_cast<gmAppUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_appData) + 0x28)->m_dragoon = 1;
}

void gmResetUseDragoon() {
    reinterpret_cast<gmAppUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_appData) + 0x28)->m_dragoon = 0;
}

bool gmCheckRisetsan() {
    return reinterpret_cast<gmMeleeUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0x62)->m_risetsan == 1;
}

// Not renamed: it_manager.cpp calls it as fn_800526E4.
extern "C" bool fn_800526E4() {
    return reinterpret_cast<gmMeleeUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0x62)->m_dragoon == 1;
}

// HYPOTHESIS: the figure collection is a halfword array at gmGlobalRecord+0x842 (stickers: +0xD58), one entry per
// collection index: bit 15 owned, bit 14 referred to, bit 13 info seen, bit 12 info-ref seen, low 10 bits the
// count owned (capped at 999).
#define GM_COLLECT_OWNED 0x8000
#define GM_COLLECT_REFER 0x4000
#define GM_COLLECT_INFO 0x2000
#define GM_COLLECT_INFOREF 0x1000
#define GM_COLLECT_FLAGS 0xF000
#define GM_COLLECT_COUNT 0x03FF

static inline u16* figureTable() {
    return reinterpret_cast<u16*>(reinterpret_cast<u8*>(g_GameGlobal->m_record) + 0x842);
}

static inline u16* sealTable() {
    return reinterpret_cast<u16*>(reinterpret_cast<u8*>(g_GameGlobal->m_record) + 0xD58);
}

// MATCH-ONLY: the record writers keep the table base (record + offset) in its own register; this needs
// opt_propagation off (the readers further down do not).
#pragma push
#pragma opt_propagation off

// Not renamed: ad_keep_manager.cpp calls it as fn_80052704.
extern "C" void fn_80052704(int id, int num) {
    int index = gmFig_Id2Index(id);
    u16* table = figureTable();
    u16 entry = table[index];
    num += entry & GM_COLLECT_COUNT;
    if (num >= 1000) {
        num = 999;
    }
    u16 flags = entry & GM_COLLECT_FLAGS;
    flags |= GM_COLLECT_OWNED;
    table[index] = flags | num;
}

int gmCheckCountFigure(int id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return *reinterpret_cast<u16*>(record + 0x842 + index * 2) & GM_COLLECT_COUNT;
}

// Copy of the exact definition in gm_common.cpp (cluster A's file); the owned flag is bit 15.
bool gmCheckExistFigure(u16 id) {
    s32 index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    // HYPOTHESIS: unk842 is a u16 table in gmGlobalRecord indexed by figure index; bit 15 is the existence flag.
    u16 flags = *reinterpret_cast<u16*>(record + 0x842 + index * 2);
    return (flags >> 15) & 1;
}

bool gmCheckReferFigure(int id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0x842 + index * 2) >> 14) & 1;
}

bool gmCheckInfoFigure(int id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0x842 + index * 2) >> 13) & 1;
}

bool gmCheckInfoRefFigure(int id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0x842 + index * 2) >> 12) & 1;
}

void gmSetFlagReferFigure(int id, u32 flag) {
    int index = gmFig_Id2Index(id);
    u16* table = figureTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_REFER;
    } else {
        entry &= ~GM_COLLECT_REFER;
    }
    table[index] = entry;
}

void gmSetFlagInfoFigure(int id, u32 flag) {
    int index = gmFig_Id2Index(id);
    u16* table = figureTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_INFO;
    } else {
        entry &= ~GM_COLLECT_INFO;
    }
    table[index] = entry;
}

void gmSetFlagInfoRefFigure(int id, u32 flag) {
    int index = gmFig_Id2Index(id);
    u16* table = figureTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_INFOREF;
    } else {
        entry &= ~GM_COLLECT_INFOREF;
    }
    table[index] = entry;
}

#pragma pop

// MATCH-ONLY: the counting loops test the owned bit directly instead of calling gmCheckExistFigure/Seal.
static inline bool figureOwned(u16 id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0x842 + index * 2) & GM_COLLECT_OWNED) != 0;
}

static inline bool sealOwned(u16 id) {
    int index = gmSeal_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0xD58 + index * 2) & GM_COLLECT_OWNED) != 0;
}

// Number of figures the player owns.
int gmGetKindCountFigureExist() {
    int total = 0;
    int max = gmGetFigMaxCount();
    for (int i = 0; i < max; i++) {
        int id = gmFig_Index2Id(i);
        if (id >= 0 && figureOwned((u16)id)) {
            total++;
        }
    }
    return total;
}

// True when every figure of the given rarity class is owned.
bool gmCheckAllFigureExistRare(u32 rarity) {
    int max = gmGetFigMaxCount();
    for (int i = 0; i < max; i++) {
        int id = gmFig_Index2Id(i);
        if (id >= 0 && rarity == gmCheckFigureRarityToCollection((u16)id) && !figureOwned((u16)id)) {
            return false;
        }
    }
    return true;
}

#pragma push
#pragma opt_propagation off

// Not renamed: ad_keep_manager.cpp calls it as fn_80052AC0.
extern "C" void fn_80052AC0(int id, int num) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    num += entry & GM_COLLECT_COUNT;
    if (num >= 1000) {
        num = 999;
    }
    u16 flags = entry & GM_COLLECT_FLAGS;
    flags |= GM_COLLECT_OWNED;
    table[index] = flags | num;
}

bool gmCheckExistSeal(int id) {
    int index = gmSeal_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0xD58 + index * 2) >> 15) & 1;
}

bool gmCheckInfoSeal(int id) {
    int index = gmSeal_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0xD58 + index * 2) >> 13) & 1;
}

bool gmCheckInfoRefSeal(int id) {
    int index = gmSeal_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0xD58 + index * 2) >> 12) & 1;
}

void gmSetFlagCountSeal(int id, u32 count) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    if (count > 1000) {
        count = 999;
    }
    table[index] = count | (entry & GM_COLLECT_FLAGS);
}

void gmSetFlagReferSeal(int id, u32 flag) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_REFER;
    } else {
        entry &= ~GM_COLLECT_REFER;
    }
    table[index] = entry;
}

void gmSetFlagInfoSeal(int id, u32 flag) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_INFO;
    } else {
        entry &= ~GM_COLLECT_INFO;
    }
    table[index] = entry;
}

void gmSetFlagInfoRefSeal(int id, u32 flag) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_INFOREF;
    } else {
        entry &= ~GM_COLLECT_INFOREF;
    }
    table[index] = entry;
}

#pragma pop


// Number of stickers the player owns.
int gmGetKindCountSealExist() {
    int total = 0;
    int max = gmGetSealMaxCount();
    for (int i = 0; i < max; i++) {
        int id = gmSeal_Index2Id(i);
        if (id >= 0 && sealOwned((u16)id)) {
            total++;
        }
    }
    return total;
}

// HYPOTHESIS: the name comes from the map; the value is a word of the set-rule record at 0x24.
u32 gmGetMenuDecisionPad() {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(g_GameGlobal->m_setRule) + 0x24);
}

// Copy of the exact definition in gm_common.cpp (cluster A's file).
int gmGetFigureIDfromChKind(int kind) {
    switch (kind) {
    default:
        return 1;
    case 0x1:
        return 0x3;
    case 0x2:
        return 0x5;
    case 0x3:
    case 0x4:
        return 0x7;
    case 0x5:
        return 0x9;
    case 0x6:
        return 0xB;
    case 0x7:
        return 0xD;
    case 0x8:
        return 0xF;
    case 0x9:
        return 0x11;
    case 0xA:
        return 0x13;
    case 0xB:
        return 0x15;
    case 0xC:
        return 0x17;
    case 0xD:
        return 0x19;
    case 0xE:
        return 0x1B;
    case 0xF:
        return 0x1D;
    case 0x10:
    case 0x11:
    case 0x12:
        return 0x1F;
    case 0x13:
        return 0x21;
    case 0x14:
        return 0x23;
    case 0x15:
        return 0x25;
    case 0x16:
        return 0x27;
    case 0x17:
        return 0x29;
    case 0x18:
        return 0x2B;
    case 0x19:
        return 0x2D;
    case 0x1A:
        return 0x31;
    case 0x1B:
        return 0x3D;
    case 0x1C:
        return 0x3F;
    case 0x1D:
    case 0x1E:
        return 0x43;
    case 0x1F:
    case 0x20:
        return 0x45;
    case 0x21:
    case 0x22:
        return 0x47;
    case 0x23:
        return 0x49;
    case 0x24:
        return 0x4B;
    case 0x25:
        return 0x4D;
    case 0x26:
        return 0x4F;
    case 0x27:
        return 0x53;
    case 0x28:
        return 0x55;
    case 0x29:
        return 0x57;
    case 0x2A:
        return 0x59;
    case 0x2B:
        return 0x5B;
    case 0x2C:
        return 0x68;
    case 0x2D:
        return 0x6F;
    case 0x2E:
        return 0x25A;
    case 0x2F:
        return 0x25B;
    case 0x30:
        return 0x25C;
    case 0x31:
        return 0x25D;
    case 0x33:
        return 0x17F;
    case 0x34:
        return 0x180;
    case 0x35:
        return 0x259;
    case 0x36:
        return 0x181;
    case 0x38:
        return 0x184;
    case 0x39:
        return 0x185;
    case 0x3A:
        return 0x186;
    case 0x3B:
        return 0x188;
    case 0x3C:
        return 0x189;
    case 0x3D:
        return 0x18A;
    case 0x37:
        // HYPOTHESIS: kind 0x37 has no figure of its own; the original returns the kind unchanged.
        return kind;
    }
}

// HYPOTHESIS: the net rule backup keeps the first rule bytes of the set-rule record (rule kind, time, stock,
// handicap, damage ratio, stage choice, then stock time, team attack, pause, score display, damage gauge) in two
// buffers owned by GameGlobal (pointers at 0x48 and 0x4C); the Reset function restores them.
static inline void saveRuleBytes(u8* rule, u8* backup, int first, int count) {
    for (int i = 0; i < count; i++) {
        backup[i] = rule[first + i];
    }
}

void gmRuleBackupForNet() {
    saveRuleBytes(reinterpret_cast<u8*>(g_GameGlobal->m_setRule), *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x48), 2, 6);
    saveRuleBytes(reinterpret_cast<u8*>(g_GameGlobal->m_setRule), *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x4C), 8, 5);
}

void gmRuleResetForNet() {
    u8* backup = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x48);
    u8* rule = reinterpret_cast<u8*>(g_GameGlobal->m_setRule);
    rule[2] = backup[0];
    rule[3] = backup[1];
    rule[4] = backup[2];
    rule[5] = backup[3];
    rule[6] = backup[4];
    rule[7] = backup[5];
    backup = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x4C);
    rule = reinterpret_cast<u8*>(g_GameGlobal->m_setRule);
    rule[8] = backup[0];
    rule[9] = backup[1];
    rule[10] = backup[2];
    rule[11] = backup[3];
    rule[12] = backup[4];
}

// ---- gmlib 0x80053078-0x80053F4C ----

// Callees owned by other clusters of this temporarily split unit.
// HYPOTHESIS: signatures come from the register use at the call sites; the names are from the sora map.
// gmResultInfo::gmSetSelStagePlayerModeResult(gmResultInfo*, gmGlobalModeMelee*)
extern "C" void fn_800555D8(gmResultInfo* result, gmGlobalModeMelee* melee);
// gmResultInfo::gmAddDiagramResultChara()
extern "C" void fn_80055058(gmResultInfo* result);
// gmResultInfo::gmAddDiagramResultName()
extern "C" void fn_800551C8(gmResultInfo* result);
// GameGlobal::getVsModeAutoHandiPlayerVal(int player, u8 ?): pointer to the player's auto handicap value
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

u32 gmGetFriendCount() {
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
                coins += gmCountMeleeCoin(humans + 3, view[i].m_unk250);
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
            handi[i] = g_GameGlobal->getVsModeAutoHandiPlayerVal(i, view[i].m_unk29);
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

// ---- gmlib 0x80053F4C-0x800568C8 ----

// Cluster E of the temporary gm_common split (0x80053F4C-0x800568C8): last-place and diagram bookkeeping of the
// melee result, the stage choice rule, event clear queries, the item switch tables and the figure and sticker lotteries.

// Saturating add as the game inlines it (the out-of-line copy is fn_8004F258 in the previous unit): the sum is
// clamped to max, and an addend above max clamps too.
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
                u16* beaten = chara->m_beatCount;
                addMaxCheck(&beaten[(u8)exchangeGmCharacterKindToMuCharKind__6muMenuFiii(playerStat<u8>(other, 0))],
                            player->m_playerBeatCounts[j], 0xFFFF);
            }
            addMaxCheck(&chara->m_fallCount, other->m_playerBeatCounts[i], 0xFFFF);
        }
        gmAddDiagramResultCommon(result, i, &chara->m_diagram);
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
struct gmItSwitchData {
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
extern gmItSwitchData lbl_804977E0;

// HYPOTHESIS: each table holds one default item switch (16 bytes) per stage / event / challenge.
extern const gmItSwitchData lbl_8042C4B8[43];
extern const gmItSwitchData lbl_8042C768[12];
extern const gmItSwitchData lbl_8042C828[19];
extern const gmItSwitchData lbl_8042C958[51];
extern const gmItSwitchData lbl_8042CC88[51];
extern const gmItSwitchData lbl_8042CFB8[14];
extern const gmItSwitchData lbl_8042D098[1];

// Takes the unlocked state into account: the team healer needs a team battle, and each assist trophy needs its unlock.
static inline void gmRestrictItemSwitch(gmItSwitchData* data) {
    if (g_GameGlobal->m_modeMelee->m_meleeInitData.m_isTeams != 1) {
        data->m_item[0] &= ~ITEM_TEAM_HEALER;
    }
    if (!gmCheckAssistKindUseEnable(0)) {
        data->m_assist &= ~ASSIST_RAY_MKII;
    }
    if (!gmCheckAssistKindUseEnable(1)) {
        data->m_assist &= ~ASSIST_ISAAC;
    }
    if (!gmCheckAssistKindUseEnable(2)) {
        data->m_assist &= ~ASSIST_INFANTRY_TANK;
    }
    if (!gmCheckAssistKindUseEnable(3)) {
        data->m_assist &= ~ASSIST_BARBARA;
    }
    if (!gmCheckAssistKindUseEnable(4)) {
        data->m_assist &= ~ASSIST_GRAY_FOX;
    }
    if (!gmCheckAssistKindUseEnable(5)) {
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
    gmItSwitchData* data = &lbl_804977E0;
    *data = lbl_8042C4B8[index];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

gmItSwitch* gmGetItemSwitchData_Simple(int kind) {
    // HYPOTHESIS: maps the simple mode kind to its row of the table
    int rows[14] = { 0, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 10, 11 };
    gmItSwitchData* data = &lbl_804977E0;
    *data = lbl_8042C768[rows[kind]];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

gmItSwitch* gmGetItemSwitchData_Allstar(int stage) {
    gmItSwitchData* data = &lbl_804977E0;
    *data = lbl_8042C828[stage];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

// kind 1 = solo events, 2 = co-op events
gmItSwitch* gmGetItemSwitchData_Event(int event, int kind) {
    gmItSwitchData* data = &lbl_804977E0;
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
    gmItSwitchData* data = &lbl_804977E0;
    *data = lbl_8042CFB8[challenger];
    gmRestrictItemSwitch(data);
    return reinterpret_cast<gmItSwitch*>(data);
}

gmItSwitch* gmGetItemSwitchData_Kumite() {
    gmItSwitchData* data = &lbl_804977E0;
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

// BYTES: gm_data_collection.o was its own translation unit in the original, so gm_lib.o calls these out of line.
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

