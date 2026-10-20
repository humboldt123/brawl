#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gm/gm_lib.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st_pirates/gr_pirates_work.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Pirates, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Pirates, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Pirates, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

class stTrigger;
class grGimmickWaterData;

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stPiratesData {
    float waterVelocity;    // 0x00 the strength of the water
    float unk04;
    float unk08;
    float unk0C;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;    // 0x24 frames the background waits before its first change
    float unk28;
    float unk2C;
    float unk30;    // 0x30
    float unk34;
    float unk38;    // 0x38 frames to the first event (after the first event the next one)
    float unk3C;
    float unk40;    // 0x40 frames between two events
    float unk44;
    float unk48;    // 0x48 the chance of event 1 (fine weather)
    float unk4C;    // 0x4C the chance of event 6 (clouds)
    float unk50;    // 0x50 the chance of event 7 (rain)
    float unk54;    // 0x54 the chance of event 9 (tornado)
    float unk58;    // 0x58 how much the chance of the event that has just been is multiplied
    float unk5C;    // 0x5C the frames of the match below which the weather event is chosen
    float unk60;    // 0x60 the chance of rain when the weather is chosen
    float unk64;    // 0x64 the chance of clouds when the weather is chosen
    float unk68;
    float unk6C;
    float unk70;
    float unk74;
    float unk78;
    float unk7C;
    float unk80;
    float unk84;
    float unk88;
    float unk8C;
    float unk90;
    float unk94;
};
static_assert(sizeof(stPiratesData) == 0x98, "Class is wrong size!");

// The Pirate Ship: the ship sails through weather events (fine, clouds, rain), a tornado, a rain of bombs from the cannons,
// the ship runs aground and barrels drop onto the deck. The stage runs the events as a state machine and hands the grounds the
// work they read (the matrices of the scene, the event, the states).
class stPirates : public stMelee {
    u8 unk1D8;                      // 0x1D8
    char _1D9[3];
    float m_sceneTimer;             // 0x1DC frames left of the scene loop (480 frames)
    u8 unk1E0;                      // 0x1E0 1 = the loop has started again this frame
    u8 unk1E1;                      // 0x1E1 the loops left of the event
    char _1E2[2];
    stPiratesMtx m_mtx[18];         // 0x1E4 the matrices of the scene (the grounds read them)
    Vec3f m_posLimit[2];            // 0x544 the left/top and right/bottom limit of the camera
    u8 unk55C;                      // 0x55C the scene animation that plays
    u8 unk55D;                      // 0x55D the step of the event state machine
    char _55E[2];
    float unk560;                   // 0x560 frames until the step goes on
    float unk564;                   // 0x564 frames of the event
    u8 unk568;                      // 0x568 the event (weather) that is on
    u8 unk569;                      // 0x569 the event that was on before
    u8 unk56A;                      // 0x56A 1 = no event has been yet
    u8 unk56B;                      // 0x56B the step of the running aground
    u8 unk56C;                      // 0x56C how often the ship runs aground
    u8 unk56D;                      // 0x56D 1 = the lion king is shown
    u8 unk56E;                      // 0x56E the step of the lion king
    u8 unk56F;                      // 0x56F the step of the bombs
    u8 unk570;                      // 0x570 the cannon that fires
    char _571[3];
    u8 unk574[0x60];                // 0x574
    Vec3f unk5D4;                   // 0x5D4 where the bomb is shot at
    float unk5E0;                   // 0x5E0 frames of the bomb event
    u8 unk5E4;                      // 0x5E4 the step of the drops
    char _5E5[3];
    float unk5E8;                   // 0x5E8 frames until the next drop
    u8 unk5EC;                      // 0x5EC the drop that falls
    u8 unk5ED[4];                   // 0x5ED the steps of the four drops
    u8 unk5F1;                      // 0x5F1 the step of the warning
    char _5F2[2];
    snd3DGenerator m_sndGenerator;  // 0x5F4
    stTrigger* m_waterTrigger;      // 0x5FC
    grGimmickWaterData* m_waterData; // 0x600
    float unk604;                   // 0x604 the strength of the water (1 = full)
    u8 unk608;                      // 0x608 1 = the stage is run by the event mode
    u8 unk609;                      // 0x609 the steps of the attacks
    u8 unk60A;
    u8 unk60B;

public:
    Matrix* mtx(int index) { return reinterpret_cast<Matrix*>(&m_mtx[index]); }

    stPirates();
    static stPirates* create();

    virtual ~stPirates();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjEnkei(int index);
    virtual void createObjLionKing(int index);
    virtual void createObjSling(int index);
    virtual void createObjShip(int index);
    virtual void createObjShipRipple(int index);
    virtual void createObjTornado(int index);
    virtual void createObjAground(int index);
    virtual void createObjSeaSky(int index);
    virtual void createObjTower(int index);
    virtual void createObjCannon(int index);
    virtual void createObjBomb(int index);
    virtual void createObjDrop(int index);
    virtual void createObjWarning(int index);
    virtual void createObjRain(int index);
    virtual void createObjAttack(int index);
    virtual void createObjSwimArea();
    virtual void updateLimit(float deltaFrame);
    virtual void updateEvent(float deltaFrame);
    virtual void updateEventSelect(float deltaFrame);
    virtual void updateEventWeatherSelect(float deltaFrame);
    virtual void updateEventWeather(float deltaFrame);
    virtual void updateEventBomb(float deltaFrame);
    virtual void updateScene(float deltaFrame);
    virtual void updateSea(float deltaFrame);
    virtual void updateDrop(float deltaFrame);
    virtual void requestEventWeatherFine();
    virtual void requestEventWeatherCloud();
    virtual void requestEventWeatherRain();
    virtual void requestEventTornado();
    virtual void requestEventBomb();
    virtual void requestEventAground();
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);

    static stClassInfoImpl<Stages::Pirates, stPirates> bss_loc_14;
};
static_assert(sizeof(stPirates) == 0x60C, "Class is wrong size!");
