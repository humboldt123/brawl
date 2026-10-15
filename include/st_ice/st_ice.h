#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>

// stMelee: the position of a player, his hip (sora_melee, unnamed)
extern "C" bool fn_27_2398D0(int playerNo, Vec3f* pos);

template<typename T>
class stClassInfoImpl<Stages::Ice, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Ice, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Ice, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original; fields that match the published
// stage parameter table (offset, type and use) carry descriptive names, the others are still offsets.
struct stIceData {
    float unk00;    // 0x00 warning lead distance
    float unk04;    // 0x04 frames the warning lasts
    float unk08;
    float unk0C; // HYPOTHESIS: possibly "Ice Platform Health" (matches a stage parameter table offset)
    float unk10; // HYPOTHESIS: possibly "Ice Platform Damage Taken Multiplier" (matches a stage parameter table offset)
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24; // HYPOTHESIS: possibly "Ice Platform Respawn Time" (matches a stage parameter table offset)
    float icicleInitialMin;    // 0x28 least frames until the next icicle
    float icicleInitialMax;    // 0x2C most frames until the next icicle
    float icicleRedoMin;    // 0x30 least frames until the next icicle (after the first)
    float icicleRedoMax;    // 0x34 most frames until the next icicle (after the first)
    float unk38;    // 0x38 frames the icicles come
    float unk3C; // HYPOTHESIS: possibly "Falling Platform Timer" (matches a stage parameter table offset)
    float unk40; // HYPOTHESIS: possibly "Tipping Speed" (matches a stage parameter table offset)
    float unk44; // HYPOTHESIS: possibly "Initial Tipping Speed" (matches a stage parameter table offset)
    float unk48; // HYPOTHESIS: possibly "Turnaround Angle of Tipping Platform" (matches a stage parameter table offset)
    float unk4C; // HYPOTHESIS: possibly "Tipping Platform Deceleration" (matches a stage parameter table offset)
    float unk50; // HYPOTHESIS: possibly "Tipping Platform Respawn Time" (matches a stage parameter table offset)
    float unk54; // HYPOTHESIS: possibly "Tipping Platform Return Speed" (matches a stage parameter table offset)
    float foodSpawnMin;    // 0x58 least frames until the next vegetable
    float foodSpawnMax;    // 0x5C most frames until the next vegetable
    float foodSpawnChance;    // 0x60 chance that a vegetable comes
    float unk64;    // 0x64 the chance is multiplied by this near a fighter
    float unk68;    // 0x68 distance of a fighter from the place of the vegetable
    float unk6C;
    float unk70;
    float cloudPlatformSpawnMin;    // 0x74 least frames until the next cloud platform
    float cloudPlatformSpawnMax;    // 0x78 most frames until the next cloud platform
    float unk7C; // HYPOTHESIS: possibly "Speed of Cloud Platform" (matches a stage parameter table offset)
    float unk80;    // 0x80 least frames until the next fish
    float unk84;    // 0x84 most frames until the next fish
    float unk88;    // 0x88 chance that the fish comes
    float unk8C;
    float unk90; // HYPOTHESIS: possibly "Polar Bear Jump Min Timer" (matches a stage parameter table offset)
    float unk94; // HYPOTHESIS: possibly "Polar Bear Jump Max Timer" (matches a stage parameter table offset)
    float unk98; // HYPOTHESIS: possibly "Polar Bear Jump Chance" (matches a stage parameter table offset)
    float unk9C; // HYPOTHESIS: possibly "Polar Bear Jump Magnitude Min" (matches a stage parameter table offset)
    float unkA0; // HYPOTHESIS: possibly "Polar Bear Jump Magnitude Max" (matches a stage parameter table offset)
    float unkA4; // HYPOTHESIS: possibly "Mountain Sink Acceleration" (matches a stage parameter table offset)
    float unkA8; // HYPOTHESIS: possibly "Mountain Sink Max Speed" (matches a stage parameter table offset)
    float unkAC; // HYPOTHESIS: possibly "Mountain Minimum Sunk Timer" (matches a stage parameter table offset)
    float unkB0; // HYPOTHESIS: possibly "Mountain Maximum Sunk Timer" (matches a stage parameter table offset)
    float unkB4; // HYPOTHESIS: possibly "Mountain Float Speed" (matches a stage parameter table offset)
    float unkB8;    // 0xB8 least frames until the next ice block
    float unkBC;    // 0xBC most frames until the next ice block
    float unkC0; // HYPOTHESIS: possibly "Speed Of Ascent?" (matches a stage parameter table offset)
    float unkC4; // HYPOTHESIS: possibly "Ice Floating Platform Tilt Speed" (matches a stage parameter table offset)
    float unkC8;
    float unkCC; // HYPOTHESIS: possibly "Ice Floating Platform Fast Speed" (matches a stage parameter table offset)
    float unkD0;
    float unkD4; // HYPOTHESIS: possibly "Ice Floating Platform Sinking Speed?" (matches a stage parameter table offset)
};
static_assert(sizeof(stIceData) == 0xD8, "Class is wrong size!");

// Summit: the stage scrolls up a mountain (the camera follows the scene frame of the stage model). The clouds, the ice blocks
// and the icicles come in turns, the bear walks along the top and the fish jumps out of the water. The numbers of the states
// (0xE = nothing) are shared with the grounds through the work pointers.
class stIce : public stMelee {
    u8 m_sceneState;                    // 0x1D8 the step of the scroll of the stage
    char _1d9[3];
    float m_unk1DC;                     // 0x1DC
    // MATCH-ONLY: raw storage of twelve matrices (a Matrix member would be built by an array constructor, the original sets
    // the identity in a loop)
    float m_mtxGimmick[12][12];         // 0x1E0 the places of the nodes of the model (the grounds read them)
    Vec3f m_posLimit[2];                // 0x420 the part of the stage the camera shows
    u8 m_part;                          // 0x438 the part of the mountain the scene is on
    char _439[3];
    float m_scenePrev;                  // 0x43C the scene frame of the last update
    float m_unk440;                     // 0x440
    char _444[4];
    Vec3f m_posBear[5];                 // 0x448 the places the bear walks to
    u8 m_bearState;                     // 0x484 (0xE = nothing)
    u8 m_fishState;                     // 0x485 the step of the wait of the fish
    char _486[2];
    float m_fishTimer;                  // 0x488 frames until the next fish
    float m_posFish[6];                 // 0x48C the places of the water the fish jumps from
    float m_posFishX[3];                // 0x4A4 the place the fish jumps to
    s32 m_fishIndex;                    // 0x4B0 the fighter the fish wants (-1 = none)
    u8 m_fishStateWork;                 // 0x4B4 the state of the fish (to the ground)
    u8 m_kumoState;                     // 0x4B5 the step of the wait of the clouds
    char _4b6[2];
    float m_kumoTimer;                  // 0x4B8
    u8 m_kumoStateA;                    // 0x4BC the state of the first cloud
    u8 m_kumoStateB;                    // 0x4BD the second one
    u8 m_iceState;                      // 0x4BE the step of the wait of the ice blocks
    char _4bf;
    float m_iceTimer;                   // 0x4C0
    u8 m_iceStateA;                     // 0x4C4 the state of the first ice block
    u8 m_iceStateB;                     // 0x4C5
    u8 m_turaraState;                   // 0x4C6 the step of the wait of the icicles
    char _4c7;
    float m_turaraTimer;                // 0x4C8
    u8 m_turaraStateWork;               // 0x4CC the state of the icicles (to the grounds)
    char _4cd[3];
    float m_turaraHP;                   // 0x4D0 how many hits the icicles hold
    stTrigger* m_triggerWater;          // 0x4D4 the area of the water
    grGimmickWaterData* m_waterData;    // 0x4D8
    float m_waterPrev;                  // 0x4DC the height of the water of the last update
    u8 m_yasaiState;                    // 0x4E0 the step of the wait of the vegetables
    char _4e1[3];
    float m_yasaiTimer;                 // 0x4E4
    u8 m_yasaiKind;                     // 0x4E8 the kind of the next vegetable
    char _4e9[3];
    float m_warningTimer;               // 0x4EC
    u8 m_warningState;                  // 0x4F0 the state of the warning sign (0xE = nothing)
    char _4f1[3];
    s32 m_dangerZone;                   // 0x4F4 the id of the place the fighters have to avoid (-1 = none)
    gfArchive m_archiveYasai0;          // 0x4F8 the items (one archive of the model for each kind)
    gfArchive m_archiveYasai1;          // 0x578
    gfArchive m_archiveYasai2;          // 0x5F8
    gfArchive m_archiveYasai3;          // 0x678
    gfArchive m_archiveYasai4;          // 0x6F8
    gfArchive m_archiveYasai5;          // 0x778
    gfArchive m_archiveYasai6;          // 0x7F8
    gfArchive m_archiveYasai7;          // 0x878
    gfArchive m_archiveYasai8;          // 0x8F8
    gfArchive m_archiveYasai9;          // 0x978
    gfArchive m_archiveYasaiParam;      // 0x9F8 and their parameters

public:
    stIce();
    static stIce* create();

    virtual ~stIce();
    virtual bool loading();
    virtual void createObj();
    virtual void getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID);
    virtual GXColor getFinalTechniqColor();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjYama(int index);
    virtual void createObjBreak(int index);
    virtual void createObjRot(int index);
    virtual void createObjKumo(int index);
    virtual void createObjIce(int index);
    virtual void createObjTurara(int index);
    virtual void createObjFish(int index);
    virtual void createObjWhiteBear(int index);
    virtual void createObjWater(int index);
    virtual void createObjWarning(int index);
    virtual void updateLimit(float deltaFrame);
    virtual void updateScene(float deltaFrame);
    virtual void updateTurara(float deltaFrame);
    virtual void updateKumo(float deltaFrame);
    virtual void updateIce(float deltaFrame);
    virtual void updateWater(float deltaFrame);
    virtual void updateFish(float deltaFrame);
    virtual void updateYasai(float deltaFrame);
    virtual bool getPlayerPosition(int playerNo, Vec3f* pos) { return fn_27_2398D0(playerNo, pos); }

    static stClassInfoImpl<Stages::Ice, stIce> bss_loc_14;
};
static_assert(sizeof(stIce) == 0xA78, "Class is wrong size!");
