#pragma once

// Gimmick data record views and forward declarations used solely by st_adventure2.cpp.
// Type names match the sora_adv_stage symbol map (createGimmickFunc<T, ...>).

#include <ec/ec_mgr.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_ladder.h>
#include <gr/gr_yakumono.h>
#include <sora_adv_stage/gr_adventure_door.h>
#include <sora_adv_stage/gr_adventure_door_bomb_factory.h>
#include <sora_adv_stage/gr_adventure_move_attack_ground.h>
#include <sora_adv_stage/gr_adventure_notify_gimmick_trigger_status.h>
#include <st/st_trigger.h>
#include <st/st_trigger_actor_common.h>
#include <types.h>

class grAdventureDoorBoss : public grAdventureDoor {
public:
    static grAdventureDoorBoss* create(int mdlIndex, u32 jumpData, const char* taskName);
};

class grAdventureDoorThreePin : public grAdventureDoorBombFactory, public grAdventureNotifyGimmickTriggerStatus {
public:
    static grAdventureDoorThreePin* create(int mdlIndex, u32 jumpData, const char* taskName);
};

class grAdventureNewItemBox : public grYakumono {
public:
    static grAdventureNewItemBox* create(int mdlIndex, const char* taskName);
};

class grAdventurKingStatue : public grAdventureMoveAttackGround {
public:
    static grAdventurKingStatue* create(int mdlIndex, const char* taskName);
};

struct grAdventureEnemyCreatorData {
    u8 _0[0x78];
    float m_baseCount; // 0x78
    u8 m_mdlIndex;     // 0x7C
    u8 _7D[0x13];
    s16 m_scaledCount; // 0x90
};

struct grGimmickFinalData {
    u8 m_mdlIndex; // 0x0
};

struct grGimmickNewItemBlockData {
    u8 _0[0x2C];
    u8 m_mdlIndex;           // 0x2C
    u8 m_collIndex;          // 0x2D
    u8 _2E[0x16];
    union {
        stTriggerData m_trigger; // 0x44
        struct {
            u16 m_triggerId;
            u8 m_triggerFlag;
            u8 m_keepKindByte;
        };
    };
};

struct grGimmickKingStatueData {
    u8 _0[0x44];
    u8 m_mdlIndex;                      // 0x44
    u8 _45[0x1A7];
    grGimmick::AttackData m_attackData; // 0x1EC
};

struct grAdventureTikuwaGroundData {
    u8 _0[0x20];
    u8 m_mdlIndex;  // 0x20
    u8 m_collIndex; // 0x21
};

struct grAdventureElasticityGroundData {
    u8 _0[0x1C];
    u8 m_mdlIndex;  // 0x1C
    u8 m_collIndex; // 0x1D
};

struct grGimmickFallingRockData {
    u8 _0[0x44];
    u8 m_mdlIndex;                      // 0x44
    u8 m_collIndex;                     // 0x45
    u8 _46[0x1A6];
    grGimmick::AttackData m_attackData; // 0x1EC
};

struct grGimmickShutterData {
    u8 _0[0x44];
    u8 m_mdlIndex;  // 0x44
    u8 m_collIndex; // 0x45
};

struct grGimmickMoveGroundData {
    u8 _0[0x44];
    u8 m_mdlIndex;  // 0x44
    u8 m_collIndex; // 0x45
};

struct grGimmickSympatheticsBlockData {
    u8 _0[0x2C];
    u8 m_mdlIndex; // 0x2C
};

struct grGimmickBrokenBlockData {
    u8 _0[0x2C];
    u8 m_mdlIndex; // 0x2C
};

struct grGimmickMoveEffectAttackGroundData {
    u8 _0[0x44];
    u8 m_mdlIndex;                      // 0x44
    u8 _45[0x1A7];
    grGimmick::AttackData m_attackData; // 0x1EC
    u8 _244[4];
    EfID m_effectId;                    // 0x248
};

struct grGimmickMoveCircleAttackGroundData {
    u8 _0[0x44];
    u8 m_mdlIndex; // 0x44
};

struct grGimmickAdventureLadderData : public grGimmickLadderData {
    float m_difficultyRatios[15]; // 0x5C
};

struct grGimmickCameraScroll {
    u8 _0[4];
};

struct grAdventureItemReactorData : public stGimmickAreaData {
    Vec2f m_pos;             // 0x28
    u8 _30[4];
    union {
        stTriggerData m_trigger; // 0x34
        struct {
            u16 m_triggerId;
            u8 m_triggerFlag;
            u8 m_keepKindByte;
        };
    };
};

struct grGimmickEffectData {
    u8 _0[0x1C];
    stTriggerData m_trigger; // 0x1C
};

struct grGimmickQuakeData {
    u8 _0[0x10];
    stTriggerData m_trigger; // 0x10
};

struct grAdventureSimpleStepJump {
    u8 _0[0x8];
    stTriggerData m_trigger; // 0x8
};

struct stVersusDropItemParamCommon {
    u8 unk0;
    u8 unk1;
    u16 unk2;
    u16 unk4;
    u16 unk6;

    stVersusDropItemParamCommon() {
        unk0 = 0;
        unk2 = 0;
        unk4 = 0;
        unk1 = 1;
    }
};
static_assert(sizeof(stVersusDropItemParamCommon) == 8, "Class is wrong size!");

struct grGimmickFrameTriggerData2 : public stActTriggerFrameCBData {
};

struct grAdventureDoorBossData : public grGimmickDoorData {
};

struct grAdventureDoorThreePinData : public grGimmickDoorData {
};

struct grAdventureDoorBombFactoryData : public grGimmickDoorData {
};

struct grGimmickBarrelCannnonStaticData {
    u8 _0[0xCC];
    u8 m_mdlIndex; // 0xCC
};

struct grGimmickBarrelCannnonPathData {
    u8 _0[0xCC];
    u8 m_mdlIndex; // 0xCC
};
