#pragma once

#include <ef/ef_screen_handle.h>
#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <nw4r/ut/ut_Color.h>
#include <snd/snd_id.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::DxZebes, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxZebes, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxZebes, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// One of the 20 cells of the Zebes organism (ground model 7): cells 0 and 6 are the anchors at the two ends of the
// chain, 1-5 are the cells between them and 7-19 the free floating ones. HYPOTHESIS: the meaning of m_state (0 unused,
// 1 floating, 2 popped, 3 about to pop, 4 chain link) follows its use in grZebesCellUpdateOne.
struct stDxZebesCell {
    u8 m_state;          // 0x00
    u8 _1;
    s16 m_timer;         // 0x02
    float m_x;           // 0x04
    float m_y;           // 0x08
    float m_velX;        // 0x0C
    float m_velY;        // 0x10
    float m_size;        // 0x14
    float m_targetSize;  // 0x18
    grMadein* m_ground;  // 0x1C
};
static_assert(sizeof(stDxZebesCell) == 0x20, "Class is wrong size!");

// The state machine that moves one of the two shafts (ground ShaftHitL / ShaftHitR) when they are hit.
struct stDxZebesShaft {
    u8 m_state;          // 0x00
    u8 m_nextState;      // 0x01
    s16 m_timer;         // 0x02
    float m_base;        // 0x04
    float m_offset;      // 0x08
    float m_velocity;    // 0x0C
    float m_strength;    // 0x10
    float _14;
};
static_assert(sizeof(stDxZebesShaft) == 0x18, "Class is wrong size!");

// A line of up to 10 effects that follows the acid: m_source holds two points per segment, m_points the same points
// moved by the stage's offset, and an effect appears wherever the segment crosses the height of the acid.
struct stDxZebesAcidLine {
    Vec3f* m_source;          // 0x00
    Vec3f m_points[20];       // 0x04
    int m_count;              // 0xF4
    u32 m_effects[10];        // 0xF8
};
static_assert(sizeof(stDxZebesAcidLine) == 0x120, "Class is wrong size!");

// Brinstar (the returning Melee stage): the acid at the bottom rises and falls, the organism in the middle grows cells
// that float around, and the two shafts on the sides move when they are hit. HYPOTHESIS: the names of the members follow
// their use in the member functions.
class stDxZebes : public stMelee {
    u8 m_flagStart : 1;              // 0x1D8: 0x80, the stage fades in first
    u8 m_flagFade : 1;               // 0x1D8: 0x40
    u8 _1D8_pad : 6;
    float m_fadeTimer;               // 0x1DC
    stCollisionWork m_collision;     // 0x1E0
    stDxZebesShaft m_shaft[2];       // 0x1F0
    stDxZebesCell m_cells[20];       // 0x220
    s16 m_acidStep;                  // 0x4A0: index into the table of acid levels
    s16 m_acidTimer;                 // 0x4A2
    s32 m_acidState;                 // 0x4A4: 0 wait, 1 about to move, 2 start, 3 accelerate, 4 slow down
    float m_acidRate;                // 0x4A8: fastest speed
    float m_acidSpeed;               // 0x4AC
    float m_acidTarget;              // 0x4B0
    float m_acidHeight;              // 0x4B4
    s16 m_mapState;                  // 0x4B8: 0 start, 1 .., 2 wait, 3 .., 4 ..
    s16 m_mapTimer;                  // 0x4BA
    s32 m_lastShaftState;            // 0x4BC
    s16 m_cellRespawnTimer;          // 0x4C0
    s16 m_cellCounter;               // 0x4C2
    s16 m_cellSpawnTimer;            // 0x4C4
    char _4C6[6];
    s16 m_bgState;                   // 0x4CC
    char _4CE[2];
    stDxZebesAcidLine m_lineA;       // 0x4D0
    stDxZebesAcidLine m_lineB;       // 0x5F0
    Vec3f m_bounds[4];               // 0x710: the box the cells live in, then the anchors (cell 0 / cell 6)
    u16 m_cellDelay;                 // 0x740
    grTenganEvent m_eventPhase;      // 0x744
    grTenganEvent m_eventShaft;      // 0x7F0
    u8 m_motionToggle;               // 0x89C
    Vec3f m_sourceA[20];             // 0x8A0
    Vec3f m_sourceB[8];              // 0x990
    float m_cameraMinY;              // 0x9F0
    float m_cameraFloor;             // 0x9F4
    u8 m_cameraInit;                 // 0x9F8
    efScreenHandle m_screenHandle;   // 0x9F9
    char _9FA[2];

public:
    stDxZebes();
    static stDxZebes* create();

    virtual ~stDxZebes();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor() { return nw4r::ut::Color(0x1400047d); }
    virtual float getAcidHeight();

    void updateBirdman(float deltaFrame);
    void grZebesCellAdd(int index, float x, float y, float size, int state);
    bool grZebesCellUpdateOne(int index);
    bool grZebesCellUpdate();
    void grZebesCellRebirth();
    void grZebesCellInit();
    void grZebesShaftUpdate(int groundA, stDxZebesShaft* shaft, int groundB, int groundC);
    void grZebesWaterInit();
    void grZebesWaterProc();
    void grZebesMapProc();
    void grZebesBGProc();
    int grWEUpdate(float height, stDxZebesAcidLine* line, Vec3f* offset);

    static stClassInfoImpl<Stages::DxZebes, stDxZebes> bss_loc_14;
};
static_assert(sizeof(stDxZebes) == 0x9FC, "Class is wrong size!");
