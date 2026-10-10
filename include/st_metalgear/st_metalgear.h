#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <if/if_smash_appear.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::MetalGear, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::MetalGear, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::MetalGear, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// HYPOTHESIS: the title of the intro for Snake (main, unnamed fn_800F723C)
class IfSnakeSmashAppearTask : public IfSmashAppearTask {
public:
    static IfSnakeSmashAppearTask* create(gfArchive* archive);
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stMetalgearData {
    float unk00;    // 0x00 chance of the snow of the weakest kind (the four chances are normalized)
    float unk04;    // 0x04
    float unk08;    // 0x08
    float unk0C;    // 0x0C
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;    // 0x30 frames the wall needs to rise again
    float unk34;    // 0x34 frames the wall stays broken
    float unk38;    // 0x38 chance of a metalgear
    float unk3C;    // 0x3C least frames until the metalgear comes
    float unk40;    // 0x40 most frames until the metalgear comes
    float unk44;    // 0x44 least frames the metalgear stays
    float unk48;    // 0x48 most frames the metalgear stays
    float unk4C;
    float unk50;
    float unk54;
    float unk58;
    float unk5C;
    float unk60;
    float unk64;
    float unk68;
    float unk6C;
    float unk70;
    float unk74;
    float unk78;
    float unk7C;
    float unk80;
    float unk84;    // 0x84 how fast the search light moves
    float unk88;    // 0x88 how fast the search light moves when it follows a fighter
    float unk8C;
    float unk90;
    float unk94;
    float unk98;
    float unk9C;
};
static_assert(sizeof(stMetalgearData) == 0xA0, "Class is wrong size!");

// Shadow Moses Island: the walls on both sides break and rise again, a search light looks for the fighters, and a metalgear
// (Gekko, Ray or Rex) comes from time to time. The snow (HYPOTHESIS: the kind of it is also the weather of the scene) is
// chosen when the stage is made.
class stMetalgear : public stMelee {
    IfSmashAppearTask* m_appearTask;        // 0x1D8 the title of the stage that comes before the match
    u8 m_appearStarted : 1;                 // 0x1DC
    char _1dc_rest : 7;
    char _1dd[3];
    Vec3f m_posGimmick[13];                 // 0x1E0 places the grounds share (HYPOTHESIS: [2] and [3] are the search light)
    Vec3f m_cameraRange[2];                 // 0x27C the part of the stage the camera shows
    u8 m_goFlag;                            // 0x294 the match was started
    u8 m_bgState;                           // 0x295
    u8 m_snow;                              // 0x296 which snow (3 - 6)
    u8 m_metalgearPhase;                    // 0x297
    float m_timerA;                         // 0x298
    float m_timerB;                         // 0x29C
    float m_timerC;                         // 0x2A0
    u8 m_metalgearKind;                     // 0x2A4 0 = Gekko, 1 = Ray, 2 = Rex, 9 = none
    u8 m_metalgearState;                    // 0x2A5
    u8 m_wallPhase[2];                      // 0x2A6 left / right
    float m_wallTimer[2];                   // 0x2A8
    u8 m_wallState[4];                      // 0x2B0 left up / left down / right up / right down
    float m_wallRotZ[2];                    // 0x2B4
    stCollisionWork m_collLeft;             // 0x2BC
    stCollisionWork m_collRight;            // 0x2CC
    u8 m_searchState;                       // 0x2DC
    char _2dd[3];
    s32 m_searchTarget;                     // 0x2E0 the fighter the search light follows (-1 = none)
    u8 m_exclamationState;                  // 0x2E4
    u8 m_eventA;                            // 0x2E5
    u8 m_eventB;                            // 0x2E6
    char _2e7;

public:
    stMetalgear();
    static stMetalgear* create();

    virtual ~stMetalgear();
    virtual bool loading();
    virtual void createObj();
    virtual void renderOpa();
    virtual void update(float deltaFrame);
    virtual bool startAppear();
    virtual void endAppear();
    virtual void forceStopAppear();
    virtual IfSmashAppearTask* getAppearTask() { return m_appearTask; }
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual bool isAppear();
    virtual bool isStartAppearTimming();
    virtual bool isBamperVector() { return true; }
    virtual void notifyEventInfoGo();
    virtual GXColor getFinalTechniqColor();
    virtual void createObjBg(int index);
    virtual void createObjEtc(int index);
    virtual void createObjWall(int index);
    virtual void createObjSearch(int index);
    virtual void createObjExclamation(int index);
    virtual void createObjAttack(int index);
    virtual void createObjMetalgear();
    virtual void createObjMetalgear1(int index);
    virtual void createObjSnow();
    virtual void createObjSnow1(int index);
    virtual void updateLimit();
    virtual void updateMetalgear(float deltaFrame);
    virtual void updateWallLeft(float deltaFrame);
    virtual void updateWallRight(float deltaFrame);
    virtual void updateSearch();
    virtual bool isMetalgear();
    virtual void selectSnow();

    static stClassInfoImpl<Stages::MetalGear, stMetalgear> bss_loc_14;
};
static_assert(sizeof(stMetalgear) == 0x2E8, "Class is wrong size!");
