#include <types.h>
#include <gm/gm_global.h>
#include <gm/gm_common.h>
#include <gf/gf_video.h>
#include <ip/ip_padconfig.h>
#include <revolution/OS/OSTime.h>
#include <revolution/SC/scapi.h>
#include <revolution/SC/scsystem.h>
#include <snd/snd_system.h>
#include <sora/ms_message.h>
#include <sr/sr_common.h>
#include <string.h>

// HYPOTHESIS: stand-ins for callees that are not named yet. fn_8001B784 (8 bytes) hands the language to the file system
// code (map: gfFileIO::setLanguage), fn_80076F30 is sndSystem::setVolBalance (music, sound effects), fn_80052250 is
// gmCharaFlag::setGlobalCharaData returned by value (the set of selectable fighters as one 64-bit value).
extern "C" void fn_8001B784(int language);
extern "C" void fn_80076F30(sndSystem* system, f32 bgm, f32 se);
extern "C" u64 fn_80052250();
// HYPOTHESIS: the parameter is the coin count of the record (gm_common.cpp); the shared header does not declare it yet.
u32 gmSetCoinNum(u32 coins);
void gmRecFigureAllInit();
void gmRecSealAllInit();

// HYPOTHESIS: m_appData points at the frame counter block that updateGameFrame writes (see gm_global_b.cpp); unk0 is
// the distribution type, unk4 the frame count, unk8 the frame divisor with unk9 its reload value, and unkA the position
// within the current frame.
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

// The factory button layout of a name tag (GameGlobal::buttonConfigInit).
static const ipButtonConfig sDefaultButtonConfig = {
    {3, 3, 4, 10, 11, 12, 0, 1, 5, 2, 2, 0x80},
    {
        {1, 0, 5, 9, 3, 4, 2, 4},
        {10, 12, 11, 0, 1, 4, 3, 2, 2, 4, 3, 0x40},
        {3, 3, 4, 4, 10, 11, 12, 0, 1, 5, 2, 2, 0x80},
    },
};

// HYPOTHESIS: two words that stay zero; the Adventure save data fills its counters from them. They sit in .sdata
// (initialised data), one per user.
static u32 sAdvDefaultCreate = 0;
static u32 sAdvDefaultInit = 0;

// HYPOTHESIS: the front of gmSelCharData (0xB8 bytes before the player init records): one record of four bytes per
// player from +0xA, and a bit field at +0x2E.
struct gmSelCharHeadView {
    char _0[0xA];
    struct Slot {
        u8 m_unk0; // starts at 40
        char _1;
        s16 m_total; // the sum of the KOs over several matches (see gmMeleeDest)
    } m_slots[MAX_PLAYERS];
    char _26[8];
    u8 : 3;
    u8 m_unk2E : 3; // starts at 3
    u8 : 2;
};

// HYPOTHESIS: the Adventure save data (gmAdvData): the first 34 records are the levels, followed by two tables whose
// entries start empty.
struct AdvLevelView {
    u32 m_state;
    s32 m_difficulty;
    u32 m_unk1;
    s32 m_percent;
    u32 m_unk2;
};
struct AdvSlotView {
    s32 m_id;
    u8 m_value;
};
struct AdvFlagView {
    u8 m_flag;
    s32 m_id;
};
struct gmAdvDataView {
    AdvLevelView m_levels[34];  // 0x000
    char _2A8[0xC];             // 0x2A8
    u8 unk2B4;
    u8 unk2B5;
    char _2B6[3];
    u8 unk2B9[0x27];            // 0x2B9
    AdvSlotView m_slots[99];    // 0x2E0
    char _5F8[2];
    u8 unk5FA;
    char _5FB;
    u8 unk5FC;
    u8 unk5FD;
    u8 unk5FE;
    u8 unk5FF;
    u8 unk600;
    u8 unk601;
    char _602[10];
    u32 unk60C;
    char _610[12];
    u32 unk61C;
    u32 unk620;
    s32 unk624;
    u32 unk628;
    u32 unk62C;
    char _630[0x4268];
    u32 unk4898[24];            // 0x4898
    u32 unk48F8;
    char _48FC[0x14];
    u8 unk4910[0x18];           // 0x4910
    AdvFlagView m_flags[2];     // 0x4928
    char _4938[0x20];
};

// The constructor of the Adventure save data, run when the singleton is made.
struct gmAdvDataInit : gmAdvDataView {
    gmAdvDataInit() {
        m_slots[0].m_id = -1;
        m_slots[0].m_value = 0xFF;
        for (AdvSlotView* p = &m_slots[1]; p < &m_slots[99]; p++) {
            p->m_id = -1;
            p->m_value = 0xFF;
        }
        for (int i = 0; i < 24; i++) {
            unk4898[i] = sAdvDefaultCreate;
        }
        unk48F8 = 0;
        m_flags[0].m_flag = 0;
        m_flags[0].m_id = -1;
        for (AdvFlagView* p = &m_flags[1]; p < &m_flags[2]; p++) {
            p->m_flag = 0;
            p->m_id = -1;
        }
    }
};

// HYPOTHESIS: the tail of gmMeleeInitData that gmInitGlobalModeMelee leaves alone and init resets afterwards.
struct gmMeleeInitTailView {
    char _0[0x4C];
    u32 unk4C;
    u8 unk50;
    char _51[7];
    u8 unk58;
    u8 unk59;
    char _5A[0x22];
    bool unk7C_7 : 1;
    bool unk7C_6 : 1;
    bool unk7C_5 : 1;
    bool unk7C_4 : 1;
    bool unk7C_3 : 1;
    bool unk7C_2 : 1;
    bool unk7C_1 : 1;
    bool unk7C_0 : 1;
    char _7D;
    s16 unk7E;
    u8 unk80;
    u8 unk81;
    bool unk82_7 : 1;
    bool unk82_6 : 1;
    bool unk82_5 : 1;
    bool unk82_4 : 1;
    bool unk82_3 : 1;
    bool unk82_2 : 1;
    bool unk82_1 : 1;
    bool unk82_0 : 1;
    char _83;
    u32 unk84;
    bool unk88_7 : 1;
    bool unk88_6 : 1;
    bool unk88_5 : 1;
    bool unk88_4 : 1;
    bool unk88_3 : 1;
    bool unk88_2 : 1;
    bool unk88_1 : 1;
    bool unk88_0 : 1;
};

// HYPOTHESIS: stands in for the inlined GameGlobal::getGlobalRecordMenuDatap (defined in gm_global_b.cpp).
static inline gmGlobalRecord::MenuData* getMenuData(GameGlobal* global) {
    return &global->m_record->m_menuData;
}

static inline void setRumble(gmGlobalRecord::MenuData* menu, int player, u8 value) {
    menu->m_unk10[player] = value;
}

inline gmGlobalRecord::MenuData* GameGlobal::getGlobalRecordMenuDatap() {
    return &m_record->m_menuData;
}

// Allocates the singleton and every block of data it points to.
void GameGlobal::create() {
    g_GameGlobal = new (Heaps::GameGlobal) GameGlobal;
    g_GameGlobal->m_appData = new (Heaps::GameGlobal) gmAppData;
    g_GameGlobal->m_handicapCounts = static_cast<s16*>(operator new(4 * sizeof(s16), Heaps::GameGlobal));
    g_GameGlobal->m_modeMelee = new (Heaps::GameGlobal) gmGlobalModeMelee;
    g_GameGlobal->unkC = new (Heaps::GameGlobal) gmPlayerInitData;
    g_GameGlobal->m_selCharData = new (Heaps::GameGlobal) gmSelCharData;
    g_GameGlobal->m_selStageData = new (Heaps::GameGlobal) gmSelStageData;
    g_GameGlobal->m_resultInfo = new (Heaps::GameGlobal) gmResultInfo;
    g_GameGlobal->m_setRule = new (Heaps::GameGlobal) gmSetRule;
    g_GameGlobal->m_record = new (Heaps::GameGlobal) gmGlobalRecord;
    g_GameGlobal->m_nameRecords = static_cast<gmGlobalRecord::NameData*>(
        operator new(120 * sizeof(gmGlobalRecord::NameData), Heaps::GameGlobal));
    g_GameGlobal->m_tournamentData = new (Heaps::GameGlobal) gmTournamentData;
    g_GameGlobal->m_advData = reinterpret_cast<gmAdvData*>(new (Heaps::GameGlobal) gmAdvDataInit);
    g_GameGlobal->m_nteData = new (Heaps::GameGlobal) nteGlobalData;
    g_GameGlobal->unk38 = static_cast<u8*>(operator new(0x20, Heaps::GameGlobal));
    g_GameGlobal->m_corps = new (Heaps::GameGlobal) gmGlobalCorps;
    g_GameGlobal->m_stageEditData = new (Heaps::GameGlobal) gmStageEditData;
    g_GameGlobal->m_stageData = new (Heaps::GameGlobal) gmStageData;
    g_GameGlobal->unk48 = static_cast<u8*>(operator new(6, Heaps::GameGlobal));
    g_GameGlobal->unk4C = static_cast<u8*>(operator new(5, Heaps::GameGlobal));
    g_GameGlobal->init();
}

// Puts every block into the state of a fresh boot: empty save data, the factory rules and options, English.
void GameGlobal::init() {
    memset(m_appData, 0, sizeof(gmAppData));
    memset(m_handicapCounts, 0, 4 * sizeof(s16));
    memset(m_modeMelee, 0, sizeof(gmGlobalModeMelee));
    memset(unkC, 0, sizeof(gmPlayerInitData));
    memset(m_selCharData, 0, sizeof(gmSelCharData));
    memset(m_selStageData, 0, sizeof(gmSelStageData));
    memset(m_resultInfo, 0, sizeof(gmResultInfo));
    memset(m_setRule, 0, sizeof(gmSetRule));
    memset(m_record, 0, sizeof(gmGlobalRecord));
    memset(m_nameRecords, 0, 120 * sizeof(gmGlobalRecord::NameData));
    memset(m_tournamentData, 0, sizeof(gmTournamentData));
    memset(m_advData, 0, sizeof(gmAdvData));
    memset(m_corps, 0, sizeof(gmGlobalCorps));
    memset(m_stageEditData, 0, sizeof(gmStageEditData));
    memset(m_stageData, 0, sizeof(gmStageData));

    reinterpret_cast<GameFrameState*>(m_appData)->unk0 = 1;
    m_record->m_menuData.m_language = 1;
    fn_8001B784(1);

    // The play time counts from now on.
    reinterpret_cast<GameFrameState*>(m_appData)->unk20 = OS_TICKS_TO_SEC(OSGetTime());

    // initGameFrame() inlined: frame 0 of normal speed.
    GameFrameState* frame = reinterpret_cast<GameFrameState*>(m_appData);
    frame->unk4 = 0;
    frame->unkC = 1.0f;
    frame->unkA = 0;
    frame->unk8 = 1;
    frame->unk9 = 1;

    // The video options start from what the console is set to: no deflicker filter in progressive mode.
    u32 deflicker = !lbl_805A0098->m_progressive;
    lbl_805A0098->setDeflicker(deflicker);
    m_record->m_menuData.m_deflicker = deflicker == 1;
    m_record->m_menuData.m_volBalance = 50;
    fn_80076F30(g_sndSystem, 0.5f, 0.5f);

    m_record->m_menuData.m_stageSwitch.m_normalMask = -1;
    m_record->m_menuData.m_stageSwitch.m_meleeMask = -1;
    gmSetCoinNum(500);
    for (int i = 0; i < 8; i++) {
        m_record->m_menuData.m_unk10[i] = 1;
    }

    // The factory rules: 3 stocks, 2 minutes, damage ratio 1.0, no stock time.
    m_setRule->m_stockCount = 3;
    m_setRule->m_damageRatio = 10;
    m_setRule->m_timeMinutes = 2;
    m_setRule->m_stockTimeMinutes = 0;

    initSelCharData();

    gmGlobalModeMelee* melee = m_modeMelee;
    gmInitGlobalModeMelee(melee);
    gmMeleeInitTailView* tail = reinterpret_cast<gmMeleeInitTailView*>(&melee->m_meleeInitData);
    tail->unk7C_7 = false;
    tail->unk7C_6 = true;
    tail->unk7C_5 = false;
    tail->unk7E = 0;
    tail->unk80 = 0;
    tail->unk81 = 0;
    tail->unk88_7 = false;
    tail->unk4C = 0;
    tail->unk50 = 1;
    tail->unk58 = 2;
    tail->unk59 = 4;
    tail->unk82_7 = false;
    tail->unk82_6 = false;
    tail->unk84 = 0;

    initResultInfoData();

    m_record->m_menuData.m_itemFrequency = 2;
    // MATCH-ONLY: the native code sets the item switches as two words.
    *reinterpret_cast<u64*>(&m_record->m_menuData.m_itemSwitch) = 0xFFFFFFFFFFFFFFFFULL;

    // The widescreen options follow the console's aspect ratio setting.
    while (SCCheckStatus() != 0) {
    }
    if (SCGetAspectRatio() == 1) {
        g_GameGlobal->m_record->m_menuData.m_isWidescreen = true;
        g_GameGlobal->m_record->m_menuData.m_41_6 = true;
    } else {
        g_GameGlobal->m_record->m_menuData.m_isWidescreen = false;
        g_GameGlobal->m_record->m_menuData.m_41_6 = false;
    }

    m_setRule->m_allowPause = true;
    m_setRule->m_showDamageGauge = 1;
    // HYPOTHESIS: two more rule fields that start at zero (not named in gmSetRule).
    reinterpret_cast<u8*>(m_setRule)[0x20] = 0;
    *reinterpret_cast<u32*>(reinterpret_cast<u8*>(m_setRule) + 0x24) = 0;

    // The Adventure save data starts empty: no level cleared, all slots unused.
    gmAdvDataView* adv = reinterpret_cast<gmAdvDataView*>(g_GameGlobal->m_advData);
    memset(adv->unk2B9, -1, sizeof(adv->unk2B9));
    adv->unk2B9[0] = 0;
    adv->unk2B4 = 1;
    adv->unk2B5 = 0;
    for (int i = 0; i < 99; i++) {
        adv->m_slots[i].m_id = -1;
        adv->m_slots[i].m_value = 0xFF;
    }
    adv->unk5FA = 0;
    adv->unk5FC = 8;
    adv->unk5FD = 8;
    adv->unk5FE = 0;
    adv->unk5FF = 1;
    adv->unk600 = 0;
    adv->unk601 = 0;
    adv->unk61C = 0;
    adv->unk60C = 0;
    adv->unk620 = 0;
    adv->unk62C = 0;
    adv->unk624 = -1;
    adv->unk628 = 0;
    for (int i = 0; i < 24; i++) {
        adv->unk4898[i] = sAdvDefaultInit;
    }
    adv->unk48F8 = 0;
    for (int i = 0; i < 34; i++) {
        adv->m_levels[i].m_state = 1;
        adv->m_levels[i].m_difficulty = 0;
        adv->m_levels[i].m_unk1 = 0;
        adv->m_levels[i].m_percent = -1;
        adv->m_levels[i].m_unk2 = 0;
    }
    adv->m_levels[0].m_state = 0;
    adv->m_levels[0].m_difficulty = 1;

    // The hidden menu entries start locked.
    g_GameGlobal->m_record->m_hideMenuFlags &= ~1;
    g_GameGlobal->m_record->m_hideMenuFlags &= ~2;
    g_GameGlobal->m_record->m_hideMenuFlags &= ~4;
    g_GameGlobal->m_record->m_hideMenuFlags &= ~8;

    gmInitChKindUseEnableAll();
    u64 charaFlags = fn_80052250();
    melee->m_meleeInitData.m_charaFlags.m_word0 = charaFlags >> 32;
    melee->m_meleeInitData.m_charaFlags.m_word1 = charaFlags;
    gmInitStageUseEnableAll();
    gmMakeCollectionRareTable();
    gmRecFigureAllInit();
    gmRecSealAllInit();
    gmInitAssistKindUseEnable();
    gmInitEventOpened();

    // HYPOTHESIS: the part of the record in front of the per-character data (counters of the Statistics screen).
    u8* record = reinterpret_cast<u8*>(m_record);
    memset(record + 0x1528, 0, 0x81);
    u32* counters = reinterpret_cast<u32*>(reinterpret_cast<u8*>(m_record) + 0x15AC);
    for (int i = 0; i < 34; i++) {
        counters[i] = 0;
    }

    memset(&adv->unk4910, 0, 0x48);
    adv->m_flags[0].m_flag = 0;
    adv->m_flags[0].m_id = 0;
    adv->m_flags[1].m_flag = 0;
    adv->m_flags[1].m_id = -1;

    // The factory values of the net battle rules.
    u8* nte = reinterpret_cast<u8*>(g_GameGlobal->m_nteData);
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
    memset(g_GameGlobal->unk38, 0, 0x20);
}

// Clears the selection data: every slot has no fighter, the CPU level and the default hit points.
void GameGlobal::initSelCharData() {
    gmSelCharData* select = g_GameGlobal->m_selCharData;
    memset(select, 0, sizeof(gmSelCharData));
    gmSelCharHeadView* head = reinterpret_cast<gmSelCharHeadView*>(select);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        head->m_slots[i].m_unk0 = 40;
        head->m_slots[i].m_total = 0;
        gmPlayerInitData* player = &select->m_playersInitData[i];
        player->m_state = 3;
        // MATCH-ONLY: the native code stores the fighter slot as a plain byte, not as an enum bit field.
        *reinterpret_cast<u8*>(player) = Character_SelectNone;
        player->m_nameIndex = 0x78;
        player->m_hitPointMax = 150;
        player->m_cpuRank = 2;
    }
    head->m_unk2E = 3;
}

gmGlobalRecord::NameData* GameGlobal::getGlobalRecordNameDatap(u8 index) {
    return &m_nameRecords[index];
}

int GameGlobal::getNumName() {
    int count = 0;
    for (int i = 0; i < 120; i++) {
        if (m_nameRecords[i].m_name[0] != 0) {
            count++;
        }
    }
    return count;
}

// Empties a name tag: no text, no counters, the factory buttons and the rumble switched on.
static inline void clearNameTag(GameGlobal* self, int index) {
    gmSetAutosaveDirtyFlag();
    gmGlobalRecord::NameData* tag = &self->m_nameRecords[index];
    gmGlobalRecord::NameData* name = self->getGlobalRecordNameDatap(index);
    name->unk0 = 0;
    name->unk2 = 0;
    memset(name->unk4, 0, sizeof(name->unk4));
    for (int i = 0; i < 35; i++) {
        name->unk54[i] = 0;
    }
    name->unkEE = 0;
    buttonConfigInit(reinterpret_cast<ipButtonConfig*>(self->getGlobalRecordNameDatap(index)->m_buttonConfig));
    tag->m_name[0] = 0;
    tag->m_rumble = 1;
}

// Makes a name tag from the first five characters of a UTF-16 text; the date is the day it is made.
static inline void makeNameTag(GameGlobal* self, int index, const wchar_t* name, int player, u32 date) {
    clearNameTag(self, index);
    gmGlobalRecord::NameData* tag = &self->m_nameRecords[index];
    int count = 0;
    while (count < 5 && *name != 0) {
        tag->m_name[count] = *name;
        count++;
        name++;
    }
    tag->m_name[count] = 0;
    tag->m_date = date;
    tag->m_rumble = self->getRumbleSetting(player);
}

void GameGlobal::delName(int index) {
    clearNameTag(this, index);
}

void GameGlobal::setName(int index, const wchar_t* name, int player) {
    gmSetAutosaveDirtyFlag();
    makeNameTag(this, index, name, player, gmGetDateCount());
}

void GameGlobal::setName1(int index, const wchar_t* name, int player, u32 date) {
    clearNameTag(this, index);
    gmGlobalRecord::NameData* tag = &m_nameRecords[index];
    int count = 0;
    while (count < 5 && *name != 0) {
        tag->m_name[count] = *name;
        count++;
        name++;
    }
    tag->m_name[count] = 0;
    tag->m_date = date;
    tag->m_rumble = getRumbleSetting(player);
}

bool GameGlobal::getNameUTF16(int index, wchar_t* dst) {
    gmGlobalRecord::NameData* name = &m_nameRecords[index];
    if (name->m_name[0] == 0) {
        return false;
    }
    memcpy(dst, name->m_name, sizeof(name->m_name));
    return true;
}

bool GameGlobal::getName(int index, char* dst) {
    wchar_t text[6];
    if (getNameUTF16(index, text)) {
        Message::utf16to8(dst, text);
        return true;
    }
    return false;
}

int GameGlobal::getNameCapacityRest() {
    int used = 0;
    for (int i = 0; i < 120; i++) {
        if (m_nameRecords[i].m_name[0] != 0) {
            used++;
        }
    }
    return 120 - used;
}

int GameGlobal::getNameNum(const char* name) {
    int count = getNumName();
    for (int i = 0; i < count; i++) {
        wchar_t wide[6];
        char utf8[0x15];
        if (getNameUTF16(i, wide)) {
            Message::utf16to8(utf8, wide);
        }
        if (strcmp(name, utf8) == 0) {
            return i;
        }
    }
    return -1;
}

bool GameGlobal::setNameRumbleSwitch(int index, u8 value) {
    gmGlobalRecord::NameData* name = &m_nameRecords[index];
    if (name->m_name[0] == 0) {
        return false;
    }
    name->m_rumble = value;
    return true;
}

bool GameGlobal::getNameRumbleSwitch(int index) {
    gmGlobalRecord::NameData* name = &m_nameRecords[index];
    if (name->m_name[0] == 0) {
        return false;
    }
    return name->m_rumble == 1;
}

// The shared header declares getLanguage static, but the native function reads this->m_record: the callers pass the
// singleton in r3, so it is defined under its mangled name with an explicit receiver.
extern "C" int getLanguage__10GameGlobalFv(GameGlobal* self) {
    return self->m_record->m_menuData.m_language;
}

bool GameGlobal::isLanguageJp() {
    return m_record->m_menuData.m_language == 0;
}

// HYPOTHESIS: the same test for language 1 (English); no map name is known.
extern "C" bool fn_8004D9FC(GameGlobal* self) {
    return self->m_record->m_menuData.m_language == 1;
}

// The handicap that the automatic handicap keeps for a player: the tag's own when the player has a name tag, else the
// counter of the player's slot (only the four fighter slots have one).
u16* GameGlobal::getVsModeAutoHandiPlayerVal(int player, int nameIndex) {
    if (nameIndex == 0x78) {
        if (player < 4) {
            return reinterpret_cast<u16*>(&m_handicapCounts[player]);
        }
        return nullptr;
    }
    return &m_nameRecords[nameIndex].m_handicap;
}

void buttonConfigInit(ipButtonConfig* config) {
    memcpy(config, &sDefaultButtonConfig, sizeof(ipButtonConfig));
}

void GameGlobal::setButtonConfig(int index, const ipButtonConfig* config) {
    memcpy(m_nameRecords[index].m_buttonConfig, config, sizeof(ipButtonConfig));
}

ipButtonConfig* GameGlobal::getButtonConfig(int index) {
    return reinterpret_cast<ipButtonConfig*>(m_nameRecords[index].m_buttonConfig);
}

bool GameGlobal::getRumbleSetting(int player) {
    return m_record->m_menuData.m_unk10[player] != 0;
}

void GameGlobal::setRumbleSetting(int player, u8 value) {
    getGlobalRecordMenuDatap()->m_unk10[player] = value;
}
