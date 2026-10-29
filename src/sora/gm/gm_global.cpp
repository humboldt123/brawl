#include <types.h>
#include <gm/gm_global.h>
#include <revolution/OS/OSTime.h>
#include <string.h>
#include <mu/mu_tour_table.h>

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
    char unk10[0x4];
    s32 unk14;
    u8 unk18;
    char unk19[0x7];
    s64 unk20;
};

gmGlobalRecord::NameData* GameGlobal::getGlobalRecordNameDatap(u8 index) {
    return &m_nameRecords[index];
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
        memset(chara->unk0, 0, sizeof(chara->unk0));
        memset(chara->unk48, 0, sizeof(chara->unk48));
        chara->unk98 = 0;
        chara->unk46 = 0;
    }
    for (int i = 0; i < 120; i++) {
        gmGlobalRecord::NameData* name = getGlobalRecordNameDatap(i);
        name->unk0 = 0;
        name->unk2 = 0;
        memset(name->unk4, 0, sizeof(name->unk4));
        for (int j = 0; j < 35; j++) {
            name->unk54[j] = 0;
        }
        name->unkEE = 0;
    }
}

u8* GameGlobal::getBGMAppFlag() {
    return m_record->m_bgmAppFlag;
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

void GameGlobal::unlockHideMenuAdditionalRule() {
    m_record->m_hideMenuFlags |= 4;
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

extern "C" int fn_8004EAEC() {
    return 0xC660;
}

extern "C" void fn_8004F258(int* a, unsigned b, unsigned c) {
    unsigned flag = 0;
    if (b > c || (unsigned)*a > c - b) {
        flag = 1;
    }
    c = flag ? c : *a + b;
    *a = (int)c;
}
