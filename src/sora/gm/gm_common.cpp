#include <types.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <string.h>
#include <stdio.h>

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
extern "C" u32* fn_8004E580(GameGlobal* global); // GameGlobal::getCharaAppFlag (also used by mu_menu.cpp, st_croll.cpp)
extern "C" u32* fn_8004E594(GameGlobal* global); // GameGlobal::getStageAppFlag (also used by mu_proc_stage_switch.cpp)
extern "C" u32* fn_8004E5A0(GameGlobal* global); // GameGlobal::getAssistAppFlag (also used by st_croll.cpp)

// The menu's fighter numbering: muMenu::exchangeGmCharacterKindToMuCharKind (mu_menu.cpp). The native callers pass the
// fighter in r3 only (the mangled name says three ints), so this stand-in is declared with one parameter.
// HYPOTHESIS: MATCH-ONLY declaration; the real header declaration is unknown.
extern "C" int exchangeGmCharacterKindToMuCharKind__6muMenuFiii(int kind);

// Callees that belong to other units / clusters (names from the sora map).
// HYPOTHESIS: parameter and return types come from the call sites only.
extern "C" void fn_8004D674(GameGlobal* global, int nameNo, wchar_t* buffer); // the player name tag as UTF-16
extern "C" u16* fn_8004DA14(GameGlobal* global, int slot, int nameNo);        // the handicap stored for a name tag, 0 if none
extern "C" gmCharaFlagWords fn_80051F28();                                    // gmCharaFlag::setGlobalCharaData

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

u8 gmRetHideMovieID(GmMovieID id) {
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
    return gmCheckBit(fn_8004E5A0(g_GameGlobal), kind);
}

void gmInitAssistKindUseEnable() {
    u32* flags = fn_8004E5A0(g_GameGlobal);
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
    u32* bits = fn_8004E580(g_GameGlobal);
    u8 hide = gmGetHideKindFromChKind(kind);
    if (hide == 14 || gmCheckBit(bits, hide * 3 + 1)) {
        return true;
    }
    return false;
}

void gmSetChKindUseEnable(gmCharacterKind kind) {
    u32* bits = fn_8004E580(g_GameGlobal);
    u8 hide = gmGetHideKindFromChKind(kind);
    if (hide != 14) {
        gmSetBit(bits, hide * 3 + 1);
    }
}

bool gmCheckChKindUseEnableAll() {
    u32* bits = fn_8004E580(g_GameGlobal);
    for (int i = 0; i < 14; i++) {
        if (!gmCheckBit(bits, i * 3 + 1)) {
            return false;
        }
    }
    return true;
}

void gmInitChKindUseEnableAll() {
    u32* bits = fn_8004E580(g_GameGlobal);
    bits[0] = 0;
    bits[1] = 0;
}

// A stage in the table of lockable stages owns the bit of its table index in the stage bit set.
// A stage that is not in the table is always available.
bool gmCheckSrStageKind(srStageKind stage) {
    bool usable = true;
    for (int i = 0; i < 12; i++) {
        if (stage == lbl_8042C0A0[i]) {
            usable = gmCheckBit(fn_8004E594(g_GameGlobal), i);
            break;
        }
    }
    return usable;
}

void gmSetStageUseEnable(srStageKind stage) {
    for (int i = 0; i < 12; i++) {
        if (stage == lbl_8042C0A0[i]) {
            gmSetBit(fn_8004E594(g_GameGlobal), i);
        }
    }
}

void gmInitStageUseEnableAll() {
    u32* flags = fn_8004E594(g_GameGlobal);
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
    u8 noName = 0x78;
    // MATCH-ONLY: the native code stores the fighter slot as a plain byte, not as an enum bit field.
    *reinterpret_cast<u8*>(player) = Character_SelectNone;
    player->m_state = 3;
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
    player->m_playerId = 0;
    player->m_startPointIdx = 0;
    player->m_controllerNo = 0;
    player->m_nameIndex = noName;
}

// HYPOTHESIS: the defaults of one match: 2 minute time limit, four players, all items on, medium item frequency.
void gmInitMeleeDataDefault(gmMeleeInitData* melee) {
    memset(melee, 0, sizeof(gmMeleeInitData));
    melee->m_gameRule = Game_Rule_Time;
    melee->m_numPlayers = 4;
    melee->m_0x1_1 = true;
    melee->m_0x1_0 = false;
    melee->m_0x2_7 = true;
    melee->m_0x2_1 = false;
    melee->m_allowPause = true;
    melee->m_0x3_2 = true;
    melee->m_hideDamageGauge = false;
    melee->m_0x3_0 = false;
    melee->m_scoreDisplay = false;
    melee->m_0x5_5 = false;
    melee->m_0x5_4 = false;
    melee->m_0x5_3 = false;
    melee->m_timeLimitFrames = 7200;
    melee->m_isTeams = false;
    melee->m_stageKind = 0;
    melee->m_subStageKind = 0;
    melee->m_damageRatio = 1.0f;
    melee->m_suicideScoreMultiplier = -1;
    melee->m_gameSpeed = 1.0f;
    melee->m_corpsKind = Corps_None;
    // MATCH-ONLY: the native code sets the three item switches as one doubleword and two words.
    *reinterpret_cast<u64*>(&melee->m_itSwitch.m_item) = 0xFFFFFFFFFFFFFFFFULL;
    *reinterpret_cast<u32*>(&melee->m_itSwitch.m_pokemon) = 0xFFFFFFFF;
    *reinterpret_cast<u32*>(&melee->m_itSwitch.m_assist) = 0xFFFFFFFF;
    melee->m_charaFlags = fn_80051F28();
    melee->m_itemFrequency = gmItSwitch::Frequency_Medium;
    melee->m_0x10 = 0x78;
    melee->m_cameraShakeScale = 1.0f;
    GameGlobal* global = g_GameGlobal;
    melee->m_0x5a_7 = ((gmGlobalRecord*)global->getGlobalRecordGameDatap())->m_unk808 >> 7;
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
        player->m_cpuType = 0;
        player->m_cpuRank = lbl_8042B858[select->m_playersInitData[i].m_cpuRank];
        player->m_playerId = i;
        player->m_startPointIdx = i;
        player->m_controllerNo = select->m_playersInitData[i].m_controllerNo;
        player->m_nameIndex = select->m_playersInitData[i].m_nameIndex;
        if (player->m_nameIndex != 0x78) {
            fn_8004D674(g_GameGlobal, player->m_nameIndex, player->m_name);
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
        if (select->m_nameIndex != 0x78) {
            u16* handicap = fn_8004DA14(g_GameGlobal, i, select->m_nameIndex);
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
    gmGlobalRecord::MenuData* menu = g_GameGlobal->getGlobalRecordMenuDatap();
    gmSetRule* rule = g_GameGlobal->m_setRule;
    gmPlayerInitData* select = g_GameGlobal->m_selCharData->m_playersInitData;
    gmMeleeInitData* melee = &global->m_meleeInitData;
    melee->m_gameRule = rule->m_rule;
    switch (melee->m_gameRule) {
    case Game_Rule_Stock:
        melee->m_0x1_1 = true;
        melee->m_timeLimitFrames = rule->m_stockTimeMinutes * 3600;
        break;
    case Game_Rule_Coin:
        melee->m_0x1_1 = true;
        melee->m_timeLimitFrames = rule->m_timeMinutes * 3600;
        break;
    default:
        melee->m_0x1_1 = true;
        melee->m_timeLimitFrames = rule->m_timeMinutes * 3600;
        break;
    }
    melee->m_0x2_7 = melee->m_timeLimitFrames != 0;
    for (int i = 0; i < 4; i++) {
        gmPlayerInitData* player = &global->m_playersInitData[i];
        switch (rule->m_handicap) {
        case 1: {
            u16* handicap = fn_8004DA14(g_GameGlobal, i, select->m_nameIndex);
            player->m_handicap = handicap != nullptr ? *handicap : 0;
            break;
        }
        case 2:
            player->m_handicap = select->m_handicap;
            break;
        default:
            player->m_handicap = 0;
            break;
        }
        player->m_startDamage = player->m_handicap;
        select++;
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
    case Game_Rule_Stock:
        melee->m_0x3_2 = false;
        melee->m_scoreDisplay = true;
        break;
    case Game_Rule_Coin:
        melee->m_0x3_2 = true;
        melee->m_scoreDisplay = true;
        break;
    default:
        melee->m_0x3_2 = true;
        melee->m_scoreDisplay = rule->m_showScoreDisplay;
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

u32 gmCountMeleeCoin(int kind, int playerCount) {
    // HYPOTHESIS: one coin rate per match kind.
    float rate[8] = { 1.0f, 0.8f, 0.8f, 0.6f, 1.0f, 0.9f, 0.8f, 0.7f };
    if (playerCount < 1) {
        playerCount = 1;
    }
    float r = rate[kind];
    return 10.0f * (r * (0.11f * playerCount));
}

u16 gmCheckFigureRarity(u32 id) {
    return gmCheckFigureRarityToCollection(id);
}

u8 gmGetMeleeCPULevel(u8 level) {
    return lbl_8042B858[level];
}

bool gmCheckExistFigure(u16 id) {
    s32 index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    // HYPOTHESIS: unk842 is a u16 table in gmGlobalRecord indexed by figure index; bit 15 is the existence flag.
    u16 flags = *reinterpret_cast<u16*>(record + 0x842 + index * 2);
    return (flags >> 15) & 1;
}

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

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804977E0[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804977F0[0x10] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A00E8[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A00EC[0x4] __attribute__((aligned(4))) = { 0 };

// The function body below is taken from https://github.com/jj-link/brawl.

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
