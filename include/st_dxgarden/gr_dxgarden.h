#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <st/se_util.h>
#include <types.h>

// Jungle Japes (DX "Garden") grounds. They are Yakumono grounds that carry a state byte and a timer; the stage hands them
// the data they need through the stage data pointer and the virtual set...Work functions.
class grDxGarden : public grYakumono {
protected:
    u8 m_state;    // 0x150
    float m_timer; // 0x154

public:
    grDxGarden(const char* taskName);
    virtual ~grDxGarden();
    static grDxGarden* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGarden) == 0x158, "Class is wrong size!");

// The backdrop of the stage.
class grDxGardenBg : public grDxGarden {
public:
    grDxGardenBg(const char* taskName) : grDxGarden(taskName) { }
    virtual ~grDxGardenBg();
    static grDxGardenBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGardenBg) == 0x158, "Class is wrong size!");

// Cranky Kong, who swings between animations. The stage data holds the number of times (bytes at +8 and +9) two of the
// animations repeat.
class grDxGardenCranky : public grDxGarden {
    u8 m_animId;    // 0x158: animation that plays (4 = none yet)
    float m_frames; // 0x15C: frames left of the animation

public:
    grDxGardenCranky(const char* taskName);
    virtual ~grDxGardenCranky();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    static grDxGardenCranky* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGardenCranky) == 0x160, "Class is wrong size!");

// What the crocodile's hit object keeps (HYPOTHESIS: the two words are never written after they are cleared).
struct grDxGardenKrapWork {
    u32 unk0;
    u32 unk4;
    grDxGardenKrapWork() {
        unk0 = 0;
        unk4 = 0;
    }
};

// The crocodile ("Krap") that snaps out of the river. It appears at a random place between two positions of the stage data,
// bites with an attack box for part of its animation and plays its sounds with a sound sequence.
class grDxGardenKrap : public grDxGarden {
    u8 m_animId;                      // 0x158: animation that plays (1 = none)
    float m_frames;                   // 0x15C: frames left of the animation
    float m_biteFrame;                // 0x160: frames since the crocodile came up
    Vec3f m_pos;                      // 0x164: where it appears
    int m_effectHandle;               // 0x170: effect of the water spray
    u8 m_effectOn;                    // 0x174
    u8 m_hasYakumono;                 // 0x175: the hit object was created
    u8 m_attackSet;                   // 0x176: the attack is on
    grDxGardenKrapWork* m_work;       // 0x178
    StSeUtil::SeSeqInstance<1, 2> m_sePlayer; // 0x17C
    SndID m_seIds[2];                 // 0x1C8
    StSeUtil::UnkStruct m_seData[4];  // 0x1D0
    snd3DGenerator m_snd;             // 0x210

public:
    grDxGardenKrap(const char* taskName);
    virtual ~grDxGardenKrap();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateEffect(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    static grDxGardenKrap* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGardenKrap) == 0x218, "Class is wrong size!");

// The candles of the stage (two of them, type 0 and 1) burn with an effect on their model.
class grDxGardenLamp : public grDxGarden {
    u8 m_type; // 0x158

public:
    grDxGardenLamp(const char* taskName) : grDxGarden(taskName) { }
    virtual ~grDxGardenLamp();
    virtual void update(float deltaFrame);
    virtual void setType(int type) { m_type = type; }
    static grDxGardenLamp* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGardenLamp) == 0x15C, "Class is wrong size!");

// The surface of the river: the splash effects and the river sound, plus a danger zone for the AI over the water.
class grDxGardenSuimen : public grDxGarden {
    float* m_posLimitWork;   // 0x158: the limits of the stage, as the stage publishes them
    snd3DGenerator m_snd;    // 0x15C
    int m_dangerZone;        // 0x164

public:
    grDxGardenSuimen(const char* taskName);
    virtual ~grDxGardenSuimen();
    virtual void update(float deltaFrame);
    virtual void setPosLimitWork(float* work) { m_posLimitWork = work; }
    static grDxGardenSuimen* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGardenSuimen) == 0x168, "Class is wrong size!");
