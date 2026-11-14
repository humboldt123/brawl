// Local BrawlHeaders shadow: adds All-Star record fields in existing unions.
#pragma once
// Local BrawlHeaders shadow: names the record fields that the GameGlobal accessors return
// (names from the sora map getters; sizes and element types are HYPOTHESIS).

#include <StaticAssert.h>
#include <types.h>
#include <stddef.h>
#include <gm/gm_global_mode_melee.h>

// The per-character (and per-name) match records that gmAddDiagramResultCommon accumulates.
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

struct gmGlobalRecord {

    struct InfoAppFlagData {
        enum State {
            State_Locked = 0x0,
            State_Unlocked = 0x1,
            State_Received = 0x2,
            State_UnlockedReceived = 0x3,
        };

        State m_unlocked75mStage : 2;
        State m_unlockedLuigiMansionStage : 2;
        State m_unlockedWolfFighter : 2;
        State m_unlockedToonLinkFighter : 2;
        State m_unlockedJigglypuffFighter : 2;
        State m_unlockedSonicFighter : 2;
        State m_unlockedMrGameAndWatchFighter : 2;
        State m_unlockedGanondorfFighter : 2;
        State m_unlockedROBFighter : 2;
        State m_unlockedSnakeFighter : 2;
        State m_unlockedLucarioFighter : 2;
        State m_unlockedCaptainFalconFighter : 2;
        State m_unlockedFalcoFighter : 2;
        State m_unlockedLuigiFighter : 2;
        State m_unlockedMarthFighter : 2;
        State m_unlockedNessFighter : 2;
        State m_encounteredCelebi : 2;
        State m_encounteredMew : 2;
        State m_unlockedRandomStageChoice : 2;
        State m_unlockedAdditionalRules : 2;
        State m_unlockedBossBattlesMode : 2;
        State m_unlockedAllStarMode : 2;
        State m_unlockedBigBlueStage : 2;
        State m_unlockedPokemonStadiumStage : 2;
        State m_unlockedGreenGreensStage : 2;
        State m_unlockedJungleJapesStage : 2;
        State m_unlockedFlatZone2Stage : 2;
        State m_unlockedGreenHillZoneStage : 2;
        State m_unlockedHanenbowStage : 2;
        State m_unlockedMarioBrosStage : 2;
        State m_unlockedSpearPillarStage : 2;
        State m_unlockedPirateShipStage : 2;
        State m_filledAllGWChronicleTitles : 2;
        State m_unlockedSMWMasterpiece : 2;
        State m_unlockedOOTMasterpiece : 2;
        State m_unlockedSMB2Masterpiece : 2;
        State m_unlockedFZeroMasterpiece : 2;
        State m_unlockedDonkeyKongMasterpiece : 2;
        State m_unlockedStageBuilderPartsC : 2;
        State m_unlockedStageBuilderPartsB : 2;
        State m_unlockedStageBuilderPartsA : 2;
        State m_unlockedShadowAssist : 2;
        State m_unlockedGrayFoxAssist : 2;
        State m_unlockedBarbaraAssist : 2;
        State m_unlockedInfantryAssist : 2;
        State m_unlockedIsaacAssist : 2;
        State m_unlockedCustomRoboAssist : 2;
        State m_encounteredJirachi : 2;
        State m_have400DifferentTrophies : 2;
        State m_have300DifferentTrophies : 2;
        State m_have200DifferentTrophies : 2;
        State m_have100DifferentTrophies : 2;
        State m_gotAllSubspaceBossTrophies : 2;
        State m_gotAllSubspaceEnemyTrophies : 2;
        State m_filledAllWiiChronicleTitles : 2;
        State m_filledAllDSChronicleTitles : 2;
        State m_filledAllGCNChronicleTitles : 2;
        State m_filledAllGBAChronicleTitles : 2;
        State m_filledAllN64ChronicleTitles : 2;
        State m_filledAllVBChronicleTitles : 2;
        State m_filledAllSNESChronicleTitles : 2;
        State m_filledAllGBChronicleTitles : 2;
        State m_empty : 2;
        State m_filledAllNESChronicleTitles : 2;
        State m_clearedAllStarAllFighters : 2;
        State m_clearedAllStar : 2;
        State m_clearedSubspace : 2;
        State m_clearedClassicAllFighters : 2;
        State m_clearedClassic : 2;
        State m_have250Songs : 2;
        State m_have200Songs : 2;
        State m_have150Songs : 2;
        State m_have100Songs : 2;
        State m_have600DifferentStickers : 2;
        State m_have500DifferentStickers : 2;
        State m_have400DifferentStickers : 2;
        State m_have300DifferentStickers : 2;
        State m_have200DifferentStickers : 2;
        State m_have100DifferentStickers : 2;
        State m_have500DifferentTrophies : 2;
        State m_cleared41SoloEvents : 2;
        State m_cleared20SoloEvents : 2;
        State m_clearedBossBattlesIntense : 2;
        State m_clearedAllStarIntense : 2;
        State m_clearedAllSubspaceStagesIntense : 2;
        State m_clearedClassicIntenseOneStock : 2;
        State m_clearedClassicIntense : 2;
        State m_clearedTargetSmashLevel5AllFighters : 2;
        State m_clearedTargetSmashLevel4AllFighters : 2;
        State m_clearedTargetSmashLevel3AllFighters : 2;
        State m_clearedTargetSmashLevel2AllFighters : 2;
        State m_clearedTargetSmashLevel1AllFighters : 2;
        State m_cleared15MinuteSmash : 2;
        State m_cleared100ManSmash : 2;
        State m_clearedBossBattlesAllFighters : 2;
        State m_clearedBossBattles : 2;
        State m_played10000Matches : 2;
        State m_played1000Matches : 2;
        State m_played100Matches : 2;
        State m_gotAllOnlineFriendIcons : 2;
        State m_filledAllChronicleTitles : 2;
        State m_gotAllMasterpieces : 2;
        State m_gotAllSongs : 2;
        State m_gotAllStageBuilderParts : 2;
        State m_gotAllStickers : 2;
        State m_gotAllTrophies : 2;
        State m_gotAllAssistTrophies : 2;
        State m_gotAllSubspaceMovies : 2;
        State m_gotAllStages : 2;
        State m_gotAllFighters : 2;
        State m_cleared21CoOpEvents : 2;
        State m_cleared10CoOpEvents : 2;

        // HYPOTHESIS: the Notices screen (muProcInfo) scans 117 two-bit states, so the block is eight words, and shows
        // the time each notice arrived from the words that follow it.
        u32 unk1C;
        u32 m_recvTime[117];        // 0x20 gmGetCalendarTime count of the moment the notice was received
    };
    static_assert(sizeof(InfoAppFlagData) == 0x1F4, "Class is wrong size!");

    struct EventHiScoreData {
        u32 m_easyScores[51];
        u32 m_normalScores[51];
        u32 m_hardScores[51];
    };

    struct MenuData {

        union StageSwitch {
            struct {
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool m_dxPStadium : 1;
                bool m_dxZebes : 1;
                bool m_dxBigBlue : 1;
                bool m_dxCorneria : 1;
                bool m_dxCruise : 1;
                bool m_dxGreens : 1;
                bool m_dxOnett : 1;
                bool m_dxGarden : 1;
                bool m_dxYorster : 1;
                bool m_dxShrine : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool m_plankton : 1;
                bool m_pictChat : 1;
                bool m_greenHill : 1;
                bool m_metalGear : 1;
                bool m_village : 1;
                bool m_newPork : 1;
                bool m_famicom : 1;
                bool m_palutena : 1;
                bool m_earth : 1;
                bool m_madein : 1;
                bool m_emblem : 1;
                bool m_gW : 1;
                bool m_ice : 1;
                bool m_fZero : 1;
                bool m_tengan : 1;
                bool m_stadium : 1;
                bool m_starFox : 1;
                bool m_halberd : 1;
                bool m_crayon : 1;
                bool m_orpheon : 1;
                bool m_norfair : 1;
                bool m_oldin : 1;
                bool m_pirates : 1;
                bool m_jungle : 1;
                bool m_donkey : 1;
                bool m_kart : 1;
                bool m_marioPast : 1;
                bool m_mansion : 1;
                bool m_dolpic : 1;
                bool m_final : 1;
                bool m_battle : 1;
            };
            struct {
                u32 m_meleeMask;
                u32 m_normalMask;
            };
            // HYPOTHESIS: both masks as one doubleword: InitAllRecord and GameGlobal::setDefault set them with one
            // 64-bit store (normal word first), and MenuData pads it to an 8-byte boundary (_27).
            u64 m_mask;
        };

        u8 m_itemFrequency;
        char _1[7];
        gmItSwitch::ItemSwitch m_itemSwitch;
        union {
            char _16[10];
            struct {
                u8 m_unk10[8];   // HYPOTHESIS: eight on/off settings that start on (set to 1 by InitAllRecord)
                s8 m_volBalance; // 0x18: (s8: muProcOptSound and GameGlobal sign-extend it) balance of the music and the sound effects, 0-100 (starts at 50); applied with sndSystem::setVolBalance
                u8 m_deflicker;  // 0x19: the video deflicker setting (1 = on); applied with gfVideo::setDeflicker
            };
        };
        u8 m_language;
        char _27[5];
        StageSwitch m_stageSwitch;

        u8 m_isWidescreen : 1;  // HYPOTHESIS: u8, not bool: muProcOptScreen::proc stores the u8 cursor into it without normalizing it
        u8 m_41_6 : 1;          // HYPOTHESIS: u8 like m_isWidescreen (GameGlobal::setSaveData copies one into the other)
        bool m_41_5 : 1;
        bool m_41_4 : 1;
        bool m_41_3 : 1;
        bool m_41_2 : 1;
        bool m_41_1 : 1;
        bool m_41_0 : 1;
        char _41[3];
    };

    // HYPOTHESIS: field layout from the record initializers; meanings unknown.
    struct NameData {
        // Roles from gmAddDiagramResult (gm_common.cpp) and the Chronicle getters (mu_proc_diagram.cpp)
        u16 m_beatTotal;             // 0x00 KOs the name tag scored
        u16 m_fallTotal;             // 0x02 times the name tag was KO'd
        gmDiagramResult m_diagram;   // 0x04
        u32 m_charPlayTime[35];      // 0x54 seconds played with each character (MuCharKind)
        union {
            struct {
                u16 unkE0;
                char _E2[0xC];
                u16 unkEE;
                char _F0[0x34];
            };
            // Roles from the name accessors in gm_global.cpp (setName1, getNameUTF16, getNameRumbleSwitch,
            // getVsModeAutoHandiPlayerVal, setButtonConfig). The old names above overlay the same bytes.
            struct {
                wchar_t m_name[6];        // 0xE0 UTF-16 text of the name tag; a zero first character marks an unused slot
                u8 m_rumble;              // 0xEC rumble switch of the name tag (1 = on)
                char _ED;
                u16 m_handicap;           // 0xEE handicap kept for the tag (see getVsModeAutoHandiPlayerVal)
                u32 m_date;               // 0xF0 gmGetDateCount() at the moment the tag was made
                u8 m_buttonConfig[0x2D];  // 0xF4 the ipButtonConfig of the tag
                char _121[3];
            };
        };
    };
    static_assert(sizeof(NameData) == 0x124, "Class is wrong size!");

    // HYPOTHESIS: one 0xE0-byte record per character slot (name from getGlobalRecordCharaNumDatap)
    // HYPOTHESIS: the field names follow the getters of the Statistics screen (muProcCount: getCharUseTime,
    // getCharNumOfChamp ...); the units are unknown.
    struct CharaNumData {
        u16 m_beatCount[35];        // 0x00 KOs of each other character
        u16 m_fallCount;            // 0x46
        gmDiagramResult m_diagram;  // 0x48 the per-character records of the Chronicle diagrams
        u16 unk98;
        char _9A[0x2];
#ifdef MU_SELCHAR_UNIT_DECLS
        union SelCharView {
            struct {
                union {
                    struct {
                        u16 : 1;
                        u16 unk9C_14 : 1;
                        u16 unk9C_13 : 1;
                        u16 unk9C_12 : 1;
                        u16 unk9C_11 : 1;
                        u16 : 11;
                    };
                    struct {
                        u16 : 8;
                        u16 unk9C_7 : 3;
                        u16 : 5;
                    };
                    struct {
                        u16 : 5;
                        u16 unk9C_b10 : 3;
                        u16 : 8;
                    };
                    struct {
                        u16 unk9C_15 : 1;
                        u16 : 10;
                        u16 unk9C_4 : 3;
                        u16 : 2;
                    };
                };
                u8 _pad[0x30 - 2];
                struct {
                    u32 : 1;
                    u32 unk0_30 : 1;
                    u32 unk0_low : 30;
                } unkCC[5];
            };
            struct {
                u8 _pad0[4];
                u32 unk4;
                u32 unk8;
                u32 unkC;
                u32 unk10;
                u32 unk14;
                u32 unk18;
                u32 unk1C;
                u32 unk20;
                u32 unk24;
                u32 unk28;
                u16 unk2C;
            };
        };
#endif
        union {
            u8 unk9C[0x44];
            // HYPOTHESIS: All-Star clear difficulty and score share this record.
            struct {
                u16 : 8;
                u16 unk9C_7 : 3;
                u16 : 5;
                u8 unk9E[6];
                u32 unkA4;
            };
            struct {
                u8 _pad9C_CC[0x30];
                struct {
                    u32 : 1;
                    u32 unk0_30 : 1;
                    u32 : 30;
                } unkCC[5];
            };
            struct {
                u8 _pad9C[0x2C];
                u16 unkC8;
            };
            struct {
                u16 : 5;
                // HYPOTHESIS: highest cleared difficulty plus one.
                u16 unk9C_b10 : 3;
                u16 : 8;
                u8 _pad9E[2];
                // HYPOTHESIS: high score stored by sqSingleSimple::updateHiScore.
                u32 unkA0;
            };
            // Boss Battles record written by sqSingleBoss::setScore.
            struct {
                u16 unk9C_15 : 1;
                u16 : 10;
                u16 unk9C_4 : 3;
                u16 : 2;
                char _9E[0xA];
                u32 unkA8;
            };
#ifdef MU_SELCHAR_UNIT_DECLS
            SelCharView m_selCharView;
#endif
        };
    };
    static_assert(sizeof(CharaNumData) == 0xE0, "Class is wrong size!");

    char _0[0x18];
    u8 m_stageAppFlag[0x4];                 // 0x18

    InfoAppFlagData m_infoAppFlagData;      // 0x1C

    char _210[0x2C];
    u8 m_movieAppFlag[0x10];                // 0x23C
    u8 m_bgmAppFlag[0x1C];                  // 0x24C
    u32 m_hideMenuFlags;                    // 0x268
    u8 m_assistAppFlag[0x4];                // 0x26C
    u8 m_clearGetAppFlag[0x10];             // 0x270
    u8 m_hideEtcAppFlag[0x8];               // 0x280
    u32 m_vsTimeMeleeCount;                 // 0x288
    u32 m_vsStockMeleeCount;                // 0x28C
    u32 m_vsCoinMeleeCount;                 // 0x290
    u32 m_vsHpMeleeCount;                   // 0x294
    u32 m_onlineVsTimeMeleeCount;           // 0x298
    u32 m_onlineVsStockMeleeCount;          // 0x29C
    u32 m_onlineVsCoinMeleeCount;           // 0x2A0
    u32 m_vsResetMeleeCount;                // 0x2A4
    u32 m_vsPartyPlayerCount;               // 0x2A8
    u32 m_vsMeleeTimeCount;                 // 0x2AC
    u32 m_vsMeleeTimeTotal;                 // 0x2B0
    u32 m_singleMeleeTimeCount;             // 0x2B4
    u32 m_singleMeleeTimeTotal;             // 0x2B8
    u32 m_onlineFriendTimeCount;            // 0x2BC
    u32 m_onlineAnybodyTimeCount;           // 0x2C0
    u32 m_onlineAnybodyWatchCount;          // 0x2C4
    u32 m_figCoin;                          // 0x2C8
    u32 m_getFigCoin;                       // 0x2CC
    u32 m_bootCount;                        // 0x2D0
    u32 m_workingTime;                      // 0x2D4
    u32 m_damageTotalCount;                 // 0x2D8
    u32 m_killTotalCount;                   // 0x2DC
    u32 m_suicideTotalCount;                // 0x2E0
    // The rarity bit sets of the trophy and the sticker lotteries (gmGetRecFigureRareBit / gmGetRecSealRareBit); the
    // melee init data keeps a copy of each for the match.
    u8 m_figureRareBit;                     // 0x2E4
    u8 m_sealRareBit;                       // 0x2E5
    u8 m_openedEvent[0x2];                  // 0x2E6
    u8 m_stagePlayCount[0x43];              // 0x2E8
    u8 m_friendCount;                       // 0x32B (gmSetFriendCount / gmGetFriendCount, 0-64)

    EventHiScoreData m_eventHiScoreData[2];

    // HYPOTHESIS: the Adventure play time in frames; the Statistics screen divides them by 60 and adds them to the
    // single-player time count and total (the names follow muProcCount::getAdventureAllPlayTime / NobePlayTime).
    u32 m_advTimeCount;                     // 0x7F4
    u32 m_advTimeTotal;                     // 0x7F8
    char _7FC[8];
    // HYPOTHESIS: the sound test shows the last categories of the sound effects only when it is set.
    u32 m_unk804_31 : 1;                    // 0x804
    u32 : 31;
    bool m_unk808_7 : 1;                    // 0x808; HYPOTHESIS: bit 7 is copied into the melee init data by gmInitMeleeDataDefault
    bool m_unk808_6 : 1;
    bool m_unk808_5 : 1;
    bool m_unk808_4 : 1;
    bool m_unk808_3 : 1;
    bool m_unk808_2 : 1;
    bool m_unk808_1 : 1;
    bool m_unk808_0 : 1;
    char _809[7];

    MenuData m_menuData; // 2064

    // MenuData ends at 0x840: its doubleword switches make it 8-byte aligned.
    char _840[2];
    // One halfword per trophy (index from gmFig_Id2Index) and per sticker (gmSeal_Id2Index): bit 15 owned, bit 14
    // referred, bit 13 info, bit 12 info referred, bits 0-9 the number owned (stops at 999).
    // HYPOTHESIS: the sticker table runs up to the next known field (1000 entries). Each table is its own struct
    // (names invented): the writers of gm_common.cpp take the table's address and index from that base.
    struct FigureData {
        u16 m_flags[0x28B];
    } m_figureData;                         // 0x842
    struct SealData {
        u16 m_flags[0x3E8];
    } m_sealData;                           // 0xD58
    u8 unk1528[0x81];                       // 0x1528: cleared with the Masterpieces flags; meaning unknown
    char _15A9[3];
    // Two flags for each title of the Masterpieces list (muSoftListDataManager): bit 2 * n it can be played, bit
    // 2 * n + 1 it is new.
    u32 m_softFlags[0x22];                  // 0x15AC
    CharaNumData m_charaNumData[35];        // 0x1634
    union {
        u8 unk34D4[0x74];
        // HYPOTHESIS: cooperative All-Star difficulty, score, characters and colors.
        struct {
            u16 : 5;
            u16 unk34D4_10 : 3;
            u16 : 6;
            u16 unk34D4_1 : 1;
            u16 : 1;
            u8 unk34D6[2];
            u32 unk34D8;
            u8 unk34DCpad[0x48];
            u8 unk3524;
            u8 unk3525;
            u8 unk3526;
            u8 unk3527;
        };
        struct {
            u8 _pad34D4_34FC[0x28];
            struct {
                u32 unk0;
                u32 unk4;
            } unk34FC[5];
        };
        struct {
            u16 unk34D4_15 : 1;
            u16 : 7;
            u16 unk34D4_7 : 3;
            u16 : 4;
            u16 unk34D4_0 : 1;
            char _34D6[6];
            u32 unk34DC;
            char _34E0[0x48];
            u8 unk3528;
            u8 unk3529;
            u8 unk352A;
            u8 unk352B;
        };
    };
#ifdef MU_SELCHAR_UNIT_DECLS
    union {
        u8 unk3548[0x44];
        struct {
            union {
                struct {
                    u16 : 1;
                    u16 unk0_14 : 1;
                    u16 unk0_13 : 1;
                    u16 unk0_12 : 1;
                    u16 unk0_11 : 1;
                    u16 unk0_8 : 3;
                    u16 unk0_7 : 1;
                    u16 unk0_6 : 1;
                    u16 unk0_5 : 1;
                    u16 unk0_4 : 1;
                    u16 unk0_3 : 1;
                    u16 unk0_2 : 1;
                    u16 unk0_1 : 1;
                    u16 unk0_0 : 1;
                };
            } flags;
            u8 _pad354A[2];
            u32 scores[8];
            struct {
                u8 char0;
                u8 color0;
                u8 char1;
                u8 color1;
            } pairs[8];
        } m_onlinePairRecord;
    };
#else
    u8 unk3548[0x44];
#endif
    char _358C[0x764];
};
static_assert(offsetof(gmGlobalRecord, m_infoAppFlagData) == 0x1C, "Class is wrong size!");
static_assert(offsetof(gmGlobalRecord, m_eventHiScoreData) == 0x32C, "Class is wrong size!");
static_assert(offsetof(gmGlobalRecord, m_figureData) == 0x842, "Class is wrong size!");
static_assert(offsetof(gmGlobalRecord, m_sealData) == 0xD58, "Class is wrong size!");
static_assert(offsetof(gmGlobalRecord, m_friendCount) == 0x32B, "Class is wrong size!");
static_assert(offsetof(gmGlobalRecord, m_softFlags) == 0x15AC, "Class is wrong size!");
static_assert(offsetof(gmGlobalRecord, m_charaNumData) == 0x1634, "Class is wrong size!");
static_assert(sizeof(gmGlobalRecord) == 0x3cf0, "Class is wrong size!");

struct nteGlobalData {
    char _spacer[0x88]; // 0x00
};
static_assert(sizeof(nteGlobalData) == 0x88, "Class is wrong size!");


// The Subspace Emissary save data. Names are HYPOTHESIS, from the readers (map menu, item generator, keep manager,
// save point, result screens) and from GameGlobal's reset.
struct gmAdvData {
    // A place of the map: hidden (1) or shown, its kind (0 not open, 1..4), the value its model shows.
    struct Level {
        int m_hidden;  // 0x00
        int m_kind;    // 0x04
        int m_value;   // 0x08
        int m_percent; // 0x0C
        int m_unk10;   // 0x10
    };
    struct Slot {
        s32 m_id;
        u8 m_value;
    };
    // The controller the adventure is played with.
    // BYTES: the flag is a union with its word: the scene units copy the structure word by word through
    // temporaries on the stack (with a plain bool the copies are folded away).
    struct PadInfo {
        union Flag {
            bool m_isSet;
            u32 m_word;
        } m_flag;
        s32 m_padNo;
    };
    // One-bit flags.
    struct Flags {
        u8 m_unk0 : 1;
        u8 m_unk1 : 1;
        u8 m_unk2 : 1;
        u8 m_unk3 : 1;
        u8 m_unk4 : 1;
        u8 m_unk5 : 1;
        u8 m_unk6 : 1;
        u8 m_unk7 : 1;
    };

    Level m_levels[34];          // 0x000
    float m_unk2A8;              // 0x2A8
    u16 m_mapFlags;              // 0x2AC: the parts of the map that are shown
    char _2AE[6];
    u8 unk2B4;                   // 0x2B4
    u8 unk2B5;
    char _2B6[3];
    u8 unk2B9[0x27];             // 0x2B9
    Slot m_slots[99];            // 0x2E0
    union {
        char _5F8[2];
        u16 unk5F8;
    };
    u8 unk5FA;                   // 0x5FA
    char _5FB;
    u8 unk5FC;                   // 0x5FC
    u8 m_difficultyLevel;        // 0x5FD
    u8 unk5FE;
    u8 unk5FF;
    u8 m_nameIndex;              // 0x600: the name record of the player (0x78 none)
    u8 unk601;
    char _602;
    u8 m_saveRequest;            // 0x603: 7 asks for a save
    u8 m_mode;                   // 0x604
    union {
        char _605[7];
        struct {
            char _605b;
            u8 unk606_7 : 1;
            u8 : 7;
            char _607[5];
        };
    };
    u32 unk60C;                  // 0x60C
    char _610[0xC];
    u32 unk61C;                  // 0x61C
    int m_itemGenKind;           // 0x620: the item generator sheet of the stage
    s32 unk624;
    u32 unk628;
    u32 unk62C;
    char _630[0x20];
    u32 m_globalFlags[0x20];     // 0x650
    char _6D0[0x41C8];
    u32 unk4898[24];             // 0x4898: four words, then ten pairs (GameGlobal's constructor)
    u32 unk48F8;                 // 0x48F8
    char _48FC[0x14];
    u32 unk4910;                 // 0x4910 (the saved data ends here)
    bool m_isNanaDead[2];        // 0x4914: per player
    u8 unk4916;                  // 0x4916: subtracted from the difficulty level
    u8 unk4917;
    u32 unk4918;
    u32 unk491C;
    u16 m_stageCoin;             // 0x4920: coins picked up on the stage (times 10 when added)
    char _4922[2];
    Flags m_flags;               // 0x4924
    char _4925[3];
    PadInfo m_padInfo[2];        // 0x4928
    char _4938[0x20];
};
static_assert(sizeof(gmAdvData) == 0x4958, "Class is wrong size!");
