#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_ladder.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Jungle, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Jungle, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Jungle, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
// HYPOTHESIS: unk14 is the strength of the stone blocks, unk28 / unk2C the delay / speed of the platform 12A, and unk40 to
// unk60 are the timers and chances of the scroll speed changes (updateSpeed); unk64 / unk68 / unk6C are its speeds.
struct stJungleData {
    float unk00;
    float unk04;
    float unk08;
    float unk0C;
    float unk10;
    float unk14;
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
    float unk40;
    float unk44;
    float unk48;
    float unk4C;
    float unk50;
    float unk54;
    float unk58;
    float unk5C;
    float unk60;
    float unk64;
    float unk68;
    float unk6C;
};
static_assert(sizeof(stJungleData) == 0x70, "Class is wrong size!");

// Rumble Falls (Jungle Japes): a scrolling stage. The model of the stage is a long tower of platforms ("Ashiba") that go
// by as the camera climbs; some of them have buttons, traps, spikes or break. The grounds get their positions from the
// stage (the "gimmick positions" published by grJungleBg) and report back through the byte arrays.
class stJungle : public stMelee {
    Vec3f m_limit[2];               // 0x1D8 the camera limits (lower left, upper right corner)
    Vec3f m_posGimmick[13];         // 0x1F0 the positions that grJungleBg publishes (nodes of the model)
    Vec3f m_pos28C[1];              // 0x28C
    Vec3f m_pos298[3];              // 0x298
    Vec3f m_pos2BC[1];              // 0x2BC
    Vec3f m_pos2C8[2];              // 0x2C8
    Vec3f m_pos2E0[2];              // 0x2E0
    Vec3f m_pos2F8[4];              // 0x2F8
    Vec3f m_pos328[4];              // 0x328
    Vec3f m_pos358[4];              // 0x358
    Vec3f m_posPrev[2];             // 0x388 the positions of the last frame (the camera limit and the water)
    Vec3f m_scroll;                 // 0x3A0 how far the stage scrolled in the last frame
    Vec3f m_pos3AC[3];              // 0x3AC the places of the ladders
    Matrix m_mtxN[2];               // 0x3D0 the matrices of the left and right stone blocks
    grGimmickLadderData* m_ladderData; // 0x430 the data of the three ladders
    u8 m_enable[13];                // 0x434 which platforms are on screen (from updateAshiba)
    u8 m_state[5];                  // 0x441 the states of the platforms with buttons
    char _446[2];
    float m_timer12A;               // 0x448 frames until the platform 12A moves again
    u8 m_stateN[2];                 // 0x44C the states of the left / right stone block
    u8 m_stateTrainer;              // 0x44E
    u8 m_trainerAway[4];            // 0x44F whether the trainer of the place has left
    u8 m_stateSpeed;                // 0x453 the state of the scroll speed
    float m_timerSpeed;             // 0x454
    float m_timerWarning;           // 0x458
    u8 m_warning;                   // 0x45C 1 = the "warning" sign is not shown (HYPOTHESIS)
    char _45d[3];
    float m_scrollDist;             // 0x460 how far the stage has scrolled
    u8 m_isEvent;                   // 0x464 1 = the event match
    char _465[3];
    float m_speed;                  // 0x468 the scroll speed of the event match
    int m_dangerZone;               // 0x46C the id of the danger zone of the AI

public:
    stJungle();
    static stJungle* create();

    virtual ~stJungle();
    virtual bool loading();
    virtual void createObj();
    virtual int getScrollDir(Vec3f* dir);
    virtual bool isReStartSamePoint() { return false; }
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual bool isBamperVector() { return true; }
    virtual void getBamperVector(Vec3f* vec);
    virtual void notifyEventInfoGo();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjN(int index);
    virtual void createObjNB(int index);
    virtual void createObjS(int index);
    virtual void createObjSB(int index);
    virtual void createObjWater(int index);
    virtual void createObjHashigo();
    virtual void createObjHashigo1(int index);
    virtual void createObjAttack();
    virtual void createObjWarning(int index);
    virtual void updateLimit(float deltaFrame);
    virtual void updateAI(float deltaFrame);
    virtual void updateAshiba(float deltaFrame);
    virtual void updateAshiba12A(float deltaFrame);
    virtual void updateTrainer(float deltaFrame);
    virtual void updateSpeed(float deltaFrame);

    static stClassInfoImpl<Stages::Jungle, stJungle> bss_loc_14;
};
static_assert(sizeof(stJungle) == 0x470, "Class is wrong size!");
