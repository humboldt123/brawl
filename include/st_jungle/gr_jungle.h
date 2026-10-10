#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <gf/gf_archive.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_ladder.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <nw4r/math/math_arithmetic.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_3d_generator.h>
#include <so/area/so_area_module_impl.h>
#include <so/collision/so_collision_hit_part.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <st/se_util.h>
#include <string.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_jungle/gr_jungle_hit.h>
#include <st_jungle/st_jungle.h>

// sora_melee / main functions that have no names yet.
// gfCamera::getScreenPosition2WorldVector (main, unnamed): the vector from the camera through a place of the screen
extern "C" void fn_80019E88(void* camera, Vec3f* out, float x, float y);
// Vec3f::normalize1 (main, unnamed): the unit vector (and its length) of a vector
extern "C" void fn_8003DEE0(Vec3f* out, Vec3f* in);
// clLine3D::checkIntersectXYPlane (main, unnamed): where the line (from the origin and the direction) meets the plane z = 0
extern "C" int fn_80043898(Vec3f* lineAndDir, Vec3f* hit);

// The value is brought into the range (the original has it as fsel).
static inline float grJungleClamp(float value, float low, float high) {
    float result = nw4r::math::FSelect(value - low, value, low);
    return nw4r::math::FSelect(result - high, high, result);
}

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// The base ground of the stage. The grounds that move have the state (the step of their sequence) and a timer.
class grJungle : public grYakumono {
protected:
    u8 m_state;                 // 0x150
    char _151[3];
    float m_timer;              // 0x154

public:
    grJungle(const char* taskName);
    virtual ~grJungle();
    virtual void update(float deltaFrame);
};
static_assert(sizeof(grJungle) == 0x158, "Class is wrong size!");

// The sign that warns of the faster scroll (a sound sequence plays while it is on; it is placed in front of the camera).
class grJungleWarning : public grJungle {
protected:
    u8* m_stateWork;            // 0x158 the state of the stage that says the sign is on (1 = the warning)
    u8 m_motion;                // 0x15C
    char _15d[3];
    float m_frameLast;          // 0x160 the frame of the animation of the last update
    StSeUtil::SeSeqInstance<1, 1> m_seSeq; // 0x164
    SndID m_seSeqId;            // 0x1A0
    StSeUtil::UnkStruct m_seSeqData; // 0x1A4

public:
    grJungleWarning(const char* taskName);
    virtual ~grJungleWarning();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setStateWork(u8* stateWork);

    static grJungleWarning* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleWarning) == 0x1B4, "Class is wrong size!");

// The backdrop: it publishes the places of 12 nodes of the model (and its water) to the stage, and plays the sounds of the
// falls that follow the camera.
class grJungleBg : public grJungle {
protected:
    Vec3f* m_posGimmickWork;    // 0x158 the places of the nodes (to the stage)
    Vec3f* m_posLimitWork;      // 0x15C the camera limits (from the stage)
    float* m_frameLocWork;      // 0x160 the frame of the animation (to the stage)
    u32 m_node[13];             // 0x164 the nodes (LV01N - LV13N; the fourth is not used)
    snd3DGenerator m_snd;       // 0x198
    s32 m_seHandle[4];          // 0x1A0

public:
    grJungleBg(const char* taskName);
    virtual ~grJungleBg();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateSE();
    virtual void setPosGimmickWork(Vec3f* posGimmickWork);
    virtual void setPosLimitWork(Vec3f* posLimitWork);
    virtual void setFrameLocWork(float* frameLocWork);

    static grJungleBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleBg) == 0x1B0, "Class is wrong size!");

// The platforms ("Ashiba") of the stage. Every one is a part of the model that has a collision; the stage gives them the
// places (the "gimmick" places published by the backdrop) and the flags of what is on the screen.
class grJungleAshiba : public grJungle {
protected:
    Vec3f* m_posWork;           // 0x158 the place of the platform
    Vec3f* m_posGimmickWork;    // 0x15C the places of the nodes
    Vec3f* m_posHashigoWork;    // 0x160 the places of the ladder
    Vec3f* m_posLimitWork;      // 0x164 the limits of the camera
    Matrix* m_mtxGimmickWork;   // 0x168
    u8* m_enableWork;           // 0x16C the platform is on screen
    Vec3f m_posPrev;            // 0x170 the place of the last frame
    u8 m_yakumonoMade;          // 0x17C the hit object was made
    char _17d[3];
    soCollisionHitData* m_hitData;           // 0x180 the hit spheres
    soCollisionHitData::Simple* m_hitSimple; // 0x184
    soSet<soCollisionHitData>* m_hitSet;     // 0x188
    ykDataGroup* m_dataGroup;   // 0x18C
    ykData* m_data;             // 0x190

public:
    grJungleAshiba(const char* taskName);
    virtual ~grJungleAshiba();
    virtual void update(float deltaFrame);
    virtual void updateBaseActive();
    virtual void updateCallBack(float deltaFrame);
    virtual void resetYakumono();
    virtual int getCallBackNodeCount();
    virtual void setPosWork(Vec3f* posWork);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork);
    virtual void setPosHashigoWork(Vec3f* posHashigoWork);
    virtual void setPosLimitWork(Vec3f* posLimitWork);
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork);
    virtual void setFrameWork();
    virtual void setStateWork(u8* stateWork);
    virtual void setEnableWork(u8* enableWork);
    virtual void setEnableYakumono();
    virtual void setDisableYakumono();

    static grJungleAshiba* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba) == 0x194, "Class is wrong size!");

// The platform with the spikes.
class grJungleAshiba01 : public grJungleAshiba {
protected:
    u8 m_attackOn;              // 0x194
    char _195[3];
    u32 m_node;                 // 0x198 the node of the spikes

public:
    grJungleAshiba01(const char* taskName);
    virtual ~grJungleAshiba01();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void setEnableYakumono();
    virtual void setDisableYakumono();
    virtual void updateYakumono(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();

    static grJungleAshiba01* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba01) == 0x19C, "Class is wrong size!");

// The platform that is a trap (a button turns it off; the platform shows the trap while it is on).
class grJungleAshiba02 : public grJungleAshiba {
protected:
    u8* m_stateWork;            // 0x194 the state of the button (from the stage)
    Vec3f m_scale;              // 0x198 the scale of the node of the trap
    float m_scaleRate;          // 0x1A4 how much of the trap is shown
    u32 m_nodeButton;           // 0x1A8
    u32 m_nodeTrap;             // 0x1AC
    snd3DGenerator m_snd;       // 0x1B0

public:
    grJungleAshiba02(const char* taskName);
    virtual ~grJungleAshiba02();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual bool setNode();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setEnableYakumono();
    virtual void setDisableYakumono();
    virtual void setStateWork(u8* stateWork);
    virtual int getCallBackNodeCount();
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setHit();

    static grJungleAshiba02* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba02) == 0x1B8, "Class is wrong size!");

// The button of the trap platform (it is pushed down when the platform is hit).
class grJungleAshiba02A : public grJungleAshiba {
protected:
    u8 m_motion;                // 0x194
    char _195[3];
    float m_frameCount;         // 0x198 the length of the animation that is bound
    u8* m_stateWork;            // 0x19C

public:
    grJungleAshiba02A(const char* taskName);
    virtual ~grJungleAshiba02A();
    virtual void update(float deltaFrame);
    virtual void setStateWork(u8* stateWork);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);

    static grJungleAshiba02A* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba02A) == 0x1A0, "Class is wrong size!");

// The platform with two spiked nodes and a stone (like the trap platform, with two hit spheres).
class grJungleAshiba03 : public grJungleAshiba {
protected:
    u8* m_stateWork;            // 0x194
    Vec3f m_scale;              // 0x198
    float m_scaleRate;          // 0x1A4
    u32 m_node[4];              // 0x1A8 the button, the spikes, the moving part and the stone
    snd3DGenerator m_snd;       // 0x1B8

public:
    grJungleAshiba03(const char* taskName);
    virtual ~grJungleAshiba03();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual bool setNode();
    virtual void resetYakumono();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setEnableYakumono();
    virtual void setDisableYakumono();
    virtual void setStateWork(u8* stateWork);
    virtual int getCallBackNodeCount();
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setHit();

    static grJungleAshiba03* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba03) == 0x1C0, "Class is wrong size!");

// The button of the platform 03.
class grJungleAshiba03A : public grJungleAshiba {
protected:
    u8 m_motion;                // 0x194
    char _195[3];
    float m_frameCount;         // 0x198
    u8* m_stateWork;            // 0x19C
    u8 m_pressed;               // 0x1A0
    char _1a1[3];

public:
    grJungleAshiba03A(const char* taskName);
    virtual ~grJungleAshiba03A();
    virtual void update(float deltaFrame);
    virtual void setStateWork(u8* stateWork);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void requestActive();

    static grJungleAshiba03A* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba03A) == 0x1A4, "Class is wrong size!");

// The platform that moves when the ladders are used (it follows the places of the nodes the backdrop publishes).
class grJungleAshiba05 : public grJungleAshiba {
public:
    grJungleAshiba05(const char* taskName);
    virtual ~grJungleAshiba05();
    virtual void update(float deltaFrame);

    static grJungleAshiba05* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba05) == 0x194, "Class is wrong size!");

// The lift that goes up when its button is pushed (it moves along the places the stage gives).
class grJungleAshiba05A : public grJungleAshiba {
protected:
    u8 m_stateButton;           // 0x194
    char _195[3];
    float m_timerButton;        // 0x198
    Vec3f m_move;               // 0x19C how far it was moved
    Vec3f m_moveStart;          // 0x1A8
    u8 m_pushes;                // 0x1B4
    char _1b5[3];
    Vec3f m_scale;              // 0x1B8
    float m_scaleRate;          // 0x1C4
    u32 m_node;                 // 0x1C8
    snd3DGenerator m_snd;       // 0x1CC

public:
    grJungleAshiba05A(const char* taskName);
    virtual ~grJungleAshiba05A();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual bool setNode();
    virtual void resetYakumono();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setEnableYakumono();
    virtual void setDisableYakumono();
    virtual int getCallBackNodeCount();
    virtual void updateMove(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateButton(float deltaFrame);
    virtual void setHit();
    virtual void requestActive();

    static grJungleAshiba05A* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba05A) == 0x1D4, "Class is wrong size!");

class grJungleAshiba07 : public grJungleAshiba {
public:
    grJungleAshiba07(const char* taskName);
    virtual ~grJungleAshiba07();
    virtual void update(float deltaFrame);

    static grJungleAshiba07* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba07) == 0x194, "Class is wrong size!");

// The platform that is there for 95 of every 180 frames.
class grJungleAshiba07A : public grJungleAshiba {
public:
    grJungleAshiba07A(const char* taskName);
    virtual ~grJungleAshiba07A();
    virtual void update(float deltaFrame);

    static grJungleAshiba07A* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba07A) == 0x194, "Class is wrong size!");

// The platform that is there from frame 5 to frame 90 of every 180.
class grJungleAshiba07B : public grJungleAshiba {
public:
    grJungleAshiba07B(const char* taskName);
    virtual ~grJungleAshiba07B();
    virtual void update(float deltaFrame);

    static grJungleAshiba07B* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba07B) == 0x194, "Class is wrong size!");

// Like the platform 03, with one hit sphere on the trap.
class grJungleAshiba08 : public grJungleAshiba {
protected:
    u8* m_stateWork;            // 0x194
    Vec3f m_scale;              // 0x198
    float m_scaleRate;          // 0x1A4
    u32 m_node[7];              // 0x1A8 (the button, the spikes and the trap are used)
    snd3DGenerator m_snd;       // 0x1C4

public:
    grJungleAshiba08(const char* taskName);
    virtual ~grJungleAshiba08();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual bool setNode();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setEnableYakumono();
    virtual void setDisableYakumono();
    virtual void setStateWork(u8* stateWork);
    virtual int getCallBackNodeCount();
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setHit();

    static grJungleAshiba08* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba08) == 0x1CC, "Class is wrong size!");

class grJungleAshiba08A : public grJungleAshiba {
protected:
    u8 m_motion;                // 0x194
    char _195[3];
    float m_frameCount;         // 0x198
    u8* m_stateWork;            // 0x19C

public:
    grJungleAshiba08A(const char* taskName);
    virtual ~grJungleAshiba08A();
    virtual void update(float deltaFrame);
    virtual void setStateWork(u8* stateWork);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void requestActive();

    static grJungleAshiba08A* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba08A) == 0x1A0, "Class is wrong size!");

// The wheels (four small ones and a big one) that turn; the sound of them follows the camera.
class grJungleAshiba09 : public grJungleAshiba {
protected:
    u32 m_node[5];              // 0x194 the four wheels and the big wheel
    snd3DGenerator m_snd;       // 0x1A8
    s32 m_seHandle;             // 0x1B0

public:
    grJungleAshiba09(const char* taskName);
    virtual ~grJungleAshiba09();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateSE();

    static grJungleAshiba09* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba09) == 0x1B4, "Class is wrong size!");

// The airplane that carries the two loads; it is there once the stage has scrolled far enough.
class grJungleAshiba10 : public grJungleAshiba {
protected:
    u8* m_stateWork;            // 0x194 the states of the two loads (from the stage)
    u32 m_nodeLeft;             // 0x198
    u32 m_nodeRight;            // 0x19C
    snd3DGenerator m_snd;       // 0x1A0
    s32 m_seHandle;             // 0x1A8
    u8 m_unused;                // 0x1AC
    char _1ad[3];

public:
    grJungleAshiba10(const char* taskName);
    virtual ~grJungleAshiba10();
    virtual void update(float deltaFrame);
    virtual void updateBaseActive();
    virtual bool setNode();
    virtual void resetYakumono();
    virtual void setStateWork(u8* stateWork);
    virtual void updateSE();

    static grJungleAshiba10* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba10) == 0x1B0, "Class is wrong size!");

class grJungleAshiba12 : public grJungleAshiba {
public:
    grJungleAshiba12(const char* taskName);
    virtual ~grJungleAshiba12();
    virtual void update(float deltaFrame);

    static grJungleAshiba12* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba12) == 0x194, "Class is wrong size!");

// The platform that rises on the side of the stage (it moves when the stage says so, with an effect).
class grJungleAshiba12A : public grJungleAshiba {
protected:
    Vec3f m_move;               // 0x194 how far it was moved
    snd3DGenerator m_snd;       // 0x1A0

public:
    grJungleAshiba12A(const char* taskName);
    virtual ~grJungleAshiba12A();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void requestMove();

    static grJungleAshiba12A* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAshiba12A) == 0x1A8, "Class is wrong size!");

// The stone block on the sides ("N"): it can be hit until it breaks; it shakes after a hit.
class grJungleN : public grJungle {
protected:
    Matrix* m_mtxWork;          // 0x158 the matrix of the block (from the stage)
    u8* m_stateWork;            // 0x15C the state of the block (0 = whole, 1 = broken, ...)
    u8* m_enableWork;           // 0x160
    float m_hp;                 // 0x164 what is left of the block
    u8 m_yakumonoMade;          // 0x168
    char _169[3];
    float m_shakeTimer;         // 0x16C
    Vec3f m_shake;              // 0x170 how far it is shaken
    soCollisionHitData* m_hitData;           // 0x17C the hit sphere
    soCollisionHitData::Simple* m_hitSimple; // 0x180
    soSet<soCollisionHitData>* m_hitSet;     // 0x184
    ykDataGroup* m_dataGroup;   // 0x188
    ykData* m_data;             // 0x18C
    snd3DGenerator m_snd;       // 0x190

public:
    grJungleN(const char* taskName);
    virtual ~grJungleN();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateBreak(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateShake(float deltaFrame);
    virtual void setHit();
    virtual void setMtxWork(Matrix* mtxWork);
    virtual void setStateWork(u8* stateWork);
    virtual void setEnableWork(u8* enableWork);
    virtual void setEnableYakumono();
    virtual void setDisableYakumono();

    static grJungleN* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleN) == 0x198, "Class is wrong size!");

// The broken pieces of the stone block (they fall when the block is broken).
class grJungleNB : public grJungle {
protected:
    Matrix* m_mtxWork;          // 0x158
    u8* m_stateWork;            // 0x15C
    u8 m_motion;                // 0x160

public:
    grJungleNB(const char* taskName);
    virtual ~grJungleNB();
    virtual void update(float deltaFrame);
    virtual void updateBreak(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork);
    virtual void setStateWork(u8* stateWork);
    virtual void requestBreak();

    static grJungleNB* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleNB) == 0x164, "Class is wrong size!");

// The stone ("StoneN"): like the stone block, it breaks with effects.
class grJungleS : public grJungle {
protected:
    Vec3f* m_posWork;           // 0x158
    u8* m_stateWork;            // 0x15C
    u8* m_enableWork;           // 0x160
    float m_hp;                 // 0x164
    float m_effectTimer;        // 0x168
    float m_shakeTimer;         // 0x16C
    Vec3f m_shake;              // 0x170
    u8 m_yakumonoMade;          // 0x17C
    char _17d[3];
    soCollisionHitData* m_hitData;           // 0x180
    soCollisionHitData::Simple* m_hitSimple; // 0x184
    soSet<soCollisionHitData>* m_hitSet;     // 0x188
    ykDataGroup* m_dataGroup;   // 0x18C
    ykData* m_data;             // 0x190
    snd3DGenerator m_snd;       // 0x194

public:
    grJungleS(const char* taskName);
    virtual ~grJungleS();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateBreak(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateShake(float deltaFrame);
    virtual void setHit();
    virtual void setPosWork(Vec3f* posWork);
    virtual void setStateWork(u8* stateWork);
    virtual void setEnableWork(u8* enableWork);
    virtual void setEnableYakumono();
    virtual void setDisableYakumono();

    static grJungleS* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleS) == 0x19C, "Class is wrong size!");

class grJungleSB : public grJungle {
protected:
    Vec3f* m_posWork;           // 0x158
    u8* m_stateWork;            // 0x15C
    u8 m_motion;                // 0x160

public:
    grJungleSB(const char* taskName);
    virtual ~grJungleSB();
    virtual void update(float deltaFrame);
    virtual void updateBreak(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork);
    virtual void setStateWork(u8* stateWork);
    virtual void requestBreak();

    static grJungleSB* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleSB) == 0x164, "Class is wrong size!");

// The water (the model follows the place the stage gives, and it is shown while the stage says so).
class grJungleWater : public grJungle {
protected:
    Vec3f* m_posWork;           // 0x158
    u8* m_enableWork;           // 0x15C

public:
    grJungleWater(const char* taskName);
    virtual ~grJungleWater();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork);
    virtual void setEnableWork(u8* enableWork);

    static grJungleWater* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleWater) == 0x160, "Class is wrong size!");

// One of the three ladders: the stage builds the data of the ladder; the ground adds the area of it and a ladder trigger that
// follows the place the stage gives.
class grJungleLadder : public grGimmickLadder {
protected:
    Vec3f* m_posWork;           // 0x1A0
    Vec3f* m_posHashigoWork;    // 0x1A4 the places of the ladders (to the stage)
    u8 m_triggerMade;           // 0x1A8
    char _1a9[3];

public:
    grJungleLadder(const char* taskName);
    virtual ~grJungleLadder();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork);
    virtual void setPosHashigoWork(Vec3f* posHashigoWork);

    static grJungleLadder* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleLadder) == 0x1AC, "Class is wrong size!");

// The things that hit the fighters (the spikes of the platforms that move in the background): they only have an attack.
class grJungleAttack : public grJungle {
protected:
    Vec3f* m_posWork;           // 0x158
    Vec3f* m_posLimitWork;      // 0x15C
    u8* m_enableWork;           // 0x160
    Vec3f m_offset;             // 0x164 where the attack is from the node
    u8 m_yakumonoMade;          // 0x170
    u8 m_attackOn;              // 0x171
    char _172[2];
    ykData* m_work;             // 0x174

public:
    grJungleAttack(const char* taskName);
    virtual ~grJungleAttack() {
        if (m_work != NULL) {
            delete m_work;
        }
        m_work = NULL;
    }
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setPosWork(Vec3f* posWork);
    virtual void setPosLimitWork(Vec3f* posLimitWork);
    virtual void setEnableWork(u8* enableWork);
};
static_assert(sizeof(grJungleAttack) == 0x178, "Class is wrong size!");

class grJungleAttack03 : public grJungleAttack {
public:
    grJungleAttack03(const char* taskName);
    virtual ~grJungleAttack03();
    virtual void setAttack();

    static grJungleAttack03* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAttack03) == 0x178, "Class is wrong size!");

class grJungleAttack08 : public grJungleAttack {
public:
    grJungleAttack08(const char* taskName);
    virtual ~grJungleAttack08();
    virtual void setAttack();

    static grJungleAttack08* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grJungleAttack08) == 0x178, "Class is wrong size!");
