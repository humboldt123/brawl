#include <types.h>
#include <gm/gm_global.h>
#include <revolution/OS/OSTime.h>
#include <revolution/SC.h>
#include <string.h>
#include <gf/gf_camera.h>
#include <gf/gf_video.h>
#include <gm/gm_common.h>
#include <mu/mu_tour_table.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>

// HYPOTHESIS: m_appData points at the frame counter block that updateGameFrame writes; unk4 is the frame count, unk8 the frame divisor with unk9 its reload value, and unkA the position within the current frame
struct GameFrameState {
    u8 unk0;
    char unk1[0x3];
    u32 unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    char unkB;
    f32 unkC;
    u8 unk10;       // HYPOTHESIS: bit n = file n of the save data may be read and written (get/setEnableSaveLoad)
    char unk11[0x3];
    s32 unk14;
    u8 unk18;
    char unk19[0x7];
    s64 unk20;
};

// HYPOTHESIS: stand-ins for callees that are not named yet. fn_8001B784 (8 bytes) hands the language to the file system
// code (map: gfFileIO::setLanguage), fn_80076F30 is sndSystem::setVolBalance (music, sound effects), fn_80078768 is
// sndBgmRateSystem::setupBGMRate.
extern "C" void fn_8001B784(int language);
extern "C" void fn_80076F30(sndSystem* system, f32 bgm, f32 se);
extern "C" void fn_80078768(sndBgmRateSystem* system);
// Message::utf8to16 / Message::utf16to8 (the message header does not mix with the other includes of this unit)
extern "C" u32 utf8to16__7MessageFPwPCc(wchar_t* dst, const char* src);
extern "C" u32 utf16to8__7MessageFPcPCw(char* dst, const wchar_t* src);
void addMaxCheck(u32* dst, u32 add, u32 max);
u32 gmSetCoinNum(u32 num);
void gmRecFigureAllInit();
void gmRecSealAllInit();

// HYPOTHESIS: gmPlayerResultInfo as the original wrote it for the placeholder data: plain bytes where the header has a
// bit field, and the part behind m_lostCoins that has no names yet. The roles of the arrays come from their sizes only.
struct gmResultPlayerDraft {
    u8 m_characterKind;  // 0x00
    u8 m_state;          // 0x01
    s8 m_colorFileIdx;   // 0x02
    char _3[2];
    u8 unk5;             // 0x05 (copy of the name index of the init data)
    u8 m_teamNo;         // 0x06
    bool unk7_7 : 1;     // 0x07
    bool unk7_6 : 1;     // HYPOTHESIS: copy of a flag of the player's init data (bit 6 of its byte 0x1B)
    bool m_isStamina : 1; // HYPOTHESIS: copy of the stamina flag of the player's init data
    bool unk7_4 : 1;
    bool unk7_3 : 1;
    bool unk7_2 : 1;
    bool unk7_1 : 1;
    bool unk7_0 : 1;
    u16 m_hitPointMax;   // 0x08 (the header has it signed; the original copies it as an unsigned halfword)
    u8 m_startStockCount; // 0x0A
    char _B;
    u16 m_startDamage;   // 0x0C
    u8 m_rank;           // 0x0E
    char _F;
    u32 m_koCount;       // 0x10
    u32 m_deadCount;     // 0x14
    u16 m_suicideCount;  // 0x18
    char _1A[0x12];
    u32 m_pickupCoins;   // 0x2C
    u32 m_lostCoins;     // 0x30
    u8 koTarget[100];    // 0x34 HYPOTHESIS: one entry per KO (m_koCount of them): the player that was knocked out
    u8 koCause[100];     // 0x98
    u8 deadTarget[100];  // 0xFC HYPOTHESIS: one entry per death (m_deadCount of them)
    u8 deadCause[100];   // 0x160
    char _1C4[8];
    u32 unk1CC[8];       // 0x1CC
    u8 unk1EC;
    char _1ED[3];
    u32 unk1F0[16];      // 0x1F0
    char _230[4];
    u32 unk234;
    u32 unk238;
    u32 unk23C;
    u32 unk240;
    u32 unk244;
    char _248[0x64];
};
static_assert(sizeof(gmResultPlayerDraft) == sizeof(gmPlayerResultInfo), "Class is wrong size!");

// HYPOTHESIS: the bytes of gmResultInfo that the header has as padding or as bit fields (the original writes them as
// plain bytes).
struct gmResultInfoDraft {
    char _0;
    u8 m_gameRule;               // 0x01
    u8 m_isTeams;                // 0x02
    s8 m_suicideScoreMultiplier; // 0x03
    u32 m_timeMinutes;           // 0x04
    u32 m_stockCount;            // 0x08
    char _C[2];
    u8 unkE_7 : 1;               // 0x0E
    u8 unkE_rest : 7;
    u8 m_numWinners;             // 0x0F
    u8 unk10;
    u8 unk11;
    u8 unk12;
    u8 unk13;                    // index of the first entry of the placeholder order that is zero
    char _14[6];
    u8 unk1A;
    char _1B[0x134D];
    u32 unk1368;
    u8 unk136C;
    char _136D[3];
    u32 unk1370;
    u32 unk1374;
    u8 m_decisionKind;           // 0x1378
    char _1379[0xF];
};
static_assert(sizeof(gmResultInfoDraft) == sizeof(gmResultInfo), "Class is wrong size!");

// The bytes of a player's init data that the result record copies, as plain bytes and unsigned halfwords (the header has
// the first as a bit field and the halfwords signed).
struct gmPlayerInitBytes {
    u8 m_characterKind;  // 0x00
    u8 m_state;          // 0x01
    char _2[2];
    u8 m_stockCount;     // 0x04
    u8 m_colorFileNo;    // 0x05
    char _6[5];
    u8 m_teamNo;         // 0x0B
    char _C[0xC];
    u8 m_nameIndex;      // 0x18
    char _19[2];
    bool unk1B_7 : 1;    // 0x1B
    bool m_0x1B_6 : 1;
    bool unk1B_5 : 1;
    bool unk1B_4 : 1;
    bool unk1B_3 : 1;
    bool unk1B_2 : 1;
    bool unk1B_1 : 1;
    bool unk1B_0 : 1;
    bool m_isStamina : 1; // 0x1C
    bool unk1C_6 : 1;
    bool unk1C_5 : 1;
    bool unk1C_4 : 1;
    bool unk1C_3 : 1;
    bool unk1C_2 : 1;
    bool unk1C_1 : 1;
    bool unk1C_0 : 1;
    char _1D[5];
    u16 m_startDamage;   // 0x22
    u16 m_hitPointMax;   // 0x24
    char _26[0x36];
};
static_assert(sizeof(gmPlayerInitBytes) == sizeof(gmPlayerInitData), "Class is wrong size!");

// MATCH-ONLY: GameGlobal::getGlobalRecordNameDatap lives in the neighbouring unit (gm_global.cpp) while the original
// object inlined it here, so this static twin stands in for the inline body.
static inline gmGlobalRecord::NameData* nameRecord(GameGlobal* global, u8 index) {
    return &global->m_nameRecords[index];
}

// The saved name of a record (12 bytes of UTF-16) and its UTF-8 form. The original inlined GameGlobal::getNameUTF16 and
// GameGlobal::getName (cluster A) here.
// MATCH-ONLY: file-local twins of those inlines.
static inline bool nameUTF16(GameGlobal* global, int index, wchar_t* dst) {
    gmGlobalRecord::NameData* name = &global->m_nameRecords[index];
    if (name->unkE0 == 0) {
        return false;
    }
    memcpy(dst, &name->unkE0, 12);
    return true;
}

static inline void nameUTF8(GameGlobal* global, int index, char* dst) {
    wchar_t name[6];
    if (nameUTF16(global, index, name)) {
        utf16to8__7MessageFPcPCw(dst, name);
    }
}

static inline void gmResetBit(u32* bits, int bit) {
    bits[bit / 32] &= ~(1 << (bit % 32));
}

static inline gmResultInfoDraft* draftOf(gmResultInfo* result) {
    return reinterpret_cast<gmResultInfoDraft*>(result);
}

// HYPOTHESIS: fills the result record with fixed placeholder values (settings of the running melee, then the same
// made-up KO lists, ranks and counters for each of the seven players), the way the result screen can be shown without a match.
void GameGlobal::initResultInfoData() {
    gmMeleeInitData* melee = &m_modeMelee->m_meleeInitData;
    u8 order[4] = { 0, 1, 2, 3 }; // HYPOTHESIS: indexed by the player number, although the loop below runs over seven players

    draftOf(m_resultInfo)->m_gameRule = melee->m_gameRule;
    draftOf(m_resultInfo)->m_isTeams = melee->m_isTeams;
    draftOf(m_resultInfo)->m_suicideScoreMultiplier = melee->m_suicideScoreMultiplier;
    draftOf(m_resultInfo)->m_timeMinutes = (u32)melee->m_timeLimitFrames / 60 / 60;
    draftOf(m_resultInfo)->m_stockCount = m_setRule->m_stockCount;
    draftOf(m_resultInfo)->unkE_7 = 0;
    draftOf(m_resultInfo)->m_numWinners = 1;
    draftOf(m_resultInfo)->unk10 = 1;
    draftOf(m_resultInfo)->unk11 = 2;
    draftOf(m_resultInfo)->unk12 = 2;
    if (order[0] == 0) {
        draftOf(m_resultInfo)->unk13 = 0;
    } else if (order[1] == 0) {
        draftOf(m_resultInfo)->unk13 = 1;
    } else if (order[2] == 0) {
        draftOf(m_resultInfo)->unk13 = 2;
    } else if (order[3] == 0) {
        draftOf(m_resultInfo)->unk13 = 3;
    }
    draftOf(m_resultInfo)->unk1A = 0;
    draftOf(m_resultInfo)->unk1368 = 0;
    draftOf(m_resultInfo)->unk136C = 0;
    draftOf(m_resultInfo)->unk1370 = 1;
    draftOf(m_resultInfo)->unk1374 = 0;
    draftOf(m_resultInfo)->m_decisionKind = gmResultInfo::Decision_Timeup;

    for (u8 i = 0; i < 7; i++) {
        gmPlayerInitBytes* init = reinterpret_cast<gmPlayerInitBytes*>(&m_modeMelee->m_playersInitData[i]);
        gmResultPlayerDraft* player = reinterpret_cast<gmResultPlayerDraft*>(&m_resultInfo->m_playersResultInfo[i]);
        player->m_characterKind = init->m_characterKind;
        player->m_state = init->m_state;
        player->m_colorFileIdx = init->m_colorFileNo;
        player->unk5 = init->m_nameIndex;
        player->m_teamNo = init->m_teamNo;
        player->unk7_6 = init->m_0x1B_6;
        player->m_isStamina = init->m_isStamina;
        player->m_hitPointMax = init->m_hitPointMax;
        player->m_startStockCount = init->m_stockCount;
        player->m_startDamage = init->m_startDamage;
        player->m_rank = order[i];
        player->m_koCount = 20 - i;
        player->m_deadCount = 25 + i;
        player->m_suicideCount = 2 * i + 27;
        player->m_pickupCoins = 300 - i;
        player->m_lostCoins = 4 * i;
        for (u32 j = 0; j < player->m_koCount; j++) {
            player->koTarget[j] = (i + 1) % 4;
            player->koCause[j] = 0;
        }
        for (u32 j = 0; j < player->m_deadCount; j++) {
            player->deadTarget[j] = (i + 1) % 4;
            player->deadCause[j] = 0;
        }
        player->unk1CC[0] = 200;
        player->unk1CC[1] = 200;
        player->unk1CC[2] = 200;
        player->unk1CC[3] = 200;
        player->unk1CC[4] = 200;
        player->unk1CC[5] = 200;
        player->unk1CC[6] = 200;
        player->unk1CC[7] = 200;
        player->unk1EC = 200;
        for (int k = 0; k < 16; k++) {
            player->unk1F0[k] = 200;
        }
        player->unk234 = 50;
        player->unk238 = 0;
        player->unk23C = 0;
        player->unk240 = 58;
        player->unk244 = 157;
    }
}

void GameGlobal::initTournamentData() {
    reinterpret_cast<muTourData*>(m_tournamentData)->m_count = 0;
    reinterpret_cast<muTourData*>(m_tournamentData)->m_playersPerMatch = 0;
    reinterpret_cast<muTourData*>(m_tournamentData)->m_unk02 = 0;
    reinterpret_cast<muTourData*>(m_tournamentData)->m_unk03 = 0;
    for (int i = 0; i < 0x20; i++) {
        muTourEntrant* entrant = &reinterpret_cast<muTourData*>(m_tournamentData)->m_entrants[i];
        entrant->m_kind = 0;
        entrant->m_nameTag = 0x78;
        entrant->m_character = 0;
        entrant->m_color = 0;
        entrant->m_handicap = 0;
        entrant->m_slot = -1;
    }
}

void GameGlobal::InitHiScore() {
    for (int i = 0; i < 2; i++) {
        memset(&m_record->m_eventHiScoreData[i], 0, sizeof(gmGlobalRecord::EventHiScoreData));
    }
    memset(m_record->unk34D4, 0, sizeof(m_record->unk34D4));
    memset(m_record->unk3548, 0, sizeof(m_record->unk3548));
    for (int i = 0; i < 35; i++) {
        memset(m_record->m_charaNumData[i].unk9C, 0, sizeof(m_record->m_charaNumData[i].unk9C));
    }
}

void GameGlobal::InitBattleRecord() {
    gmGlobalRecord::CharaNumData* chara;
    for (int i = 0; i < 35; i++) {
        chara = &m_record->m_charaNumData[i];
        memset(chara->m_beatCount, 0, sizeof(chara->m_beatCount));
        memset(&chara->m_diagram, 0, sizeof(chara->m_diagram));
        chara->unk98 = 0;
        chara->m_fallCount = 0;
    }
    for (int i = 0; i < 120; i++) {
        gmGlobalRecord::NameData* name = nameRecord(this, i);
        name->unk0 = 0;
        name->unk2 = 0;
        memset(name->unk4, 0, sizeof(name->unk4));
        for (int j = 0; j < 35; j++) {
            name->unk54[j] = 0;
        }
        name->unkEE = 0;
    }
}

// Factory reset of the record: the counters, unlocks, names and rule settings go back to what a new save starts with.
void GameGlobal::InitAllRecord() {
    memset(m_record, 0, sizeof(gmGlobalRecord));
    memset(m_nameRecords, 0, 120 * sizeof(gmGlobalRecord::NameData));
    ((GameFrameState*)m_appData)->unk20 = OS_TICKS_TO_SEC(OSGetTime());
    (*g_GameGlobal->getGlobalRecordBootCountDatap())++;

    m_record->m_menuData.m_language = 1;
    fn_8001B784(1);
    bool deflicker = !lbl_805A0098->m_progressive;
    lbl_805A0098->setDeflicker(deflicker);
    m_record->m_menuData.m_deflicker = deflicker == 1;
    if (SCGetAspectRatio() == 1) {
        g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen = true;
        g_GameGlobal->getGlobalRecordMenuDatap()->m_41_6 = true;
    } else {
        g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen = false;
        g_GameGlobal->getGlobalRecordMenuDatap()->m_41_6 = false;
    }
    bool standardAspect = !g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
    gfCameraManager* camera = gfCameraManager::getManager();
    camera->setAspectRatio(standardAspect);

    m_record->m_menuData.m_volBalance = 50;
    fn_80076F30(g_sndSystem, 0.5f, 0.5f);
    gmGlobalRecord::MenuData::StageSwitch* stages = &m_record->m_menuData.m_stageSwitch;
    stages->m_normalMask = 0xFFFFFFFF;
    stages->m_meleeMask = 0xFFFFFFFF;
    m_record->m_menuData.m_itemFrequency = gmItSwitch::Frequency_Medium;
    u32* items = reinterpret_cast<u32*>(&m_record->m_menuData.m_itemSwitch);
    items[1] = 0xFFFFFFFF;
    items[0] = 0xFFFFFFFF;

    gmSetCoinNum(500);
    m_record->m_menuData.m_unk10[0] = 1;
    m_record->m_menuData.m_unk10[1] = 1;
    m_record->m_menuData.m_unk10[2] = 1;
    m_record->m_menuData.m_unk10[3] = 1;
    m_record->m_menuData.m_unk10[4] = 1;
    m_record->m_menuData.m_unk10[5] = 1;
    m_record->m_menuData.m_unk10[6] = 1;
    m_record->m_menuData.m_unk10[7] = 1;

    // the four bonus modes (all-star, boss battles, additional rules, random stage in stage select) are locked again
    g_GameGlobal->getGlobalRecordGameDatap()->m_hideMenuFlags &= ~1;
    g_GameGlobal->getGlobalRecordGameDatap()->m_hideMenuFlags &= ~2;
    g_GameGlobal->getGlobalRecordGameDatap()->m_hideMenuFlags &= ~4;
    g_GameGlobal->getGlobalRecordGameDatap()->m_hideMenuFlags &= ~8;
    gmInitChKindUseEnableAll();
    gmInitStageUseEnableAll();
    gmRecFigureAllInit();
    gmRecSealAllInit();
    gmInitAssistKindUseEnable();
    gmInitEventOpened();

    memset(reinterpret_cast<u8*>(m_record) + 0x1528, 0, 0x81);
    u32* unk15AC = reinterpret_cast<u32*>(reinterpret_cast<u8*>(m_record) + 0x15AC);
    for (int i = 0; i < 34; i++) {
        unk15AC[i] = 0;
    }
    fn_80078768(lbl_805A01D8);

    // default rules: time match, 2 minutes, 3 stocks, 100% damage, pause on, damage gauge shown
    gmSetRule* rule = m_setRule;
    reinterpret_cast<u8*>(rule)[2] = 0;
    rule->m_timeMinutes = 2;
    rule->m_stockCount = 3;
    rule->m_handicap = 0;
    rule->m_damageRatio = 10;
    rule->m_stageChoice = 0;
    rule->m_stockTimeMinutes = 0;
    rule->m_isTeamAttack = false;
    rule->m_allowPause = true;
    rule->m_showScoreDisplay = false;
    rule->m_showDamageGauge = 1;
    memset(&rule->_32[0x10], 0, 0x58);

    // HYPOTHESIS: the nte settings start with these values (one byte per setting, mostly percentages)
    s8* nte = reinterpret_cast<s8*>(g_GameGlobal->m_nteData);
    memset(nte, 0, sizeof(nteGlobalData));
    nte[0x02] = 24;
    nte[0x83] = 0;
    nte[0x41] = 1;
    nte[0x42] = 100;
    nte[0x43] = 20;
    nte[0x44] = 10;
    nte[0x45] = 40;
    nte[0x46] = 50;
    nte[0x47] = 60;
    nte[0x4B] = 15;
    nte[0x4E] = 25;
    nte[0x50] = -100;
    nte[0x51] = -100;
    nte[0x52] = -100;
    nte[0x53] = -100;
    nte[0x54] = -100;
    nte[0x57] = 10;
    nte[0x58] = 15;
    nte[0x59] = 20;
    nte[0x5A] = 25;
    nte[0x5B] = 70;
    nte[0x5C] = 65;
    nte[0x5D] = 60;
    nte[0x5E] = 50;
    nte[0x60] = 1;
    nte[0x61] = 1;
    nte[0x62] = 2;
    nte[0x63] = 3;
    nte[0x64] = 3;
    nte[0x65] = 2;
    nte[0x66] = 1;
    nte[0x68] = 2;
    nte[0x69] = 5;
    nte[0x6A] = 5;
    nte[0x6B] = 4;
    nte[0x6C] = 4;
    nte[0x6D] = 3;
    nte[0x6E] = 3;
    nte[0x70] = 15;
    nte[0x71] = 15;
    nte[0x72] = 10;
    nte[0x73] = 10;
    nte[0x74] = 10;
    nte[0x75] = 10;
    nte[0x76] = 80;
}

u32* GameGlobal::getCharaAppFlag() {
    return reinterpret_cast<u32*>(m_record);
}

u8* GameGlobal::getBGMAppFlag() {
    return m_record->m_bgmAppFlag;
}

u32* GameGlobal::getStageAppFlag() {
    return reinterpret_cast<u32*>(m_record->m_stageAppFlag);
}

u32* GameGlobal::getAssistAppFlag() {
    return reinterpret_cast<u32*>(m_record->m_assistAppFlag);
}

u8* GameGlobal::getClearGetAppFlag() {
    return m_record->m_clearGetAppFlag;
}

u8* GameGlobal::getHideEtcAppFlag() {
    return m_record->m_hideEtcAppFlag;
}

gmGlobalRecord* GameGlobal::getGlobalRecordGameDatap() {
    return m_record;
}

gmGlobalRecord::MenuData* GameGlobal::getGlobalRecordMenuDatap() {
    return &m_record->m_menuData;
}

gmGlobalRecord::CharaNumData* GameGlobal::getGlobalRecordCharaNumDatap(int chara) {
    return &m_record->m_charaNumData[chara];
}

u8* GameGlobal::getMovieAppFlag() {
    return m_record->m_movieAppFlag;
}

gmGlobalRecord::InfoAppFlagData* GameGlobal::getInfoAppFlag() {
    return &m_record->m_infoAppFlagData;
}

u32* GameGlobal::getGlobalRecordFigCoinDatap() {
    return &m_record->m_figCoin;
}

u32* GameGlobal::getGlobalRecordGetFigCoinDatap() {
    return &m_record->m_getFigCoin;
}

u32* GameGlobal::getGlobalRecordVsPartyPlayerCountDatap() {
    return &m_record->m_vsPartyPlayerCount;
}

u32* GameGlobal::getGlobalRecordVsMeleeTimeCountDatap() {
    return &m_record->m_vsMeleeTimeCount;
}

u32* GameGlobal::getGlobalRecordVsMeleeTimeTotalDatap() {
    return &m_record->m_vsMeleeTimeTotal;
}

u32* GameGlobal::getGlobalRecordVsTimeMeleeCountDatap() {
    return &m_record->m_vsTimeMeleeCount;
}

u32* GameGlobal::getGlobalRecordVsStockMeleeCountDatap() {
    return &m_record->m_vsStockMeleeCount;
}

u32* GameGlobal::getGlobalRecordVsCoinMeleeCountDatap() {
    return &m_record->m_vsCoinMeleeCount;
}

u32* GameGlobal::getGlobalRecordVsHpMeleeCountDatap() {
    return &m_record->m_vsHpMeleeCount;
}

u32* GameGlobal::getGlobalRecordOnlineVsTimeMeleeCountDatap() {
    return &m_record->m_onlineVsTimeMeleeCount;
}

u32* GameGlobal::getGlobalRecordOnlineVsStockMeleeCountDatap() {
    return &m_record->m_onlineVsStockMeleeCount;
}

u32* GameGlobal::getGlobalRecordOnlineVsCoinMeleeCountDatap() {
    return &m_record->m_onlineVsCoinMeleeCount;
}

u32* GameGlobal::getGlobalRecordVsResetMeleeCountDatap() {
    return &m_record->m_vsResetMeleeCount;
}

u32* GameGlobal::getGlobalRecordSingleMeleeTimeCountDatap() {
    return &m_record->m_singleMeleeTimeCount;
}

u32* GameGlobal::getGlobalRecordSingleMeleeTimeTotalDatap() {
    return &m_record->m_singleMeleeTimeTotal;
}

u32* GameGlobal::getGlobalRecordOnlineFriendTimeCountDatap() {
    return &m_record->m_onlineFriendTimeCount;
}

u32* GameGlobal::getGlobalRecordOnlineAnybodyTimeCountDatap() {
    return &m_record->m_onlineAnybodyTimeCount;
}

u32* GameGlobal::getGlobalRecordOnlineAnybodyWatchCountDatap() {
    return &m_record->m_onlineAnybodyWatchCount;
}

u32* GameGlobal::getGlobalRecordBootCountDatap() {
    return &m_record->m_bootCount;
}

u32* GameGlobal::getGlobalRecordWorkingTimeDatap() {
    s64 now = OS_TICKS_TO_SEC(OSGetTime());
    u32 time = m_record->m_workingTime + (u32)(now - ((GameFrameState*)m_appData)->unk20);
    if (time >= 360000000) {
        time = 359999999;
    }
    m_record->m_workingTime = time;
    ((GameFrameState*)m_appData)->unk20 = now;
    return &m_record->m_workingTime;
}

u32* GameGlobal::getGlobalRecordDamageTotalCountDatap() {
    return &m_record->m_damageTotalCount;
}

u32* GameGlobal::getGlobalRecordKillTotalCountDatap() {
    return &m_record->m_killTotalCount;
}

u32* GameGlobal::getGlobalRecordSuicideTotalCountDatap() {
    return &m_record->m_suicideTotalCount;
}

u8* GameGlobal::getGlobalRecordGetOpenedEventDatap(int index) {
    return &m_record->m_openedEvent[index];
}

// An event list is opened up to the event number stored for it: the event is open while it is below that number.
bool GameGlobal::isGlobalRecordEventOpened(u8 list, u8 event) {
    return m_record->m_openedEvent[list] > event;
}

u8* GameGlobal::getGlobalRecordGetStagePlayCountDatap(int stage) {
    return &m_record->m_stagePlayCount[stage];
}

bool GameGlobal::checkHideMenuAllstar() {
    return m_record->m_hideMenuFlags & 1;
}

void GameGlobal::unlockHideMenuAllstar() {
    m_record->m_hideMenuFlags |= 1;
}

bool GameGlobal::checkHideMenuBossBattle() {
    return (m_record->m_hideMenuFlags >> 1) & 1;
}

void GameGlobal::unlockHideMenuBossBattle() {
    m_record->m_hideMenuFlags |= 2;
}

bool GameGlobal::checkHideMenuAdditionalRule() {
    return (m_record->m_hideMenuFlags >> 2) & 1;
}

void GameGlobal::unlockHideMenuAdditionalRule() {
    m_record->m_hideMenuFlags |= 4;
}

bool GameGlobal::checkHideMenuRandomStageSW() {
    return (m_record->m_hideMenuFlags >> 3) & 1;
}

void GameGlobal::unlockHideMenuRandomStageSW() {
    m_record->m_hideMenuFlags |= 8;
}

void GameGlobal::initGameFrame() {
    GameFrameState* state = (GameFrameState*)m_appData;

    state->unk4 = 0;
    state->unkC = 1.0f;
    state->unkA = 0;
    state->unk8 = 1;
    state->unk9 = 1;
}

void GameGlobal::initGameFrameForReplay() {
    GameFrameState* state = (GameFrameState*)m_appData;

    state->unk14 = 0;
    state->unk18 = 0;
}

float GameGlobal::getGameFrame() const {
    GameFrameState* state = (GameFrameState*)m_appData;

    return (f32)state->unk4 + ((f32)state->unkA / (f32)state->unk8);
}

float GameGlobal::getGameFrameForPhysics() const {
    GameFrameState* state = (GameFrameState*)m_appData;

    return (f32)state->unk4 + ((f32)state->unkA / (f32)state->unk8);
}

float GameGlobal::getGameFrameDelta() const {
    GameFrameState* state = (GameFrameState*)m_appData;

    return state->unkC;
}

bool GameGlobal::isJustGameFrame() const {
    GameFrameState* state = (GameFrameState*)m_appData;

    return state->unkA == 0;
}

bool GameGlobal::isPrevJustGameFrame() const {
    GameFrameState* state = (GameFrameState*)m_appData;

    return state->unkA == state->unk8 - 1;
}

u32 GameGlobal::getSlowRate() const {
    GameFrameState* state = (GameFrameState*)m_appData;

    return state->unk8;
}

void GameGlobal::setSlowRate(u8 rate) {
    GameFrameState* state = (GameFrameState*)m_appData;
    u32 r = rate > 1 ? rate : 1;
    state->unk9 = (u8)r < 100 ? r : 100;
}

void GameGlobal::updateGameFrame() {
    GameFrameState* state = (GameFrameState*)m_appData;

    if (state->unk8 == 1) {
        state->unk4++;
        u8 temp = state->unk9;
        state->unk8 = temp;
        state->unkA = 0;
        state->unkC = 1.0f / (f32)temp;
        state->unk14++;
        state->unk18 = 0;
        return;
    }

    state->unkA++;
    if (state->unkA >= state->unk8) {
        state->unkA = 0;
        state->unk8 = state->unk9;
        state->unk4++;
    }
    state->unkC = 1.0f / (f32)state->unk8;

    state->unk18++;
    if (state->unk18 >= state->unk8) {
        state->unk18 = 0;
        state->unk14++;
    }
}

// Function bodies below are taken from https://github.com/jj-link/brawl.

u8 GameGlobal::getDistributionType() {
    GameFrameState* state = (GameFrameState*)m_appData;

    return state->unk0;
}

u8 GameGlobal::getEnableSaveLoad() {
    GameFrameState* state = (GameFrameState*)m_appData;

    return state->unk10;
}

void GameGlobal::setEnableSaveLoad(u8 flags) {
    GameFrameState* state = (GameFrameState*)m_appData;

    state->unk10 = flags;
}

u32 GameGlobal::getSaveDataSize() {
    return 0xC660;
}

// A save buffer starts with its version (0x14) and its size.
bool GameGlobal::checkSaveData(const void* data) {
    const u32* header = static_cast<const u32*>(data);
    if (header[0] == 0x14 && header[1] == 0xC660) {
        return true;
    }
    return false;
}

// Takes the save buffer into the record, the name records and the nte data (the layout is the one getSaveData writes).
// The window mode is not taken from the file: the widescreen flag stays what the console says, and the settings that the
// game applies right away (aspect ratio, language, deflicker, volume balance) are applied after loading.
void GameGlobal::setSaveData(const void* data) {
    const u8* buffer = static_cast<const u8*>(data);

    // HYPOTHESIS: this makes the play time of the record the time of the load before the copy overwrites it
    m_record->m_workingTime = OS_TICKS_TO_SEC(OSGetTime());
    bool widescreen = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
    memcpy(m_record, buffer + 8, sizeof(gmGlobalRecord));
    memcpy(m_nameRecords, buffer + 8 + sizeof(gmGlobalRecord), 120 * sizeof(gmGlobalRecord::NameData));
    memcpy(m_nteData, buffer + 8 + sizeof(gmGlobalRecord) + 120 * sizeof(gmGlobalRecord::NameData), sizeof(nteGlobalData));

    gmGlobalRecord::MenuData* menu = g_GameGlobal->getGlobalRecordMenuDatap();
    if (widescreen != menu->m_41_6) {
        menu->m_isWidescreen = widescreen;
        g_GameGlobal->getGlobalRecordMenuDatap()->m_41_6 = widescreen;
    }
    bool standardAspect = !g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
    gfCameraManager* camera = gfCameraManager::getManager();
    camera->setAspectRatio(standardAspect);

    int language = 1;
    switch (m_record->m_menuData.m_language) {
    case 0:
        language = 0;
        break;
    case 1:
        language = 1;
        break;
    }
    fn_8001B784(language);

    bool deflicker = m_record->m_menuData.m_deflicker == 1;
    lbl_805A0098->setDeflicker(deflicker);
    m_record->m_menuData.m_deflicker = deflicker == 1;

    s8 balance = m_record->m_menuData.m_volBalance;
    fn_80076F30(g_sndSystem, (f32)(100 - balance) / 100.0f, (f32)balance / 100.0f);

    // the "has been seen" bit of every hidden fighter starts clear
    for (int i = 0; i < 14; i++) {
        int bit = i * 3;
        gmResetBit(g_GameGlobal->getCharaAppFlag(), bit);
    }
}

// Writes the save buffer: version and size, then the record, the name records and the nte data. The play time is brought
// up to date first.
void GameGlobal::getSaveData(void* data) {
    u8* buffer = static_cast<u8*>(data);

    getGlobalRecordWorkingTimeDatap();
    reinterpret_cast<u32*>(buffer)[0] = 0x14;
    reinterpret_cast<u32*>(buffer)[1] = 0xC660;
    memcpy(buffer + 8, m_record, sizeof(gmGlobalRecord));
    memcpy(buffer + 8 + sizeof(gmGlobalRecord), m_nameRecords, 120 * sizeof(gmGlobalRecord::NameData));
    memcpy(buffer + 8 + sizeof(gmGlobalRecord) + 120 * sizeof(gmGlobalRecord::NameData), m_nteData, sizeof(nteGlobalData));
}

u32 GameGlobal::getAdventureSaveDataSize() {
    return 0x4930;
}

// The adventure save is a header with the offsets of two parts, the name of the player (UTF-16 behind a date stamp) and
// the adventure data (0x4910 bytes). The name is how the adventure data finds its player again: the index of the name
// record with that name is kept in the data (0x78 when there is none).
// The index of the name record whose name is the given one, -1 if there is none.
// MATCH-ONLY: the lookup is a file-local twin of an inline of the original.
static inline int findNameRecord(GameGlobal* global, int nameCount, const char* name) {
    char current[0x15];
    for (int i = 0; i < nameCount; i++) {
        nameUTF8(global, i, current);
        if (strcmp(name, current) == 0) {
            return i;
        }
    }
    return -1;
}

void GameGlobal::setAdventureSaveData(const void* data) {
    const u8* buffer = static_cast<const u8*>(data);
    const u32* header = static_cast<const u32*>(data);

    memcpy(m_advData, buffer + header[1], 0x4910);
    char savedName[0x15];
    utf16to8__7MessageFPcPCw(savedName, reinterpret_cast<const wchar_t*>(buffer + header[0] + 6));

    int nameCount = 0;
    for (int i = 0; i < 120; i++) {
        if (m_nameRecords[i].unkE0 != 0) {
            nameCount++;
        }
    }
    int found = findNameRecord(this, nameCount, savedName);
    int index = 0x78;
    if (found >= 0) {
        index = found;
    }
    reinterpret_cast<u8*>(m_advData)[0x600] = index;
}

void GameGlobal::getAdventureSaveData(void* data) {
    u8* buffer = static_cast<u8*>(data);
    u32* header = static_cast<u32*>(data);

    header[0] = 8;
    header[1] = 0x20;
    memcpy(buffer + 0x20, m_advData, 0x4910);
    u8* nameBlock = buffer + header[0];
    *reinterpret_cast<u32*>(nameBlock) = 0x07092000;
    nameBlock[4] = 1;
    char name[0x15] = { 0 };
    u8 index = reinterpret_cast<u8*>(m_advData)[0x600];
    if (index < 0x78) {
        nameUTF8(this, index, name);
    }
    utf8to16__7MessageFPwPCc(reinterpret_cast<wchar_t*>(nameBlock + 6), name);
}

void addMaxCheck(u32* dst, u32 add, u32 max) {
    unsigned over = 0;
    if (add > max || *dst > max - add) {
        over = 1;
    }
    max = over ? max : *dst + add;
    *dst = max;
}
