#pragma once

#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <gr/gr_stadium_vision.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Stadium, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Stadium, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Stadium, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Pokemon Stadium (1): the stage turns into an electric, ice, ground or flying field in a shuffled order. It shares the
// transformation skeleton and the big-screen code with Pokemon Stadium 2 (st_dxpstadium); here each terrain also brings
// a few Pokemon (Jibacoil, Elekible, Yukikaburi, ...) that grow in one after the other and play their motions.
class stStadium : public stMelee {
    u8 m_unk1D8;                     // 0x1D8
    grTenganEvent m_eventPick;       // 0x1DC: waits, then picks the next terrain
    grTenganEvent m_eventTransform;  // 0x288: the transformation (phases 0-7, updateSpecialStage)
    grTenganEvent m_eventGrow;       // 0x334: the terrain has grown, wait before it settles
    grTenganEvent m_eventQuake;      // 0x3E0: camera quake while the terrain moves
    grTenganEvent m_eventScreen;     // 0x48C: terrain symbol on the big screen
    grTenganEvent m_eventScreenHold; // 0x538: HYPOTHESIS: holds the transformation until the symbol was shown
    grTenganEvent m_eventHazard;     // 0x5E4: HYPOTHESIS: timer of the terrain's hazards
    grGimmickBeltConveyorData* m_beltDataA; // 0x690: conveyor belts of the electric terrain
    grGimmickBeltConveyorData* m_beltDataB; // 0x694
    stTrigger* m_beltTriggerA;       // 0x698
    stTrigger* m_beltTriggerB;       // 0x69C
    u8 m_unk6A0;                     // 0x6A0
    float m_unk6A4;                  // 0x6A4
    u8 m_unk6A8;                     // 0x6A8
    u8 m_terrain;                    // 0x6A9: ground index of the current terrain (0xA electric, 0xC ice, 0xD ground, 0xE flying)
    s32 m_visionActive;              // 0x6AC: the big screen follows a fighter
    float m_terrainScale;            // 0x6B0
    s32 m_seQuake;                   // 0x6B4: sound handle of the quake
    u8 m_normalSymbol;               // 0x6B8: the symbol shown is the "normal" one (between two terrains)
    s32 m_seHandleA;                 // 0x6BC
    s32 m_seHandleB;                 // 0x6C0
    s32 m_pickIndex;                 // 0x6C4
    u8 m_pickOrder[4];               // 0x6C8: shuffled terrain order
    float m_flyZ[3];                 // 0x6CC: random depth of the three flying Pokemon
    float m_grow[3];                 // 0x6D8: grow factor of the Pokemon groups (they appear one after another)
    u8 m_visionPlayer;               // 0x6E4
    u8 m_visionNext;                 // 0x6E5
    u8 m_visionCount;                // 0x6E6
    u8 m_lastChoice;                 // 0x6E7
    grTenganEvent m_eventVision;     // 0x6E8: schedules the next picture on the big screen
    float m_visionZoomTarget;        // 0x794
    float m_visionZoom;              // 0x798
    Vec3f m_visionPosA;              // 0x79C
    Vec3f m_visionPosB;              // 0x7A8
    Vec2f m_visionMin;               // 0x7B4: where the fighter is on the screen (0-1)
    Vec2f m_visionMax;               // 0x7BC

public:
    stStadium();
    static stStadium* create();

    virtual ~stStadium();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual void setVision(u8 kind);
    virtual bool isBamperVector() { return true; }
    virtual void notifyEventInfoReady();
    virtual void notifyEventInfoGo();

    // HYPOTHESIS: new virtual of this stage (last vtable slot)
    virtual void createObjDetails();

    void updateSpecialStage(float deltaFrame);
    void updateSymbol(float deltaFrame);
    void enableVisionScreen();
    void updateVisionScreenPos();
    void updateVisionScreen();

    static stClassInfoImpl<Stages::Stadium, stStadium> bss_loc_14;
};
static_assert(sizeof(stStadium) == 0x7C4, "Class is wrong size!");
