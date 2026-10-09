#pragma once

#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::DxCruise, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxCruise, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxCruise, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Rainbow Cruise: the steady auto-scrolling course. 17 "chikuwa" blocks wobble, drop out of sight and come back when
// a fighter stands on them, three carpets fly off when landed on, and the seesaw tilts with the fighters' weight.
// updateAI reports the moving hazards to the AI as danger zones.
class stDxCruise : public stMelee {
    grTenganEvent m_blockEvent[17];  // 0x1D8: one phase machine per falling block
    float m_blockHeight[17];         // 0xD44: height offset of each block
    float m_blockSpeed[17];          // 0xD88: fall speed of each block
    grTenganEvent m_carpetEvent[3];  // 0xDCC: one phase machine per carpet
    u8 m_carpetFlag[3];              // 0xFD0: HYPOTHESIS: the carpet was landed on
    char _fd3[1];                    // 0xFD3
    float m_unkFD4;                  // 0xFD4
    float m_seesawAngle;             // 0xFD8: tilt of the seesaw (degrees)
    float m_seesawDir;               // 0xFDC: tilt direction while the seesaw settles
    float m_seesawSpeed;             // 0xFE0
    float m_weightLeft;              // 0xFE4: summed distance of the fighters on the left side
    float m_weightRight;             // 0xFE8: ... and on the right side
    s32 m_seesawState;               // 0xFEC: 0 balancing, 1 tilting
    s32 m_prevCount;                 // 0xFF0: fighters on the seesaw last frame
    s32 m_count;                     // 0xFF4: fighters on the seesaw this frame
    s32 m_settleFrames;              // 0xFF8
    int m_dangerZone[3];             // 0xFFC: AI danger zone handles, -1 when unused
    u8 m_unk1008;                    // 0x1008: HYPOTHESIS: an event match that scales the stage speed
    u8 m_speedInit;                  // 0x1009
    float m_speedScale;              // 0x100C
    float m_lastMotionFrame;         // 0x1010

public:
    stDxCruise();
    static stDxCruise* create();

    virtual ~stDxCruise();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual bool isEventEnd(int unk1, int* unk2, int* unk3);

    // HYPOTHESIS: new virtuals of this stage (vtable slots 0x21C / 0x220)
    virtual void updateAI(float deltaFrame);
    virtual void initStageData();

    void Seasaw();

    static stClassInfoImpl<Stages::DxCruise, stDxCruise> bss_loc_14;
};
static_assert(sizeof(stDxCruise) == 0x1014, "Class is wrong size!");
