#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <st/st_class_info.h>
#include <st/st_data_container.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Village, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Village, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Village, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stVillageData {
    float unk00;    // 0x00 chance of the platform that is used in the late scenes
    float unk04;    // 0x04 frames until the UFO first comes
    float unk08;    // 0x08 frames until the UFO may come again
    float unk0C;    // 0x0C chance of the UFO
    float unk10;    // 0x10 how fast the UFO moves
    float unk14;
    float unk18;    // 0x18 frames until the first balloon
    float unk1C;    // 0x1C least frames until the next balloon
    float unk20;    // 0x20 most frames until the next balloon
    float unk24;    // 0x24 chance of a balloon that moves backwards
    float unk28;    // 0x28 how fast a balloon moves
    float unk2C;    // 0x2C frames the balloon stays
    float unk30;    // 0x30 frames until the bird comes first
    float unk34;    // 0x34 frames until the bird may come again
    float unk38;    // 0x38 chance of the bird
    float unk3C;    // 0x3C frames until the taxi comes first
    float unk40;    // 0x40 frames until the taxi may come again
    float unk44;    // 0x44 chance of the taxi
};
static_assert(sizeof(stVillageData) == 0x48, "Class is wrong size!");

// Smashville: a village scene that changes with the time of day. The townsfolk come and go (the guests), a bird, a taxi and a
// UFO pass by, and two floating platforms follow the stage.
class stVillage : public stMelee {
    u8 m_scene;                             // 0x1D8 which time of day (0 - 4)
    u8 m_area;                              // 0x1D9 HYPOTHESIS: where the stage is played (0 - 2, only 0 and 1 have the guests of the bar)
    char _1da[4];
    u8 m_live;                              // 0x1DE
    char _1df;
    Vec3f m_posGuest[11];                   // 0x1E0 where the guests stand (the last two are the seats of the pigeon house)
    u8 m_ashibaScene;                       // 0x264 which of the two platform models is used
    u8 m_perioState;                        // 0x265
    char _266[2];
    float m_perioTimer;                     // 0x268
    u8 m_perioMotion;                       // 0x26C
    u8 m_taxiState;                         // 0x26D
    char _26e[2];
    float m_taxiTimer;                      // 0x270
    u8 m_taxiMotion;                        // 0x274
    u8 m_ufoState;                          // 0x275
    char _276[2];
    float m_ufoTimer;                       // 0x278
    u8 m_motion[12];                        // 0x27C the motion states the guests and the passing things share
    nw4r::g3d::ResFile m_guestRes;          // 0x288 the animations of the guests
    stDataMultiContainer* m_guestList;      // 0x28C which guests come at which time of day
    stDataMultiContainer* m_guestPos;       // 0x290 where the guests stand
    char _294[4];

public:
    stVillage();
    static stVillage* create();

    virtual ~stVillage();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjSky(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjGuest();
    virtual void createObjGuest1(int index, int type);
    virtual void createObjGuestPathMove(int index);
    virtual void createObjLiveDeco(int index);
    virtual void createObjClock(int index);
    virtual void createObjBalloon(int index);
    virtual void updatePerio(float deltaFrame);
    virtual void updateTaxi(float deltaFrame);
    virtual void updateUFO(float deltaFrame);
    virtual void initStageDataTbl();
    virtual void selectScene();
    virtual void setLive(u8 live) { m_live = live; }
    virtual void setScene(u8 scene) { m_scene = scene; }
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor();

    static stClassInfoImpl<Stages::Village, stVillage> bss_loc_14;
};
static_assert(sizeof(stVillage) == 0x298, "Class is wrong size!");
