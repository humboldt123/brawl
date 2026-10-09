#pragma once

#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <nw4r/ut/ut_Color.h>
#include <snd/snd_id.h>
#include <st/se_util.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Orpheon, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Orpheon, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Orpheon, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Frigate Orpheon: the Space Pirate Queen's ship. A scheduler event (m_eventCall) randomly starts one of two big
// events - the power failure (the ship goes dark, the Queen's nose pokes in) or the spin (the whole stage rolls over)
// - while two trap floors and the background Queen move on their own timers.
class stOrpheon : public stMelee {
    grTenganEvent m_eventCall;       // 0x1D8: picks the next big event
    grTenganEvent m_eventPower;      // 0x284: the power failure
    grTenganEvent m_eventSpin;       // 0x330: the stage rolls over
    grTenganEvent m_eventTrap1;      // 0x3DC: "Move1", the first trap floor
    grTenganEvent m_eventTrap2A;     // 0x488: "Move2A"
    grTenganEvent m_eventTrap2B;     // 0x534: "Move2B"
    float m_spinAngle;               // 0x5E0: current roll of the stage (degrees)
    float m_spinTarget;              // 0x5E4: where the roll is heading (0 / 180 / 360-ish)
    float m_queenPathT;              // 0x5E8: path parameter of the Queen ("PQ") ground
    float m_queenRoll;               // 0x5EC
    float m_trap1PathT;              // 0x5F0
    float m_trap2APathT;             // 0x5F4
    float m_trap2BPathT;             // 0x5F8
    float m_trap1Wait;               // 0x5FC: random wait before the next Move1 cycle
    float m_trap2AWait;              // 0x600
    float m_trap2BWait;              // 0x604
    float m_unk608;                  // 0x608
    float m_unk60C;                  // 0x60C
    float m_unk610;                  // 0x610
    StSeUtil::SeSeqInstance<3, 3> m_seQueen; // 0x614
    StSeUtil::SeSeqInstance<1, 1> m_seSpin;  // 0x688
    u8 m_queenMotion;                // 0x6C4: motion of the Queen ground (0 idle, 1 bite)
    bool m_isSpinning : 1;           // 0x6C5, 0x80
    bool m_isSpun : 1;               // 0x6C5, 0x40: the stage is rolled over
    u8 m_unk6C5 : 6;
    u8 m_repeatCount;                // 0x6C6
    u8 m_lastChoice;                 // 0x6C7

public:
    stOrpheon();
    static stOrpheon* create();

    virtual ~stOrpheon();
    virtual void createObj();
    virtual bool loading();
    virtual void update(float deltaFrame);
    virtual GXColor getFinalTechniqColor() { return nw4r::ut::Color(0x1400047D); }

    virtual void setJointCliff(bool enable);
    void setVisibilityTrainer(bool visible);

    void eventSpinStage(float deltaFrame);
    void eventTrapFloor1(float deltaFrame);
    void eventTrapFloor2A(float deltaFrame);
    void eventTrapFloor2B(float deltaFrame);
    void eventPowerFailure(float deltaFrame);
    void eventCall(float deltaFrame);

    static stClassInfoImpl<Stages::Orpheon, stOrpheon> bss_loc_14;
};
static_assert(sizeof(stOrpheon) == 0x6C8, "Class is wrong size!");
