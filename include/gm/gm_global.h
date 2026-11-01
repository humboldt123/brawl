// Local shadow of BrawlHeaders' gm/gm_global.h: adds the frame queries used by the fighter manager
// (names from the sora map: GameGlobal::isJustGameFrame / isPrevJustGameFrame).
#pragma once

#include <StaticAssert.h>
#include <gm/gm_global_corps.h>
#include <gm/gm_global_mode_melee.h>
#include <gm/gm_global_record.h>
#include <gm/gm_result_info.h>
#include <gm/gm_sel_char_data.h>
#include <gm/gm_sel_stage_data.h>
#include <gm/gm_set_rule.h>
#include <gm/gm_tournament_data.h>
#include <gm/gm_stage_data.h>
#include <gm/gm_stage_edit_data.h>
#include <types.h>

struct ipButtonConfig;

class GameGlobal {
public:
    /* data */
    gmAppData* m_appData;               // 0x00
    s16* m_handicapCounts;              // 0x04 (HYPOTHESIS: four halfwords, cleared by gmInitVSMelee / gmResetHandiSetting)
    gmGlobalModeMelee* m_modeMelee;     // 0x08
    union {
        char _1[0x04];                  // 0x0C
        gmPlayerInitData* unkC;         // HYPOTHESIS: one spare player init record (0x5C bytes, zeroed by init)
    };
    gmSelCharData* m_selCharData;       // 0x10
    gmSelStageData* m_selStageData;     // 0x14
    gmResultInfo* m_resultInfo;         // 0x18
    gmSetRule* m_setRule;               // 0x1C
    char _3[0x04];                      // 0x20
    gmGlobalRecord* m_record;         // 0x24
    gmGlobalRecord::NameData* m_nameRecords;         // 0x28
    gmTournamentData* m_tournamentData; // 0x2C
    gmAdvData* m_advData;           // 0x30
    nteGlobalData* m_nteData;           // 0x34
    union {
        char _4[0x04];                  // 0x38
        u8* unk38;                      // HYPOTHESIS: a 0x20-byte block that init zeroes after the net rule defaults
    };
    gmGlobalCorps* m_corps;             // 0x3C
    gmStageEditData* m_stageEditData;   // 0x40
    gmStageData* m_stageData;           // 0x44
    union {
        char _6[0x8];                   // 0x48
        struct {
            u8* unk48;                  // HYPOTHESIS: 6-byte backup of the rule screen bytes 2..7 (gmRuleBackupForNet)
            u8* unk4C;                  // HYPOTHESIS: 5-byte backup of the rule screen bytes 8..12
        };
    };

    gmGlobalRecord::MenuData* getGlobalRecordMenuDatap();
    // Accessors below: names from the sora map; return types are HYPOTHESIS.
    gmGlobalRecord::NameData* getGlobalRecordNameDatap(int index); // the index is an int: the native function and its callers do not narrow it
    void initTournamentData();
    void InitHiScore();
    void InitBattleRecord();
    u8* getBGMAppFlag();
    u8* getClearGetAppFlag();
    u8* getHideEtcAppFlag();
    gmGlobalRecord* getGlobalRecordGameDatap();
    gmGlobalRecord::CharaNumData* getGlobalRecordCharaNumDatap(int chara);
    u8* getMovieAppFlag();
    gmGlobalRecord::InfoAppFlagData* getInfoAppFlag();
    u32* getStageAppFlag();                 // the bits of the stages (and other things) that were unlocked
    int getNumName();                       // how many names there are
    u32* getGlobalRecordFigCoinDatap();
    u32* getGlobalRecordGetFigCoinDatap();
    u32* getGlobalRecordVsPartyPlayerCountDatap();
    u32* getGlobalRecordVsMeleeTimeCountDatap();
    u32* getGlobalRecordVsMeleeTimeTotalDatap();
    u32* getGlobalRecordVsTimeMeleeCountDatap();
    u32* getGlobalRecordVsStockMeleeCountDatap();
    u32* getGlobalRecordVsCoinMeleeCountDatap();
    u32* getGlobalRecordVsHpMeleeCountDatap();
    u32* getGlobalRecordOnlineVsTimeMeleeCountDatap();
    u32* getGlobalRecordOnlineVsStockMeleeCountDatap();
    u32* getGlobalRecordOnlineVsCoinMeleeCountDatap();
    u32* getGlobalRecordVsResetMeleeCountDatap();
    u32* getGlobalRecordSingleMeleeTimeCountDatap();
    u32* getGlobalRecordSingleMeleeTimeTotalDatap();
    u32* getGlobalRecordOnlineFriendTimeCountDatap();
    u32* getGlobalRecordOnlineAnybodyTimeCountDatap();
    u32* getGlobalRecordOnlineAnybodyWatchCountDatap();
    u32* getGlobalRecordBootCountDatap();
    u32* getGlobalRecordWorkingTimeDatap();
    u32* getGlobalRecordDamageTotalCountDatap();
    u32* getGlobalRecordKillTotalCountDatap();
    u32* getGlobalRecordSuicideTotalCountDatap();
    u8* getGlobalRecordGetOpenedEventDatap(int index);
    u8* getGlobalRecordGetStagePlayCountDatap(int stage);
    bool checkHideMenuAllstar();
    void unlockHideMenuAllstar();
    bool checkHideMenuBossBattle();
    void unlockHideMenuBossBattle();
    void unlockHideMenuAdditionalRule();
    void unlockHideMenuRandomStageSW();
    void initGameFrame();
    void initGameFrameForReplay();
    float getGameFrameForPhysics() const;
    u8 getDistributionType();
    float getGameFrame() const;
    void updateGameFrame();
    bool isJustGameFrame() const;
    bool isPrevJustGameFrame() const;
    u32 getSlowRate() const;
    void setSlowRate(u8 rate);

    // Cluster A (gm_global.cpp, 0x8004C1A4-0x8004DAC4). Names are from the sora map unless noted; types are HYPOTHESIS.
    static void create();                       // allocates the singleton and all of its data blocks in the GameGlobal heap
    void init();                                // resets every block to the state of a fresh boot
    void initSelCharData();                     // uses g_GameGlobal, not this
    void delName(int index);                    // clears a name tag record
    void setName(int index, const wchar_t* name, int player);
    void setName1(int index, const wchar_t* name, int player, u32 date);
    bool getNameUTF16(int index, wchar_t* dst); // false when the slot is unused
    bool getName(int index, char* dst);         // the same text as UTF-8
    int getNameCapacityRest();                  // how many more name tags can be made (120 in all)
    int getNameNum(const char* name);           // the number of the tag with that text, -1 if there is none
    bool setNameRumbleSwitch(int index, u8 value);
    bool getNameRumbleSwitch(int index);
    bool isLanguageJp();                        // HYPOTHESIS: language 0 is Japanese (the text input screens test for 0)
    u16* getVsModeAutoHandiPlayerVal(int player, int nameIndex); // the handicap of a player: a tag's or one of the four slots'
    void setButtonConfig(int index, const ipButtonConfig* config);
    ipButtonConfig* getButtonConfig(int index);
    bool getRumbleSetting(int player);
    void setRumbleSetting(int player, u8 value);

    // Cluster B (gm_global_b.cpp, 0x8004DAC4-0x8004F294). Names are from the sora map unless noted; types are HYPOTHESIS.
    void initResultInfoData();                  // fills the result record with the placeholder data of the result screen test
    void InitAllRecord();                       // resets the whole record (and the names) to the factory state
    u32* getCharaAppFlag();                     // the record starts with the unlock bits of the hidden fighters
    u32* getAssistAppFlag();
    bool isGlobalRecordEventOpened(u8 list, u8 event);
    bool checkHideMenuAdditionalRule();
    bool checkHideMenuRandomStageSW();
    float getGameFrameDelta() const;
    u8 getEnableSaveLoad();                     // map: the entry at 0x8004EAD4 is named setEnableSaveLoad, but it reads the byte
    void setEnableSaveLoad(u8 flags);           // bit n = file n of the save data may be read and written
    static u32 getSaveDataSize();
    static bool checkSaveData(const void* data); // the header of a save buffer is the version (0x14) and the size
    void setSaveData(const void* data);         // buffer -> record, names and nte data (map has the wrong address a914)
    void getSaveData(void* data);               // record, names and nte data -> buffer
    static u32 getAdventureSaveDataSize();
    void setAdventureSaveData(const void* data);
    void getAdventureSaveData(void* data);

    static int getLanguage();
    static GameGlobal* getInstance();
};
static_assert(sizeof(GameGlobal) == 0x50, "Class is wrong size!");

extern GameGlobal* g_GameGlobal;
