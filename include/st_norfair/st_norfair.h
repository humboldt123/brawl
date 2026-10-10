#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_data_container.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Norfair, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Norfair, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Norfair, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stNorfairData {
    float unk00;    // 0x00 how much the chance of the same event twice is multiplied by
    float unk04;    // 0x04 frames left in a timed match under which no event starts
    float unk08;    // 0x08
    float unk0C;    // 0x0C
    float unk10;    // 0x10
    float unk14;    // 0x14
    float unk18;    // 0x18
    float unk1C;    // 0x1C
    float unk20;    // 0x20
    float unk24;    // 0x24
    float unk28;    // 0x28
    float unk2C;    // 0x2C
    float unk30;    // 0x30
    float unk34;    // 0x34
    float unk38;    // 0x38
    float unk3C;    // 0x3C
    float unk40;    // 0x40
    float unk44;    // 0x44
    float unk48;    // 0x48
    float unk4C;    // 0x4C
    float unk50;    // 0x50
    float unk54;    // 0x54
    float unk58;    // 0x58
    float unk5C;    // 0x5C
    float unk60;    // 0x60
    float unk64;    // 0x64
    float unk68;    // 0x68
};
static_assert(sizeof(stNorfairData) == 0x6C, "Class is wrong size!");

// Norfair: lava rises and falls behind the platforms. A scheduler picks the next event from a table of the stage data:
// fireballs that burst from the lava (kind 1), a quake with a rising wave of lava (kind 2), the two walls that slide in
// from the sides (kind 4) or the zone with the shutters and doors (kind 3). The names of the events are HYPOTHESIS: the
// game has only numbers. (The numbers are those of the event the stage writes to m_eventKind.)
class stNorfair : public stMelee {
    Vec3f m_posAshiba[5];               // 0x1D8 the places of the platforms the fireballs come from (x, the y is set per place)
    grTenganEvent m_eventAttack[5];     // 0x214 one fireball each: the effect (phase 0), the wait (1), the damage (2)
    grTenganEvent m_eventQuake;         // 0x570 the quake that shakes the camera
    grTenganEvent m_eventBg;            // 0x61C the background moves
    grTenganEvent m_eventWave;          // 0x6C8 the wave of lava
    Vec3f m_posLimit[2];                // 0x774 the part of the stage the camera shows
    u8 m_bgState;                       // 0x78C the state of the background (0 = down, 1 = rising, 2 = up, 3 = falling)
    u8 m_eventWait;                     // 0x78D 1 while the stage waits for the next event
    float m_eventTimerTarget;           // 0x790 frames until the next event
    float m_eventTimer;                 // 0x794 frames since the last one
    u8 m_eventKind;                     // 0x798 the event that runs (0 = none)
    u8 m_lastEventKind;                 // 0x799 the last event (its chance is changed for the next one)
    u8 m_eventCount;                    // 0x79A the row of the table of events
    char _79b;
    Vec3f m_posMagma;                   // 0x79C the lava (the y is its height)
    float m_cameraBottom;               // 0x7A8 the lowest the camera goes
    u8 m_cameraInit;                    // 0x7AC the bottom of the camera is not known yet
    char _7ad[3];
    Vec3f m_posZone[5];                 // 0x7B0 the places of the zones the fighters can wait in
    u8 m_zoneIndex;                     // 0x7EC the zone that is in use
    u8 m_zoneState;                     // 0x7ED the state of the zone (8 = nothing)
    u8 m_zoneIn[4];                     // 0x7EE a fighter is in the zone
    char _7f2[2];
    Vec3f m_posAshibaT[4];              // 0x7F4 the places of the platforms of the trainer
    u8 m_bgmStarted;                    // 0x824 the sound of the stage was played
    char _825[3];
    stDataMultiContainer* m_tblMagma;   // 0x828 the table of the lava (from the stage data)
    stDataMultiContainer* m_tblEvent;   // 0x82C the table of the events
    u8 m_eventResult;                   // 0x830 the decision of the last event of the wave
    u8 m_trainerEvent;                  // 0x831 the stage event of the adventure is on
    char _832[2];
    s32 m_dangerZone[5];                // 0x834 the ids of the places the fighters have to avoid (-1 = none)

public:
    stNorfair();
    static stNorfair* create();

    virtual ~stNorfair();
    virtual bool loading();
    virtual void createObj();
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual GXColor getFinalTechniqColor();
    virtual bool isBamperVector() { return true; }
    virtual int getZoneState();
    virtual void getZonePos(Vec3f* pos);
    virtual float getMagmaHeight();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjMagma(int index);
    virtual void createObjDoor(int index);
    virtual void createObjZone(int index);
    virtual void createObjShutter(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjAshibaT(int index);
    virtual void createObjWall(int index);
    virtual void createObjWave(int index);
    virtual void updateLimit();
    virtual void updateEvent(float deltaFrame);
    virtual void initStageDataTbl();

    static stClassInfoImpl<Stages::Norfair, stNorfair> bss_loc_14;
};
static_assert(sizeof(stNorfair) == 0x848, "Class is wrong size!");
