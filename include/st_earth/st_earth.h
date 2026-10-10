#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Earth, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Earth, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Earth, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stEarthData {
    float unk00;   // 0x00 frames until the first pellet is dropped (the least)
    float unk04;   // 0x04 (the most)
    float unk08;   // 0x08 least frames until the next pellet
    float unk0C;   // 0x0C most frames until the next pellet
    float unk10;   // 0x10 the chance that a pellet is dropped
    float unk14;   // 0x14 the weight of the first color of the pellets
    float unk18;   // 0x18 the weight of the second color
    float unk1C;   // 0x1C the weight of the third color
    float unk20;   // 0x20 the factor of the color that was dropped last
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
    float unk38;   // 0x38 how near to a pellet the pellet can be dropped
    u8 unk3C;       // 0x3C how many pellets
    char _3d[3];
    float unk40;   // 0x40 frames until the first oniyon comes (the least)
    float unk44;   // 0x44 (the most)
    float unk48;   // 0x48 least frames until the next oniyon
    float unk4C;   // 0x4C most frames until the next oniyon
    float unk50;   // 0x50 the chance that the oniyon waits
    float unk54;   // 0x54 the weight of the first color of the oniyon
    float unk58;   // 0x58 the weight of the second color
    float unk5C;   // 0x5C the weight of the third color
    float unk60;   // 0x60 the factor of the color that was shown last
    float unk64;
    float unk68;   // 0x68 least frames that the oniyon stays
    float unk6C;   // 0x6C most frames that the oniyon stays
    float unk70;
    float unk74;
    float unk78;
    float unk7C;
    float unk80;
    float unk84;
    float unk88;   // 0x88 frames until the first chappy comes (the least)
    float unk8C;   // 0x8C (the most)
    float unk90;   // 0x90 least frames until the next chappy
    float unk94;   // 0x94 most frames until the next chappy
    float unk98;   // 0x98 least frames until the chappy leaves
    float unk9C;   // 0x9C most frames until the chappy leaves
    float unkA0;
    float unkA4;
    float unkA8;
    float unkAC;
    float unkB0;
    float unkB4;   // 0xB4 frames until the first rain (the least)
    float unkB8;   // 0xB8 (the most)
    float unkBC;   // 0xBC least frames of the fine weather
    float unkC0;   // 0xC0 most frames of the fine weather
    float unkC4;   // 0xC4 the chance of the rain
    float unkC8;   // 0xC8 frames of the sun
    float unkCC;
    float unkD0;
    float unkD4;   // 0xD4 least frames until the rain ends
    float unkD8;   // 0xD8 most frames until the rain ends
};
static_assert(sizeof(stEarthData) == 0xDC, "Class is wrong size!");

// One pellet of the stage (a pellet is dropped on a leaf, the fighters can take it).
struct stEarthPelletData {
    u8 m_state;         // 0x00 0 = not made, 1 = made, 2 = falls, 3 = on a leaf
    char _1[3];
    float unk04;        // 0x04
    u8 unk08;           // 0x08
    u8 m_leaf;          // 0x09 the leaf that it falls on (4 = none yet)
    u8 m_color;         // 0x0A
    u8 unk0B;           // 0x0B
    float unk0C;        // 0x0C
    float unk10;        // 0x10
    Vec3f m_pos;        // 0x14
    float unk20;        // 0x20
    float unk24;        // 0x24
    Matrix m_mtx;       // 0x28

    stEarthPelletData() : m_mtx(true) { }
};
static_assert(sizeof(stEarthPelletData) == 0x58, "Class is wrong size!");

// The area of the belt conveyor that is a triangle (the grounds of the stage have no other one).
struct stEarthBeltData {
    stGimmickAreaData m_areaData;   // 0x00
    Vec2f m_points[3];              // 0x28
    float m_speed;                  // 0x40
    bool m_isRight;                 // 0x44
    char _45[3];
    stTriggerData m_triggerData;    // 0x48
};
static_assert(sizeof(stEarthBeltData) == 0x4C, "Class is wrong size!");

class Ground;

// Rainy Earth (Distant Planet): the stage of the Pikmin. The fighters stand on the leaves, the stump and the branch while the
// weather changes (rain, a sun and the river) and an oniyon, a chappy and the pellets come and go. The stage keeps the state
// of all of it and the grounds read it from the work pointers.
class stEarth : public stMelee {
    Vec3f m_posGimmick[3];              // 0x1D8 the places of the pellet on the stump
    Vec3f m_posLeaf[3];                 // 0x1FC the offset of the three leaves
    u8 m_stateOniyon;                   // 0x220 the step of the oniyon (0 = choose, 1 = wait, 2 = comes, 3 = stays, 4 = goes)
    char _221[3];
    float m_timerOniyon;                // 0x224 frames until the next step
    u8 m_colorOniyon;                   // 0x228
    u8 m_firstOniyon;                   // 0x229
    u8 m_leafOniyon;                    // 0x22A the leaf that the oniyon comes to
    u8 m_stateWeather;                  // 0x22B the step of the weather
    float m_timerWeather;               // 0x22C
    u8 m_weather;                       // 0x230 the animation of the weather (0 = fine, 1 = rain, 2 ..)
    char _231[3];
    float m_timerSun;                   // 0x234
    u8 m_firstWeather;                  // 0x238
    u8 m_sunOn;                         // 0x239
    char _23a[2];
    float m_riverSpeed;                 // 0x23C how fast the river runs (the belt conveyor follows)
    u32 m_efRain[4];                    // 0x240 the effects of the weather
    s32 m_seHandle;                     // 0x250 the sound of the weather (-1 = none)
    u8 m_stateChappy;                   // 0x254
    char _255[3];
    float m_timerChappy;                // 0x258
    u8 m_firstChappy;                   // 0x25C
    char _25d[3];
    stCollisionWork m_collWork;         // 0x260 the collision of the chappy
    u8 m_statePellet;                   // 0x270
    char _271[3];
    float m_timerPellet;                // 0x274
    stEarthPelletData* m_pelletData;    // 0x278 the pellets (as many as the stage data says)
    u8 m_firstPellet;                   // 0x27C
    u8 m_lastColor;                     // 0x27D
    char _27e[2];
    stTrigger* m_triggerBelt;           // 0x280 the area of the belt conveyor
    stEarthBeltData* m_beltData;        // 0x284
    gfArchive m_archivePelletParam;     // 0x288 the parameters of the pellet item (from the file 0x2712)
    gfArchive m_archivePelletBrres;     // 0x308 the model of the pellet item (from the file 0x2711)

public:
    stEarth();
    static stEarth* create();

    virtual ~stEarth();
    virtual bool loading();
    virtual void createObj();
    virtual void getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID);
    virtual bool isBamperVector() { return true; }
    virtual void update(float deltaFrame);
    virtual void createObjOniyon(int index);
    virtual void createObjStump(int index);
    virtual void createObjBranch(int index);
    virtual void createObjLeaf(int index);
    virtual void createObjChappy();
    virtual void createObjPellet();
    virtual void createObjPellet1(int index);
    virtual void createObjPelletFlower(int index);
    virtual void createObjRain(int index);
    virtual void createObjRiver(int index);
    virtual void createObjHaneMizu(int index);
    virtual void createObjBeltConv();
    virtual void updateOniyon(float deltaFrame);
    virtual void updateWeather(float deltaFrame);
    virtual void updateChappy(float deltaFrame);
    virtual void updatePellet(float deltaFrame);
    virtual void updateBelt(float deltaFrame);
    virtual void initPelletData(stEarthPelletData* pellet);
    virtual int selectPelletColor(int lastColor);
    virtual int getCountPelletRideLeaf();
    virtual bool isEnableSpacePellet(float rate);

    static stClassInfoImpl<Stages::Earth, stEarth> bss_loc_14;
};
static_assert(sizeof(stEarth) == 0x388, "Class is wrong size!");
