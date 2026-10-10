#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <revolution/GX.h>
#include <st/st_class_info.h>
#include <st/st_data_container.h>
#include <st/st_melee.h>
#include <types.h>

#include <st_mariopast/nw4r_scnproc.h>

template<typename T>
class stClassInfoImpl<Stages::Plankton, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Plankton, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Plankton, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stPlanktonData {
    float unk00;
    float unk04;    // 0x04 frames the color of a leaf stays
    float unk08;    // 0x08 the angle the leaves rest at
    float unk0C;    // 0x0C hit strength from which a leaf is pushed
    float unk10;    // 0x10 hit strength from which a leaf is pushed hard
    float unk14;    // 0x14 how far a hit pushes a leaf
    float unk18;    // 0x18 upper angle limit
    float unk1C;    // 0x1C lower angle limit
    float unk20;    // 0x20 frames between two steps of a leaf
    float unk24;    // 0x24 angle step of a leaf
    float unk28;    // 0x28 frames the flower needs to open / close
    float unk2C;    // 0x2C angle of the left leaf at rest
    float unk30;    // 0x30 hit strength from which the left leaf is pushed
    float unk34;    // 0x34 hit strength from which the left leaf is pushed hard
    float unk38;    // 0x38 how far a hit pushes the left leaf
    float unk3C;    // 0x3C
    float unk40;    // 0x40
    float unk44;    // 0x44
    float unk48;    // 0x48
    float unk4C;    // 0x4C how much a flying leaf is moved
    float unk50;    // 0x50 gravity of a flying leaf
    float unk54;    // 0x54 gravity of a leaf that bounced
    float unk58;    // 0x58 frames before the first leaf
    float unk5C;
    float unk60;    // 0x60 frames between two leaves
    float unk64;    // 0x64 chance of a leaf
    u32 unk68;      // 0x68 how often a leaf bounces
    u8 unk6C;       // 0x6C
    u8 unk6D;       // 0x6D
    char _6E[2];
    float unk70;    // 0x70 color factors of the platform by the game rule
    float unk74;
    float unk78;
    GXColor unk7C[15]; // 0x7C color of the platform (first tev color)
    GXColor unkB8[15]; // 0xB8 color of the platform (second tev color)
};
static_assert(sizeof(stPlanktonData) == 0xF4, "Class is wrong size!");

// What the leaf of the platform is doing (HYPOTHESIS: all field names).
struct stPlanktonLeaf {
    float unk00;
    float m_angle;      // 0x04 the angle of the leaf
    Vec3f m_posA;       // 0x08
    Vec3f m_posB;       // 0x14
    u8 m_hitCount;      // 0x20 how often the leaf was hit
    u8 m_wait;          // 0x21
    u8 unk22;
    u8 m_flip;          // 0x23
    u8 m_index;         // 0x24
    char _25[3];
};
static_assert(sizeof(stPlanktonLeaf) == 0x28, "Class is wrong size!");

// The timing of a group of leaves that fall one after another (from the stage data file).
struct stPlanktonHanenbouPattern {
    u16 m_frame[8]; // 0x00 the frame every leaf starts at (0 ends the group)
    float m_wait;   // 0x10 frames to wait after the group
};

struct stPlanktonHamon {
    float m_x;      // 0x00
    float m_y;      // 0x04
    u8 m_state;     // 0x08 3 = free
    u8 m_kind;      // 0x09
    char _0A[2];
};
static_assert(sizeof(stPlanktonHamon) == 0xC, "Class is wrong size!");

struct stPlanktonHanenbou {
    float m_x;      // 0x00
    float m_y;      // 0x04
    float m_prevX;  // 0x08
    float m_prevY;  // 0x0C
    float m_speedX; // 0x10
    float m_speedY; // 0x14
    float m_angle;  // 0x18
    u8 m_state;     // 0x1C 3 = free
    char _1D[3];
};
static_assert(sizeof(stPlanktonHanenbou) == 0x20, "Class is wrong size!");

struct stPlanktonWave {
    float m_height; // 0x00 how high the water is over its level
    float m_prev;   // 0x04 the height of the last frame (the force that makeWave puts)
    float m_next;   // 0x08 the height of the next frame
};
static_assert(sizeof(stPlanktonWave) == 0xC, "Class is wrong size!");

// Plankton Stage: the platforms are leaves that tilt when they are hit, the water makes waves, leaves fall from above.
class stPlankton : public stMelee {
    nw4r::g3d::ScnProc* m_scnProc;              // 0x1D8 draws the water
    nw4r::g3d::ResFile m_resFile;               // 0x1DC the textures of the water
    u8 m_intro;                                 // 0x1E0 0 = the start effect is not made yet
    u8 m_waterState;                            // 0x1E1
    u8 m_hanenbouState;                         // 0x1E2
    char _1e3;
    float m_hanenbouTimer;                      // 0x1E4
    float m_hanenbouWait;                       // 0x1E8
    stPlanktonLeaf m_leaf[10];                  // 0x1EC
    Vec3f m_posAshibaLeft[2];                   // 0x37C where the left leaf's ends are
    Vec3f m_posLimit[2];                        // 0x394 left / right limit of the camera
    Vec3f m_posLimitNow;                        // 0x3AC
    Vec3f m_posSuimen[2];                       // 0x3B8 the water level
    stPlanktonHamon m_hamon[16];                // 0x3D0
    stPlanktonHanenbou m_hanenbou[8];           // 0x490
    stPlanktonWave m_wave[192];                 // 0x590
    u8 m_stateWater;                            // 0xE90
    char _e91[3];
    stDataMultiContainer* m_container;          // 0xE94
    u8 m_hanenbouKind;                          // 0xE98
    u8 m_hanenbouCount;                         // 0xE99
    u8 m_hanenbouStep;                          // 0xE9A
    u8 m_event;                                 // 0xE9B 1 = the stage is run by the event mode

public:
    stPlankton();
    static stPlankton* create();

    virtual ~stPlankton();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjFlower();
    virtual void createObjAshiba(int index);
    virtual void createObjAshibaLeft();
    virtual void createObjHanenbou();
    virtual void createObjHamon();
    virtual void createObjWater();
    virtual void updateLimit();
    virtual void updateAshibaLeft();
    virtual void updateHanenbou(float deltaFrame);
    virtual void updateWater(float deltaFrame);
    virtual void updateWaterMakeWave(float deltaFrame);
    virtual void updateWaterCtrlWave(float deltaFrame);
    virtual void makeHamon(Vec3f* pos, u8 kind);
    virtual void makeHanenbou(float speed);
    virtual void makeWave(float x);
    virtual u8 getHamonBlankDataIndex();
    virtual u8 getHanenbouBlankDataIndex();
    virtual void playSEWater(float x);
    virtual bool isValidResTex();
    virtual nw4r::g3d::ResTex getResTex(const char* name);
    virtual void notifyEventInfoReady();
    virtual void notifyEventInfoGo();
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor();

    static void drawProc(nw4r::g3d::ScnProc* proc, bool opa);

    static stClassInfoImpl<Stages::Plankton, stPlankton> bss_loc_14;
};
static_assert(sizeof(stPlankton) == 0xE9C, "Class is wrong size!");
