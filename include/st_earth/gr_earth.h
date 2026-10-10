#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <gf/gf_archive.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <it/it_create.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_3d_generator.h>
#include <so/area/so_area_module_impl.h>
#include <so/collision/so_collision_hit_part.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <st/se_util.h>
#include <st/st_trigger.h>
#include <string.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_earth/st_earth.h>

// The length of a vector (the originals have it inline: the sum of the squares is turned into the length with the reciprocal
// square root, and zero for a vector of nothing).
static inline float grEarthVecLength(Vec3f* v) {
    float lengthSq = v->m_z * v->m_z + v->m_x * v->m_x + v->m_y * v->m_y;
    if (1.17549435e-38f < __fabsf(lengthSq)) {
        return lengthSq * rsqrtf(lengthSq);
    }
    return 0.0f;
}

// MATCH-ONLY: the original scales vectors with paired singles (inline asm in the shared vector code).
static inline void grEarthVec3Scale(register Vec3f* pOut, register const Vec3f* v, register float c) {
    register float fr1, fr0;
    // clang-format off
    asm {
        psq_l    fr1, Vec3f.m_x(v), 0, 0
        psq_l    fr0, Vec3f.m_z(v), 1, 0
        ps_muls0 fr1, fr1, c
        ps_muls0 fr0, fr0, c
        psq_st   fr1, Vec3f.m_x(pOut), 0, 0
        psq_st   fr0, Vec3f.m_z(pOut), 1, 0
    }
    // clang-format on
}

// sora_melee functions that have no names yet.
// itManager::lotCreateItem
extern "C" void fn_27_2AA7AC(itManager* manager, ItemKind kind, int lotId, Vec3f* pos, int a, int b, int c);
// gmCheckGlobalItemSwitch (main, unnamed): whether the item may appear in the match
extern "C" bool fn_8005136C(int itemKind);
// itManager::getItemVariationNum (sora_melee, unnamed)
extern "C" int fn_27_2A0DDC(itManager* manager, int itemKind);
// ecMgr::setDelay (main, unnamed): the effect starts after the frames
extern "C" void fn_8005FF8C(ecMgr* manager, u32 effect, int frames);
// Yakumono::setGlobalOffset (HYPOTHESIS: the name; the arguments are the offset kind and the hit group)
extern "C" void fn_27_263800(Yakumono* yakumono, int kind, int group);
// grYakumono::setAreaShape (sora_melee, unnamed in the symbols)
extern "C" void fn_27_26537C(grYakumono* self, int index, float* offset, float* range);
// Yakumono::getDamage
extern "C" void fn_27_26399C(Yakumono* yakumono);
// Yakumono::setHitStatus (HYPOTHESIS: the name; it sets what a hit sphere does, the arguments are the sphere and two modes)
extern "C" void fn_27_2637E8(Yakumono* yakumono, int index, int mode, int flag);
// the stage that is played (sora_melee .bss)
extern "C" Stage* lbl_27_bss_5668;

// HYPOTHESIS: the 3 bits at the top of the word at cmSubject+8 are a mode that the grounds set to 1 while the subject follows
// them and clear (together with the next 3 bits) when it does not.
struct grEarthSubjectBits {
    u32 mode : 3;
    u32 rest : 29;
};
static inline void grEarthSubjectOn(cmSubject* subject) {
    reinterpret_cast<grEarthSubjectBits*>(reinterpret_cast<u8*>(subject) + 8)->mode = 1;
}
static inline void grEarthSubjectOff(cmSubject* subject) {
    u32* word = reinterpret_cast<u32*>(reinterpret_cast<u8*>(subject) + 8);
    *word = *word & 0x03FFFFFF;
}

// MATCH-ONLY: the start of the settings of the match (the game mode is in the top six bits of the first byte, the byte
// at 0xE says whether items are on)
struct grEarthMeleeView {
    u8 m_gameMode : 6;
    u8 _unused : 2;
    u8 _pad[0xD];
    u8 m_itemsOn;
};

// The base ground of the stage. The grounds that move have the state (the step of their sequence) and a timer.
class grEarth : public grYakumono {
protected:
    u8 m_state;                 // 0x150
    char _151[3];
    float m_timer;              // 0x154

public:
    grEarth(const char* taskName);
    virtual ~grEarth();
};
static_assert(sizeof(grEarth) == 0x158, "Class is wrong size!");

// The branch (the ivy) that the fighters stand on. It has five nodes; when a fighter lands on it, it swings down and up
// again, and the nodes follow with a delay (the wave runs along the branch).
class grEarthBranch : public grEarth {
protected:
    grCollisionJoint* m_joint;  // 0x158 the joint of the collision that the fighter landed on
    float m_landTimer;          // 0x15C frames since a fighter landed on it
    Vec3f m_posLeft;            // 0x160 the left end of the branch (a little more to the left)
    Vec3f m_posRight;           // 0x16C the right end
    Vec3f m_pos[5];             // 0x178 how far each node is moved
    float m_phase[5];           // 0x1B4 where each node is in the wave
    float m_swing;              // 0x1C8 how far the branch swings
    float m_swingSpeed;         // 0x1CC
    u32 m_node[5];              // 0x1D0 the nodes of the model

    float getPosY(int index) { return m_pos[index].m_y; }

public:
    grEarthBranch(const char* taskName);
    virtual ~grEarthBranch();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateCalcPos(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void translateCollision();

    static grEarthBranch* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthBranch) == 0x1E4, "Class is wrong size!");

// The Chappy: it comes into the stage, opens its mouth (it eats the fighters that stand in it), spits an item and goes away.
class grEarthChappy : public grEarth {
protected:
    Vec3f* m_posGimmickWork;    // 0x158 the places of the gimmicks (from the stage)
    u8 m_started;               // 0x15C the first update was done
    u8 m_reqOut;                // 0x15D the stage says that it goes away
    u8 m_reqOpen;               // 0x15E the stage says that it opens its mouth
    char _15f;
    Vec3f m_pos;                // 0x160 where it is
    Vec3f m_posOffset;          // 0x16C
    Vec3f m_rot;                // 0x178 how it is turned (degrees)
    float m_offsetX;            // 0x184 how far to the right of the place of the stage it stands
    float m_landTimer;          // 0x188 frames since a fighter landed on it
    float m_timerStay;          // 0x18C frames until it opens its mouth
    u8 m_motion;                // 0x190
    char _191[3];
    float m_frame;              // 0x194
    float m_frameCount;         // 0x198 the length of the animation that is bound
    u32 m_nodeLocator[0x34];    // 0x19C the nodes of the locators (the places of the fighters when they are eaten)
    u32 m_nodeKosi;             // 0x26C
    u32 m_nodeAgo;              // 0x270
    u32 m_nodeBody;             // 0x274
    u32 m_nodeAsiL;             // 0x278
    u32 m_nodeAsiR;             // 0x27C
    snd3DGenerator m_snd;       // 0x280
    u8 m_seStep;                // 0x288 how many of the sounds of the animation were played
    char _289[3];
    cmSubject m_subject;        // 0x28C the camera follows the chappy
    Vec3f m_posCamera;          // 0x310
    u8 m_cameraOn;              // 0x31C
    u8 m_itemEvent;             // 0x31D
    char _31e[2];
    s32 m_eatState;             // 0x320 1 while a fighter is in the mouth
    stTrigger* m_trigger;       // 0x324 the trigger of the area of the mouth
    soAreaData m_areaData;      // 0x328
    soSet<soAreaData> m_areaDataSet; // 0x348
    ykAreaData m_ykData;        // 0x350

public:
    grEarthChappy(const char* taskName) __attribute__((never_inline));
    virtual ~grEarthChappy();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateColl(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateLanding(float deltaFrame);
    virtual void updateSE(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, float* frameCount);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork);
    virtual bool requestIn();
    virtual bool requestOut();
    virtual bool requestOpen();
    virtual bool requestClose();
    virtual bool isInEnd();
    virtual bool isOutEnd();
    virtual bool isOutEndForce();
    virtual void makeItem();
    virtual void makeItemInMouth();
    virtual void creatEatArea();
    virtual void presentPosEvent();

    static grEarthChappy* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthChappy) == 0x35C, "Class is wrong size!");

// A leaf: a platform that goes down when a fighter (or a pellet) is on it and comes back. There are three of them (the
// type is 0 left, 1 center, 2 right). The pellets that fall on it go down with it.
class grEarthLeaf : public grEarth {
protected:
    u8 m_type;                  // 0x158 which of the three leaves it is (3 = none yet)
    u8 m_stateDown;             // 0x159 the step of the way down
    u8 m_stateReturn;           // 0x15A the step of the way back
    u8 m_seReturn;              // 0x15B the sound of the way back was not played yet
    float m_timerDown;          // 0x15C frames of the step
    float m_downLimit;          // 0x160 how low it can go
    float m_downAmount;         // 0x164 how far it is pressed
    float m_posY;               // 0x168
    float m_speedBack;          // 0x16C
    float m_decay;              // 0x170
    float m_speedDown;          // 0x174
    float m_speedY;             // 0x178
    float m_timerReturn;        // 0x17C frames until the leaf comes back
    float m_weight;             // 0x180 the weight of the fighter on it
    Vec3f* m_posOffsetWork;     // 0x184 how far the leaf is moved (to the stage)
    Vec3f m_posBase;            // 0x188 where the leaf is when it is not pressed
    stEarthPelletData* m_pelletData; // 0x194 the pellets (from the stage)
    snd3DGenerator m_snd;       // 0x198

public:
    grEarthLeaf(const char* taskName);
    virtual ~grEarthLeaf();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateDown(float deltaFrame);
    virtual void updateReturn(float deltaFrame);
    virtual void updatePellet();
    virtual void updateCallBack(float deltaFrame);
    virtual void setDownLimit(float downLimit);
    virtual float getDownLimit();
    virtual void getPos(Vec3f* pos, int mode);
    virtual void getPosPellet(float rate, Vec3f* pos);
    virtual void setType(u8 type);
    virtual void setPosOffsetWork(Vec3f* posOffsetWork);
    virtual void setPelletData(stEarthPelletData* pelletData);

    static grEarthLeaf* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthLeaf) == 0x1A0, "Class is wrong size!");

// The oniyon (a dragonfly): it comes to one of the leaves, lands, and stays there; it makes an item when the stage says so.
// A pellet that hits it makes it go away. The states are 0 (arrive), 1 (wait), 0x10 - 0x12 (the landing), 0x13 (landed), 0x14 /
// 0x15 (the take off) and 0x16 / 0x17 (the item).
class grEarthOniyon : public grEarth {
protected:
    u8 m_color;                 // 0x158 the color of it (0 - 2)
    u8 m_itemStep;              // 0x159 the step of the item (0xFF = over)
    char _15a[2];
    float m_itemTimer;          // 0x15C
    u8 m_itemKind;              // 0x160 which of the items (0 = none)
    u8 m_landingLeaf;           // 0x161 which leaf it lands on
    char _162[2];
    float m_fallSpeed;          // 0x164
    Vec3f m_posOffset;          // 0x168 how far it is from the place it is aimed to
    Vec3f m_posLeaf;            // 0x174 the place of the leaf it lands on (from the stage)
    Vec3f m_posBase;            // 0x180
    Vec3f m_rot;                // 0x18C how it is turned (degrees)
    float m_hp;                 // 0x198
    u8 m_motion;                // 0x19C
    char _19d[3];
    float m_frameCount;         // 0x1A0 the length of the animation that is bound
    u8 m_started;               // 0x1A4 the hit object was made
    char _1a5[3];
    soCollisionHitData* m_hitData;           // 0x1A8 the hit sphere
    soCollisionHitData::Simple* m_hitSimple; // 0x1AC
    soSet<soCollisionHitData>* m_hitSet;     // 0x1B0
    ykDataGroup* m_dataGroup;   // 0x1B4
    ykData* m_data;             // 0x1B8
    snd3DGenerator m_snd;       // 0x1BC
    s32 m_seHandle;             // 0x1C4

public:
    grEarthOniyon(const char* taskName) __attribute__((never_inline));
    virtual ~grEarthOniyon();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void makeItem();
    virtual void setMotion(u32 animId, bool loop, float* frameCount);
    virtual void requestLanding(u8 leaf);
    virtual void requestTakeOff();
    virtual void requestItem(int kind);
    virtual bool isPreLanding();
    virtual bool isLanding();
    virtual bool isTakeOff();
    virtual void setPosLeaf(float x, float y, float z);
    virtual u8 getLandingLeaf();
    virtual u8 selectColor(int lastColor);

    static grEarthOniyon* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthOniyon) == 0x1C8, "Class is wrong size!");

// The pellet (a Pikmin pellet that sits on a leaf and is made into an item when it is ripe). The stage keeps the data of all
// the pellets; this ground shows one of them (its own data is given by setPelletData).
class grEarthPellet : public grEarth {
protected:
    Matrix m_mtx;                       // 0x158 the place of the pellet
    stEarthPelletData* m_pelletData;    // 0x188 the data of this pellet (from the stage)

public:
    // (inline: the original has no constructor of its own)
    inline grEarthPellet(const char* taskName) : grEarth(taskName) {
        m_pelletData = NULL;
        m_mtx.setIdentity();
        grCalcWorldCallBack* callback = &m_calcWorldCallBack;
        if (callback != NULL) {
            callback->m_numNodeCallbackData = 1;
            callback->initialize(false, Heaps::StageInstance);
            callback->m_nodeCallbackDatas[0].m_flags |= 8;
        }
    }
    virtual ~grEarthPellet();
    virtual void update(float deltaFrame);
    virtual void updatePos();
    virtual void updateState(float deltaFrame);
    virtual void updateMotion();
    virtual void updateCallBack();
    virtual void changeColor();
    virtual void setPelletData(stEarthPelletData* pelletData);

    static grEarthPellet* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthPellet) == 0x18C, "Class is wrong size!");

// The flower of the pellets: it grows the pellet, and it opens when it is hit. The pellets get bigger over time (the stage
// keeps the data of each of them) and the flower shows it with its animations.
class grEarthPelletFlower : public grEarth {
protected:
    u8 m_motion;                // 0x158
    char _159[3];
    float m_frameCount;         // 0x15C the length of the animation that is bound
    Vec3f* m_posWork;           // 0x160 (to the stage)
    stEarthPelletData* m_pelletData; // 0x164 the data of the pellet (from the stage)
    u8 m_started;               // 0x168 the hit object was made
    char _169[3];
    soCollisionHitData* m_hitData;           // 0x16C the two hit spheres
    soCollisionHitData::Simple* m_hitSimple; // 0x170
    soSet<soCollisionHitData>* m_hitSet;     // 0x174
    ykDataGroup* m_dataGroup;   // 0x178
    ykData* m_data;             // 0x17C
    snd3DGenerator m_snd;       // 0x180

public:
    grEarthPelletFlower(const char* taskName) __attribute__((never_inline));
    virtual ~grEarthPelletFlower();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateState(float deltaFrame);
    virtual void updateMotion();
    virtual void updateCallBack();
    virtual void setHit();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPelletData(stEarthPelletData* pelletData);
    virtual void setPosWork(Vec3f* posWork);

    static grEarthPelletFlower* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthPelletFlower) == 0x188, "Class is wrong size!");

// snd3DGenerator::isPlay (main, unnamed): whether the sound of the handle is still playing
extern "C" bool fn_800797A4(snd3DGenerator* generator, s32 handle);

// The river that runs under the stage: it starts with the rain (the stage says it) and runs for the length of its animation;
// the speed of the river is given to the stage (the belt conveyor on the water follows it).
class grEarthRiver : public grEarth {
protected:
    u8 m_riverState;            // 0x158 the step of the river (0 = hidden, 2 = starts, 3 = runs, 4 - 7 = loops and ends)
    char _159[3];
    float m_timerRiver;         // 0x15C
    u8 m_endRequest;            // 0x160 the stage says that the river ends
    char _161[3];
    float* m_speedWork;         // 0x164 how fast the river runs (to the stage)
    float m_framePrev;          // 0x168
    float m_frameCount;         // 0x16C the length of the animation that is bound
    snd3DGenerator m_snd;       // 0x170
    s32 m_seHandle;             // 0x178 the sound of the river

public:
    // (inline: the original has no constructor of its own)
    inline grEarthRiver(const char* taskName) : grEarth(taskName), m_snd() {
        setMdlIndex(6);
        m_riverState = 0;
        m_timerRiver = 0.0f;
        m_framePrev = 0.0f;
        m_frameCount = 0.0f;
        m_endRequest = 0;
        m_speedWork = NULL;
        m_seHandle = 0;
    }
    virtual ~grEarthRiver();
    virtual void update(float deltaFrame);
    virtual void startRiver();
    virtual void endRiver();
    virtual void setMotion(u32 animId, bool loop, float* frameCount);
    virtual void setSpeedWork(float* speedWork);

    static grEarthRiver* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthRiver) == 0x17C, "Class is wrong size!");

// The sun: it turns the weather to fine or to rain (the stage asks for it) and says when the weather has changed far enough.
// The states are 0xA - 0xC (fine) and 0xD - 0xF (rain).
class grEarthSun : public grEarth {
protected:
    float m_savedFrame;         // 0x158 the frame at which the animation stops
    u8 m_request;               // 0x15C the stage asks for the other weather
    char _15d[3];

public:
    // (inline: the original has no constructor of its own)
    inline grEarthSun(const char* taskName) : grEarth(taskName) {
        m_savedFrame = 0.0f;
        m_request = 0;
    }
    virtual ~grEarthSun();
    virtual void update(float deltaFrame);
    virtual void requestToFine();
    virtual void requestToRain();
    virtual void setMotion(u32 animId, bool loop, float* frameCount);
    virtual bool isChangeWeather();

    static grEarthSun* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthSun) == 0x160, "Class is wrong size!");

// The stump (a tree stump with a platform): the pellets that lie on it follow the places of the nodes of the model; the places
// of the stage are given from the nodes.
class grEarthStump : public grEarth {
protected:
    stEarthPelletData* m_pelletData; // 0x158 the pellets (from the stage)
    Vec3f* m_posGimmickWork;    // 0x15C the places of the nodes (to the stage)
    grCollisionJoint* m_joint;  // 0x160 the joint of the collision of this ground

public:
    // (inline: the original has no constructor of its own)
    inline grEarthStump(const char* taskName) : grEarth(taskName) {
        m_pelletData = NULL;
        m_posGimmickWork = NULL;
        m_joint = NULL;
    }
    virtual ~grEarthStump();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateJoint();
    virtual void updatePellet();
    virtual void getPosPellet(float rate, Vec3f* pos);
    virtual void setPelletData(stEarthPelletData* pelletData);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork);
    virtual void setAILowPriority(u8 flag);
    virtual void setCollisionAttr(u8 material);

    static grEarthStump* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthStump) == 0x164, "Class is wrong size!");

// The rain: it follows the weather of the stage (0 = fine, 3 = none; the rest is rain) and shows the animation of it.
class grEarthRain : public grEarth {
protected:
    u8* m_weatherWork;          // 0x158 the weather (from the stage)
    u8 m_motion;                // 0x15C
    char _15d[3];
    float m_frameCount;         // 0x160 the length of the animation that is bound

public:
    // (inline: the original has no constructor of its own)
    inline grEarthRain(const char* taskName) : grEarth(taskName) {
        m_weatherWork = NULL;
        m_motion = 3;
        m_frameCount = 0.0f;
    }
    virtual ~grEarthRain();
    virtual void update(float deltaFrame);
    virtual void updateWeather(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setWeatherWork(u8* weatherWork);

    static grEarthRain* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthRain) == 0x164, "Class is wrong size!");

// The drops that fly up from the water (three of them: left, right and center): shown while it rains.
class grEarthHaneMizu : public grEarth {
protected:
    u8* m_weatherWork;          // 0x158 the weather (from the stage)
    Vec3f* m_posOffsetWork;     // 0x15C where the leaf is (from the stage)

public:
    // (inline: the original has no constructor of its own)
    inline grEarthHaneMizu(const char* taskName) : grEarth(taskName) {
        m_weatherWork = NULL;
        m_posOffsetWork = NULL;
        grCalcWorldCallBack* callback = &m_calcWorldCallBack;
        if (callback != NULL) {
            callback->m_numNodeCallbackData = 1;
            callback->initialize(false, Heaps::StageInstance);
            callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
        }
    }
    virtual ~grEarthHaneMizu();
    virtual void update(float deltaFrame);
    virtual void updateWeather();
    virtual void updateCallBack();
    virtual void setWeatherWork(u8* weatherWork);
    virtual void setPosOffsetWork(Vec3f* posOffsetWork);

    static grEarthHaneMizu* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthHaneMizu) == 0x160, "Class is wrong size!");

// The background (the big model): nothing but the model.
class grEarthMainBg : public grEarth {
public:
    // (inline: the original has no constructor of its own)
    inline grEarthMainBg(const char* taskName) : grEarth(taskName) { }
    virtual ~grEarthMainBg();

    static grEarthMainBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grEarthMainBg) == 0x158, "Class is wrong size!");
