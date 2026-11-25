#pragma once

// Minimal view of the Subspace Emissary stage class (sora_adv_stage's stAdventure2). Only what the gimmick units need;
// the full class is not reconstructed yet. HYPOTHESIS: the member names.

#ifdef ST_ADVENTURE2_IMPL

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <gm/gm_global_record.h>
#include <gr/gr_path.h>
#include <it/it_gen_archive.h>
#include <st/st_class_info.h>
#include <st/st_common_gimmick.h>
#include <st/st_data_container.h>
#include <types.h>

class grAdventureRebirthPos;
class stAdventureVersusItemDrop;
struct grAdventureChangeCameraPathData;
struct grGimmickAdventureLadderData;
struct grGimmickCameraScroll;
struct grAdventureTruckReactorData;
struct grAdventureItemReactorData;
struct grGimmickFrameTriggerData2;
struct grGimmickCatapultData;
struct grGimmickEffectData;
struct grGimmickQuakeData;
struct grAdventureSimpleStepJump;
struct stVersusDropItemParamCommon;

// HYPOTHESIS: BGM table entry read by getBgmID / getBgmVolume / getBgmPlayOffsetFrame.
struct stAdventure2BgmEntry {
    u32 m_stepId;
    int m_bgmId;
    u32 m_volume;
    float m_playOffsetFrame;
};
static_assert(sizeof(stAdventure2BgmEntry) == 0x10, "Class is wrong size!");

// HYPOTHESIS: parameter block at stAdventure2 + 0x1C0.
struct stAdventure2StageParam {
    u8 _0[0x7C];
    float m_pokeTrainerPosZ;       // 0x7C
    u8 m_gimmickDamageScale[15];   // 0x80
    u8 _8F[0xF];
    u8 m_enemyCreatorScale[15];    // 0x9E
    u8 _AD[0x33];
    u8 m_versusDropItemGroup[15];  // 0xE0
};

// HYPOTHESIS: gimmick data records passed to stActTriggerCreteGimmick callbacks.
struct grAdvRebirthPosData {
    float m_pos[2];
    float m_topLeft[2];
    float m_bottomRight[2];
};
static_assert(sizeof(grAdvRebirthPosData) == 0x18, "Class is wrong size!");

struct grAdventureChangeCameraPathData {
    u8 m_pathIndex;
    u8 _1[3];
    u16 m_triggerId; // 0x4 (2-byte aligned copy matching lhz/sth in createGimmickChangeCameraPath)
    u8 m_triggerFlag;
    u8 m_triggerKeep;
};
static_assert(sizeof(grAdventureChangeCameraPathData) == 8, "Class is wrong size!");

struct grGimmickNodeVisibilityData {
    u8 m_mdlIndex;      // 0x0
    u8 m_isVisible;     // 0x1
    u8 _2[2];
    char m_nodeName[0x20]; // 0x4
    SndID m_sndId;      // 0x24
    EfID m_efId;        // 0x28
    u16 m_triggerId;    // 0x2C
    u8 m_triggerFlag;   // 0x2E
    u8 m_keepKindByte;  // 0x2F
};
static_assert(sizeof(grGimmickNodeVisibilityData) == 0x30, "Class is wrong size!");

class stAdventure2 : public stCommonGimmick {
public:
    grFixedPathCollection* m_cameraPath;     // 0x1B4
    bool m_unk1B8;                           // 0x1B8
    u8 _1B9[3];
    stDataMultiContainer* m_advStageParamContainer; // 0x1BC
    stAdventure2StageParam* m_advStageParam; // 0x1C0
    u8 _1C4[0xC];
    stAdventure2BgmEntry* m_bgmTable;        // 0x1D0
    u32 m_bgmCount;                          // 0x1D4
    stDataContainer* m_destroyBossParamContainer; // 0x1D8
    u8 _1DC[0x8];
    stDataMultiContainer* m_versusDropItemContainer; // 0x1E4
    stVersusDropItemParamCommon** m_versusDropItems; // 0x1E8
    u32 m_versusDropItemCount;               // 0x1EC
    gfArchive* m_itemPacArchives[12];        // 0x1F0
    gfArchive* m_enemyFigureArchive;         // 0x220
    gfArchive* m_figureArchives[25];         // 0x224
    gfArchive* m_enemyArchives[162];         // 0x288
    gfArchive* m_enmCommonArchive;           // 0x510
    gfArchive* m_primFaceArchive;            // 0x514
    stAdventureVersusItemDrop* m_versusItemDrop; // 0x518
    void* m_jumpTable;                       // 0x51C
    u32 m_jumpTableCount;                    // 0x520
    gmAdvData* m_advData;                    // 0x524
    u32 m_stepId;                            // 0x528
    s32 m_unk52C;                            // 0x52C
    char m_startPosNodeName[0x20];           // 0x530
    union {
        u8 m_flags550;
        struct {
            bool m_flag550_0 : 1;
            bool m_flag550_1 : 1;
            bool m_flag550_2 : 1;
            bool m_flag550_3 : 1;
            bool m_flag550_4 : 1;
            bool m_flag550_5 : 1;
            bool m_flag550_6 : 1;
            bool m_flag550_7 : 1;
        };
    };                                       // 0x550
    u8 _551[3];
    // 0x554, five rows of four floats indexed by background layer: x scale, y scale, x/y/z offset
    const float* m_backGroundParams;         // 0x554
    stDataMultiContainer* m_enemyCreatorContainer; // 0x558
    u8 _55C[0x4];
    Vec3f m_rebirthPos;                      // 0x560
    Vec3f m_unk56C;                          // 0x56C
    grAdventureRebirthPos* m_rebirthPosGimmick; // 0x578
    u8 _57C[0x4];
    int m_pokeTrainerPointNum;               // 0x580
    Vec2f* m_pokeTrainerPointData;           // 0x584
    itGenSheetKind m_itemGenSheetKind;       // 0x588
    u8 _590[0x4];
    u32 m_preloadItemFlag0 : 1;              // 0x594
    u32 m_preloadItemFlag1 : 1;
    u32 m_preloadItemFlagRest : 30;
    u8 _598[0x10];
    cmSubject m_cameraSubject;               // 0x5A8
    u8 _padEnd[0x630 - 0x5A8 - sizeof(cmSubject)];

    static stAdventure2* create();
    stAdventure2();
    virtual bool loading();
    virtual ~stAdventure2();

    virtual void processAnim();
    virtual void processFixCamera();
    virtual void processEnd();
    virtual void renderOpa();

    virtual void createObj();
    virtual int getPokeTrainerPointNum();
    virtual void getPokeTrainerPointData(int* unk1, int unk2);
    virtual float getPokeTrainerPositionZ();
    virtual bool isAdventureStage();
    virtual void getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID);
    virtual void getItemGenPac(gfArchive** archive);
    virtual void getItemPacEnemyFigure(gfArchive** archive);
    virtual void getEnemyPac(gfArchive** brres, gfArchive** param, gfArchive** enmCommon, gfArchive** primFaceBrres, EnemyKind enemyKind);
    virtual void getAdvRadarData(gfArchive** out1, gfArchive** out2);
    virtual void updateStagePositions();
    virtual Rect2D* getAIRange();
    virtual void initCameraParam();
    virtual void startLoadLocalData();
    virtual bool isLoadLocalData();
    virtual int entryLocalData();
    virtual void removeLocalData();
    virtual void appearanceFighterLocal();
    virtual void getFighterStartPos(Vec3f* startPos, int fighterIndex);
    virtual void getFighterReStartPos(Vec3f* startPos, int fighterIndex);
    virtual float getFighterDeadEffectSizeRate();
    virtual float getEnemyDeadEffectSizeRate();
    virtual int getBgmID(u32 step);
    virtual int getNowStepBgmID();
    virtual float getBgmVolume();
    virtual float getBgmPlayOffsetFrame();
    virtual bool isNextStepBgmEqualNowStepBgmFromFlag();
    virtual void notifyTimmingGameStart();
    virtual bool isCameraLocked();
    virtual bool isBossBattleMode();
    virtual bool isSimpleBossBattleMode();
    virtual bool isBamperVector();
    virtual stDestroyBossParamCommon getDestroyBossParamCommon(u32);
    virtual void stAdventureEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId);
    virtual void setStageOutEffectInit(int flag);
    virtual void setStageInEffectInit();
    virtual bool helperStarWarp(bool force = false);
    virtual bool initializeFighterAttackRatio(int playerNo, float ratio);
    virtual void startFighterEvent();
    virtual stVersusDropItemParamCommon getVersusDropItemParamCommon();

    void changeStep();
    void requestStepJump(u32 jumpData);
    void createPokeTrainerRebirthList(stDataMultiContainer* container);
    void getScrollLockPoketrainerPos(Vec3f* outPos, int index);
    Ground* createGimmickNodeVisibility(grGimmickNodeVisibilityData* data);
    void setGimmickNodeVisibilityEffect(grGimmickNodeVisibilityData& data);
    bool setGimmickNodeVisibility(grGimmickNodeVisibilityData& data, Vec3f* outPos) __attribute__((never_inline));
    void changeGimmickCameraPath(grAdventureChangeCameraPathData& data);
    void setRebirthPos(const grAdvRebirthPosData& data);
    void setRebirthPosForForceScroll(Vec3f* pos);
    void changeCamearaMeleeMode(u32 index);
    void changeCameraForceTargetMode(bool unk);
    void setCameraAdvCameraTarget(Vec3f* pos);
    void setDemoCameraParam(Vec3f* pos, Vec3f* target);
    void setCameraAdvCameraOffset(Vec2f* offset);
    void stopCameraAdvCameraOffset();
    void setFighterPos(int player, Vec3f* pos);
    void setFighterStartDamage(int player);
    bool getFighterPos(int playerNo, Vec3f* outPos);
    void createItem(itGenId genId, Vec3f* pos);
    void notifyEnemyEventOnHit(int createId, u32 taskId, u32 damage, gfTask::Category category);
    void notifyEnemyEventCreateForLeftWnBossPackn(u32 createId);
    void notifyEnemyEventCreateForRightWnBossPackn(u32 createId);
    int getAttackFighterNo(u32 taskId);
    void notifyEntryDoor();
    void initForGameMode();
    void initAdventureDropItemParameterCommon(gfArchive* archive);
    void applyAdventureJumpTable(void* data, int size);
    void unResistCameraSubjectOnPositionCenter();
    void notifyScrollLockStart();
    void notifyScrollLockEnd();

    void createGimmickCameraScroll(grGimmickCameraScroll* data);

    template <class T, void (*Fn)(stAdventure2*, T*)>
    void createGimmickFunc(u32 resId, stDataMultiContainer* container);

    static stClassInfoImpl<Stages::Adventure, stAdventure2> bss_loc_14;
};
static_assert(sizeof(stAdventure2) == 0x630, "Class is wrong size!");

ST_CLASS_INFO

#else

#include <st/stage.h>
#include <types.h>

class stAdventure2 : public Stage {
    u8 m_unreconstructed[0x554 - sizeof(Stage)];

public:
    // HYPOTHESIS: the request carries an unsigned jump-table key.
    void requestStepJump(u32 jumpData);
    // 0x554, five rows of four floats indexed by background layer: x scale, y scale, x/y/z offset
    // (grAdventureBackGround::setBackGround).
    const float* m_backGroundParams;

    void stopCameraAdvCameraOffset();
    void setFighterPos(int player, Vec3f* pos);
    // HYPOTHESIS: the camera offset argument contains two floats.
    void setCameraAdvCameraOffset(Vec2f* offset);
};

#endif
