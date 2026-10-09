#pragma once

#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <gr/gr_stadium_vision.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <nw4r/ut/ut_Color.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::DxPStadium, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxPStadium, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxPStadium, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Pokemon Stadium (returning Melee stage): the stage turns into one of four terrains - fire, grass, rock, water - in a shuffled order. A
// scheduler event (m_eventPick) starts the transformation (m_eventTransform), which is a small phase machine that
// shakes the stage, scales the old terrain away and the new one in. The big screen (grStadiumVision, ground 0) shows
// the terrain names and sometimes zooms in on one of the fighters.
class stDxPStadium : public stMelee {
    grTenganEvent m_eventPick;       // 0x1D8: waits, then picks the next terrain
    grTenganEvent m_eventTransform;  // 0x284: the transformation (phases 0-7, updateSpecialStage)
    grTenganEvent m_eventGrow;       // 0x330: the new terrain has grown, wait before it settles
    grTenganEvent m_eventQuake;      // 0x3DC: camera quake while the terrain moves
    grTenganEvent m_eventScreen;     // 0x488: terrain announcement on the big screen
    grTenganEvent m_eventScreenHold; // 0x534: HYPOTHESIS: keeps the transformation waiting for the announcement
    u8 m_terrain;                    // 0x5E0: ground index of the current terrain (1 fire, 2 grass, 4 rock, 7 water; 0 none)
    float m_terrainScale;            // 0x5E4: scale of the terrain model while it grows / shrinks
    float m_unk5E8;                  // 0x5E8: HYPOTHESIS: path position of the water spray nodes
    u32 m_unk5EC;                    // 0x5EC: effect handle, -1 when unused
    u32 m_effectHandles[5];          // 0x5F0: terrain effects (fire, rock, water spray x4)
    u32 m_unk604;                    // 0x604: effect handle, -1 when unused
    u32 m_unk608;                    // 0x608: effect handle, -1 when unused
    s32 m_pickIndex;                 // 0x60C: next entry of m_pickOrder
    u8 m_pickOrder[4];               // 0x610: shuffled terrain order, each cycle shows every terrain once
    u8 m_unk614;                     // 0x614
    bool m_visionActive;             // 0x615: the big screen follows a fighter
    u8 m_visionPlayer;               // 0x616: the fighter shown on the screen
    u8 m_visionNext;                 // 0x617: round-robin index into the fighters in play
    u8 m_visionCount;                // 0x618: screens shown since the last terrain announcement
    grTenganEvent m_eventVision;     // 0x61C: schedules the next picture on the big screen
    float m_visionZoomTarget;        // 0x6C8
    float m_visionZoom;              // 0x6CC
    Vec3f m_visionPosA;              // 0x6D0: smoothed corner of the fighter's camera box
    Vec3f m_visionPosB;              // 0x6DC
    float m_visionLeft;              // 0x6E8: where the fighter is on the screen (0-1)
    float m_visionTop;               // 0x6EC
    float m_visionRight;             // 0x6F0
    float m_visionBottom;            // 0x6F4
    bool m_unk6F8;                   // 0x6F8: HYPOTHESIS: the terrain is settling (water pools)
    stTrigger* m_beltTrigger;        // 0x6FC: the conveyor belt of the water terrain
    grGimmickBeltConveyorData* m_beltData; // 0x700

public:
    stDxPStadium();
    static stDxPStadium* create();

    virtual ~stDxPStadium();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual void setVision(u8 kind);
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor() { return nw4r::ut::Color(0x14000496); }

    void updateSpecialStage(float deltaFrame);
    void updateVisionTerrain(float deltaFrame);
    void startPlayerVision();
    void updateVisionRect();
    void updateVisionScreen();

    static stClassInfoImpl<Stages::DxPStadium, stDxPStadium> bss_loc_14;
};
static_assert(sizeof(stDxPStadium) == 0x704, "Class is wrong size!");
