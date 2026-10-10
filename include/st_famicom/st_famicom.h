#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Famicom, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Famicom, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Famicom, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stFamicomData {
    float unk00;
    float unk04;
    float unk08;
    float unk0C;    // 0x0C frames the POW block needs to come back
    float unk10;
    float unk14;    // 0x14 frames until the first enemy
    float unk18;    // 0x18 least frames until the next enemy
    float unk1C;    // 0x1C most frames until the next enemy
    float unk20;    // 0x20 chance of the first kind of enemy (the second one has the rest)
    float unk24;    // 0x24
    u8 m_enemyMax;  // 0x28 how many enemies the stage can have at the same time
    char _29[3];
    float unk2C;
    float unk30;
    float unk34;    // 0x34 gravity of the enemies
    float unk38;
    float unk3C;
    float unk40;    // 0x40 walking speed of the enemies (the second level)
    float unk44;    // 0x44 walking speed of the enemies (the fourth level)
    float unk48;    // 0x48 frames an enemy waits after it lands
    float unk4C;    // 0x4C frames until the first green ball
    float unk50;    // 0x50 least frames until the next one
    float unk54;    // 0x54 most frames until the next one
    float unk58;
    float unk5C;    // 0x5C
    float unk60;    // 0x60 chance that a ball is not thrown
};

// The record of an enemy of the stage (the stage owns them, the ground of the enemy writes into its own).
struct stFamicomEnemyData {
    Vec3f m_pos;        // 0x00
    Vec3f m_speed;      // 0x0C
    u8 m_dir;           // 0x18 1 = it walks to the right
    u8 m_grounded;      // 0x19
    char _1a[2];
    float m_timer;      // 0x1C
    float m_hitDir;     // 0x20 the side the last hit came from (-1 = none)
    u8 m_state;         // 0x24 0xD = free
    char _25[3];

    stFamicomEnemyData() { }
};
static_assert(sizeof(stFamicomEnemyData) == 0x28, "Class is wrong size!");

// What the floors tell the grounds about a fighter who bumped into them from below.
struct stFamicomYukaToss {
    float m_x;      // 0x00
    float m_y;      // 0x04
    float m_z;      // 0x08
    float m_value;  // 0x0C
    u8 m_state;     // 0x10 0xD = nothing
    char _11[3];
};
static_assert(sizeof(stFamicomYukaToss) == 0x14, "Class is wrong size!");

// The data of a pressure area of the stage (it is made by the stage and given to the area: the area of the floor, of the POW
// block). Nothing is named by the original.
struct stFamicomPressureData {
    char _00[0x18];
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
    float unk38;
    float unk3C;
    u8 unk40;
    char _41[7];
};
static_assert(sizeof(stFamicomPressureData) == 0x48, "Class is wrong size!");

// Flat Zone 2: the floors the enemies walk on, the POW block under them and the green balls. The stage makes the enemies
// (a shellcreeper or a sidestepper) and the balls from the stage data and tells the grounds where everything is.
class stFamicom : public stMelee {
    Vec3f m_posLimit[2];                // 0x1D8 the part of the stage the camera shows
    u8 m_stateArea;                     // 0x1F0 the state of the area under the floors (0xD = nothing)
    char _1f1[3];
    float m_landingWork[4];             // 0x1F4 how long the fighters stood on the four floors
    float m_landingStock[4];            // 0x204
    u8 m_landingFlg[4];                 // 0x214 the fighters stand on the right side
    stCollisionWork m_collWork[8];      // 0x218 the collisions of the floors (center, side A..F) and of the POW block
    Vec3f m_posYuka[7];                 // 0x298 the places of the seven floors
    stFamicomYukaToss m_yukaToss[7];    // 0x2EC what the fighters did to the floors
    stTrigger* m_triggerYuka[4];        // 0x378 the pressure areas of the floors
    stFamicomPressureData* m_pressureYuka[4]; // 0x388
    Vec3f m_posPow;                     // 0x398 the place of the POW block
    u8 m_powState;                      // 0x3A4 the state of the block (8 = it is hit, 0xD = nothing)
    char _3a5[3];
    stTrigger* m_triggerPow;            // 0x3A8
    stFamicomPressureData* m_pressurePow; // 0x3AC
    u8 m_ballState;                     // 0x3B0
    char _3b1[3];
    float m_ballTimer;                  // 0x3B4 frames until the next ball
    float m_ballWait;                   // 0x3B8 the wait that was chosen
    Vec3f m_posBall[4];                 // 0x3BC the places of the green balls
    u8 m_ballKind;                      // 0x3EC the place the last ball came from (0xD = none)
    u8 m_ballKindPrev;                  // 0x3ED
    u8 m_ballLtoR;                      // 0x3EE a ball rolls from the left to the right
    u8 m_ballLtoRPrev;                  // 0x3EF
    u8 m_enemyState;                    // 0x3F0
    char _3f1[3];
    float m_enemyTimer;                 // 0x3F4 frames until the next enemy
    Vec3f m_posEnemy[4];                // 0x3F8 the places the enemies come from and go to
    stFamicomEnemyData* m_enemyData;    // 0x428 the records of the enemies (one per enemy the stage can have)
    gfArchive m_archiveShellBrres;      // 0x42C the item of a shellcreeper (the model)
    gfArchive m_archiveShellParam;      // 0x4AC (its parameters)
    gfArchive m_archiveCrabBrres;       // 0x52C the item of a sidestepper
    gfArchive m_archiveCrabParam;       // 0x5AC

public:
    stFamicom();
    static stFamicom* create();

    virtual ~stFamicom();
    virtual bool loading();
    virtual void createObj();
    virtual void getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID);
    virtual GXColor getFinalTechniqColor();
    virtual bool isBamperVector() { return true; }
    virtual void notifyEventInfoGo();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjYukaCenter(int index);
    virtual void createObjYukaSideA(int index);
    virtual void createObjYukaSideB(int index);
    virtual void createObjYukaSideC(int index);
    virtual void createObjYukaTrigger();
    virtual void createObjPow(int index);
    virtual void createObjBall(int index);
    virtual void createObjEnemy();
    virtual void updateLimit();
    virtual void updateEnemy(float deltaFrame);
    virtual void updateBall(float deltaFrame);
    virtual void updatePow(float deltaFrame);
    virtual void updateYuka(float deltaFrame);
    virtual void initEnemyData(stFamicomEnemyData* data);
    virtual u8 getCountEnemy();
    virtual u8 getEmptyIndexEnemyTbl();
    virtual void makeEnemy();

    static stClassInfoImpl<Stages::Famicom, stFamicom> bss_loc_14;
};
static_assert(sizeof(stFamicom) == 0x62C, "Class is wrong size!");
