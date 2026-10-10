#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Palutena, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Palutena, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Palutena, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stPalutenaData {
    float m_hp;     // 0x00 the life of every platform
    float unk04;
    float unk08;
    float unk0C;
    float unk10;    // 0x10 fraction of the life under which a platform is damaged (1st stage of breaking)
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;    // 0x24 frames the platforms need to be built again (from the platform's side)
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
    float unk70;    // 0x70 speed of the clouds
    char _74[0x10];
};
static_assert(sizeof(stPalutenaData) == 0x84, "Class is wrong size!");

// How the moving platform (grGimmickMovement) moves: the points it moves between and how (HYPOTHESIS: layout).
struct grGimmickMovementData {
    Vec3f m_start;  // 0x00
    Vec3f m_goal;   // 0x0C
    float m_speed;  // 0x18 the speed the movement starts with
    float m_accel;  // 0x1C
    u8 m_kind;      // 0x20
    char _21[3];
    float m_time;   // 0x24 the frames the movement takes (0 = speed based)
    u8 m_ease;      // 0x28 0 / 1 = sine in, 2 = cosine
    char _29[3];
};
static_assert(sizeof(grGimmickMovementData) == 0x2C, "Class is wrong size!");

// Palutena's Temple: the platforms fall apart when they are hit (their life is kept by the stage), clouds move around and a
// chain hangs between two of them.
class stPalutena : public stMelee {
    Vec3f m_posGimmick[2];                 // 0x1D8 positions the grounds publish (the clouds' ends)
    float m_hp[10];                        // 0x1F0 the life of every platform
    grGimmickMovementData m_movement;      // 0x218 the movement data of the moving platform
    void* m_resData;                       // 0x244 the file with the animations of the platforms
    u8 m_event;                            // 0x248 1 = the stage is run by the event mode
    char _249[3];

public:
    stPalutena();
    static stPalutena* create();

    virtual ~stPalutena();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjAshibaE(int index);
    virtual void createObjAshibaBreak(int index);
    virtual void createObjKumo(int index);
    virtual void createObjChain(int index);
    virtual void updatePos();
    virtual void updateBreak(float deltaFrame);
    virtual void initMovementData();
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual bool isBamperVector() { return true; }

    static stClassInfoImpl<Stages::Palutena, stPalutena> bss_loc_14;
};
static_assert(sizeof(stPalutena) == 0x24C, "Class is wrong size!");
