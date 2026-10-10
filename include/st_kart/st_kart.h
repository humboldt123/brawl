#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gr/gr_path.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Kart, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Kart, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Kart, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The parameters of the stage (the stage data file). Nothing is named by the original, so the fields are the offsets.
struct stKartData {
    float unk00; // speed factor of the karts
    float unk04; // how much speed a kart loses when it is hit
    float unk08; // how fast a kart that was hit flies away to the side
    float unk0C;
    float unk10;
    float unk14; // speed factor of a kart far behind the others
    float unk18;
    float unk1C; // life of a kart
    float unk20; // frames a kart that is gone waits before it comes back
    float unk24;
    float unk28; // frames to wait when no kart can be followed
    float unk2C; // frames until the AI of a kart looks for another way of driving
    u8 m_kartNum; // 0x30 how many karts there are (8 at most)
    char _31[3];
    float unk34;
    float unk38;
    float unk3C;
    float unk40;
    float unk44;
    float unk48;
};
static_assert(sizeof(stKartData) == 0x4C, "Class is wrong size!");

// The state of one kart. It is shared by the ground of the kart, its attack and its icon.
struct stKartState {
    u32 m_point;    // 0x00 the point of the path the kart is driving to
    float m_rate;   // 0x04 how far the kart is on the way to the next point (0 - 1)
    float m_side;   // 0x08 where the kart is between the left (0) and the right (1) edge of the road
    Vec3f m_pos;    // 0x0C
    float m_rotX;   // 0x18
    float m_rotY;   // 0x1C
    float m_rotZ;   // 0x20
    int m_lap;      // 0x24 how many rounds the kart did
    float m_life;   // 0x28 the hits the kart can take
    u8 m_rank;      // 0x2C
    u8 m_state;     // 0x2D 0 = not started, 1 = on the flat road, 2 = goes up, 3 = goes down, 4 = hit, 5 / 6 = flies away left / right, 7 = gone
    u8 m_pathKind;  // 0x2E the path the kart takes to the side (8 = none)
    char _2f;
    int m_seId;     // 0x30 the sound of the kart that flew away (-1 = none)
};
static_assert(sizeof(stKartState) == 0x34, "Class is wrong size!");

// Mario Kart (Rainbow Road): the karts drive along paths of the stage. The stage makes the grounds (the road, the map, the
// warning, a kart with its attack and icon for each player) and shares the state of the karts with them.
class stKart : public stMelee {
    grFixedPathCollection* m_path; // 0x1D8 the paths the karts drive on
    Vec3f m_limit[2];              // 0x1DC the area of the camera (lower left corner, upper right corner)
    u8 m_unk1F4;                   // 0x1F4 what the warning shows (10 = nothing)
    u8 m_unk1F5;                   // 0x1F5 what the road does (8 / 9 = opens a side, 10 = nothing)
    char _1f6[2];
    stKartState m_kart[8];         // 0x1F8
    u8 m_zoom;                     // 0x398 1 = the camera is zoomed out
    char _399[3];

public:
    stKart();
    static stKart* create();

    virtual ~stKart();
    virtual bool loading();
    virtual void createObj();
    virtual void renderDebug();
    virtual void update(float deltaFrame);
    virtual void createObjPath();
    virtual void createObjBg(int index);
    virtual void createObjMap(int index);
    virtual void createObjWarning(int index);
    virtual void createObjKart();
    virtual void createObjKart1(u8 team, int mdlIndex);
    virtual void createObjAttack(int team, int mdlIndex);
    virtual void createObjIcon(int team, int mdlIndex);
    virtual void updateLimit(float deltaFrame);
    virtual void updateKart(float deltaFrame);
    // HYPOTHESIS: the original overrides the Stage function of this slot (the zone of the light set at a position); the
    // declaration of Stage has no parameter, so this is a new slot here.
    virtual int getZoneLightSetIndex(Vec3f* position);
    virtual bool isBamperVector() { return true; }

    static stClassInfoImpl<Stages::Kart, stKart> bss_loc_14;
};
static_assert(sizeof(stKart) == 0x39C, "Class is wrong size!");
