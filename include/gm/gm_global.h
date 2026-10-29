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

class GameGlobal {
public:
    /* data */
    gmAppData* m_appData;               // 0x00
    char _0[0x4];                       // 0x04
    gmGlobalModeMelee* m_modeMelee;     // 0x08
    char _1[0x04];                      // 0x0C
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
    char _4[0x04];                      // 0x38
    gmGlobalCorps* m_corps;             // 0x3C
    gmStageEditData* m_stageEditData;   // 0x40
    gmStageData* m_stageData;           // 0x44
    char _6[0x8];                       // 0x48

    gmGlobalRecord::MenuData* getGlobalRecordMenuDatap();
    // Accessors below: names from the sora map; return types are HYPOTHESIS.
    gmGlobalRecord::NameData* getGlobalRecordNameDatap(u8 index); // HYPOTHESIS: u8 from the inlined use in InitBattleRecord; target symbol stays fn_8004D138 (called by that name from mu_name.cpp)
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

    static int getLanguage();
    static GameGlobal* getInstance();
};
static_assert(sizeof(GameGlobal) == 0x50, "Class is wrong size!");

extern GameGlobal* g_GameGlobal;
