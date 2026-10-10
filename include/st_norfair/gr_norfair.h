#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <so/collision/so_collision_hit_part.h>
#include <st/se_util.h>
#include <string.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_norfair/st_norfair.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grNorfairHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// The base ground of the stage. The grounds that move have the state (the step of their sequence), a timer and the number
// of the event of the stage.
class grNorfair : public grYakumono {
protected:
    u8 m_state;                 // 0x150
    char _151[3];
    float m_timer;              // 0x154
    u8* m_eventIDWork;          // 0x158 the event of the stage (from the stage)

public:
    grNorfair(const char* taskName);
    virtual ~grNorfair();
    virtual void setEventIDWork(u8* eventIDWork) { m_eventIDWork = eventIDWork; }
};
static_assert(sizeof(grNorfair) == 0x15C, "Class is wrong size!");

// The platforms that come out of the lava and go back.
class grNorfairAshiba : public grNorfair {
protected:
    u8* m_posIndex;             // 0x15C the zone that is in use (from the stage)
    u8 m_motion;                // 0x160
    char _161[3];
    float m_prevFrame;          // 0x164 the frame of the last update
    float m_frameCount;         // 0x168

public:
    // (inline: the original has no constructor of its own)
    grNorfairAshiba(const char* taskName) : grNorfair(taskName) {
        m_posIndex = NULL;
        m_motion = 2;
        m_prevFrame = 0.0f;
        m_frameCount = 0.0f;
    }
    virtual ~grNorfairAshiba();
    virtual void update(float deltaFrame);
    virtual void setMotionFrame(float frame, u32 index);
    virtual void updateActive(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosIndex(u8* posIndex) { m_posIndex = posIndex; }

    static grNorfairAshiba* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairAshiba) == 0x16C, "Class is wrong size!");

// The background. The places of the platforms of the stage are the places of its nodes.
class grNorfairBg : public grNorfair {
protected:
    Vec3f* m_posZoneWork;       // 0x15C the places of the zones (to the stage)
    Vec3f* m_posAshibaTWork;    // 0x160 the places of the platforms of the trainer (to the stage)
    u32 m_nodeZone[5];          // 0x164 the nodes of the zones
    u32 m_nodeAshibaT[4];       // 0x178 the nodes of the platforms of the trainer

public:
    // (inline: the original has no constructor of its own)
    grNorfairBg(const char* taskName) : grNorfair(taskName) {
        m_posZoneWork = NULL;
        m_posAshibaTWork = NULL;
        memset(m_nodeZone, 0, sizeof(m_nodeZone));
        memset(m_nodeAshibaT, 0, sizeof(m_nodeAshibaT));
    }
    virtual ~grNorfairBg();
    virtual void processAnim();
    virtual bool setNode();
    virtual void setPosZoneWork(Vec3f* posZoneWork) { m_posZoneWork = posZoneWork; }
    virtual void setPosAshibaTWork(Vec3f* posAshibaTWork) { m_posAshibaTWork = posAshibaTWork; }

    static grNorfairBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairBg) == 0x188, "Class is wrong size!");

// The door that blocks the way in the zone, it opens when it is hit.
class grNorfairDoor : public grNorfair {
protected:
    Vec3f* m_posWork;           // 0x15C the places the stage gives it
    u8* m_posIndex;             // 0x160 the one of them that is its place
    u8* m_eventIDWork;          // 0x164 the event of the stage (it keeps its own, the one of the base is not used)
    u8* m_stateWork;            // 0x168 the state of the door (to the stage)
    float m_scaleY;             // 0x16C
    u8 m_motion;                // 0x170
    char _171[3];
    float m_frameCount;         // 0x174
    u8 m_hasHit;                // 0x178
    char _179[3];
    soCollisionHitData* m_hitData;           // 0x17C the hit boxes
    soCollisionHitData::Simple* m_hitSimple; // 0x180 their form for the hit object
    soSet<soCollisionHitData>* m_hitSet;     // 0x184
    ykDataGroup* m_dataGroup;                // 0x188
    ykData* m_data;                          // 0x18C
    snd3DGenerator m_snd;       // 0x190

public:
    grNorfairDoor(const char* taskName);
    virtual ~grNorfairDoor();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setEventIDWork(u8* eventIDWork) { m_eventIDWork = eventIDWork; }
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void setHit();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setPosIndex(u8* posIndex) { m_posIndex = posIndex; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }

    static grNorfairDoor* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairDoor) == 0x198, "Class is wrong size!");

// The zone: a part of the stage where the fighters are safe from the lava. It shows when it opens and the camera looks at it.
class grNorfairZone : public grNorfair {
protected:
    Vec3f* m_posWork;           // 0x15C the places the stage gives it
    Vec3f* m_posLimitWork;      // 0x160 the part of the stage the camera shows
    u8* m_posIndex;             // 0x164 the one of the places that is its place
    u8* m_stateWork;            // 0x168 the state of the zone (to the stage)
    u8* m_inOutWork;            // 0x16C a fighter is in the zone (to the stage)
    float m_inTimer[4];         // 0x170 frames until the fighter counts as out
    float m_scaleY;             // 0x180
    u8 m_motion;                // 0x184
    char _185[3];
    float m_frameCount;         // 0x188
    snd3DGenerator m_snd;       // 0x18C
    cmSubject m_subject;        // 0x194
    s32 m_dangerZone[4];        // 0x218

public:
    grNorfairZone(const char* taskName);
    virtual ~grNorfairZone();
    virtual void update(float deltaFrame);
    virtual void updateArea(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateAI(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setPosLimitWork(Vec3f* posLimitWork) { m_posLimitWork = posLimitWork; }
    virtual void setPosIndex(u8* posIndex) { m_posIndex = posIndex; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setInOutWork(u8* inOutWork) { m_inOutWork = inOutWork; }

    static grNorfairZone* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairZone) == 0x228, "Class is wrong size!");

// The shutter of the zone.
class grNorfairShutter : public grNorfair {
protected:
    Vec3f* m_posWork;           // 0x15C the places the stage gives it
    u8* m_posIndex;             // 0x160 the one of them that is its place
    u8 m_motion;                // 0x164
    char _165[3];
    float m_frameCount;         // 0x168

public:
    // (inline: the original has no constructor of its own)
    grNorfairShutter(const char* taskName) : grNorfair(taskName) {
        m_posWork = NULL;
        m_posIndex = NULL;
        m_motion = 2;
        m_frameCount = 0.0f;
        grCalcWorldCallBack* callback = &m_calcWorldCallBack;
        if (callback == NULL) {
            return;
        }
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
    virtual ~grNorfairShutter();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setPosIndex(u8* posIndex) { m_posIndex = posIndex; }

    static grNorfairShutter* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairShutter) == 0x16C, "Class is wrong size!");

// The platforms of the trainer move up and down between three places.
class grNorfairAshibaT : public grNorfair {
protected:
    Vec3f* m_posWork;           // 0x15C the places of the platform (to the stage)
    Vec3f* m_posMagmaWork;      // 0x160 the lava (from the stage)
    Vec3f m_offset;             // 0x164 where the platform is now (the y is the height)
    float m_posYFrom;           // 0x170 the height the move starts at
    float m_posYTo;             // 0x174 the height it goes to
    float m_moveTime;           // 0x178 frames the move takes
    u8 m_index;                 // 0x17C the place it is at
    u8 m_first;                 // 0x17D
    char _17e[2];

public:
    grNorfairAshibaT(const char* taskName);
    virtual ~grNorfairAshibaT();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setPosMagmaWork(Vec3f* posMagmaWork) { m_posMagmaWork = posMagmaWork; }

    static grNorfairAshibaT* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairAshibaT) == 0x180, "Class is wrong size!");

// The walls of lava that come in from the sides and push the fighters.
class grNorfairWall : public grNorfair {
protected:
    Vec3f* m_posLimitWork;      // 0x15C the part of the stage the wall moves in (from the stage)
    Vec3f m_pos;                // 0x160 the place of the wall (the x moves)
    float m_speed;              // 0x16C
    float m_target;             // 0x170 where the wall stops, relative to the camera
    float m_speedPrev;          // 0x174 the speed of the last frame
    u8 m_type;                  // 0x178 0 = left, 1 = right
    u8 m_hasHit;                // 0x179
    u8 m_attackSet;             // 0x17A
    char _17b;
    ykData* m_attackWork;       // 0x17C the work of the hit object (two numbers)
    u8 m_soundStarted;          // 0x180
    char _181[3];
    s32 m_seHandle;             // 0x184 the sound that plays while it moves (-1 = none)
    snd3DGenerator m_snd;       // 0x188
    cmSubject m_subject;        // 0x190
    s32 m_dangerZone;           // 0x214

public:
    grNorfairWall(const char* taskName);
    virtual ~grNorfairWall();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateAI(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttackLeft();
    virtual void setAttackRight();
    virtual void setAttackDetails(int index, Vec3f* offset);
    virtual void setTypeLeft() { m_type = 0; }
    virtual void setTypeRight() { m_type = 1; }
    virtual void setPosLimitWork(Vec3f* posLimitWork) { m_posLimitWork = posLimitWork; }

    static grNorfairWall* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairWall) == 0x218, "Class is wrong size!");

// The wave of lava that sweeps over the stage.
class grNorfairWave : public grNorfair {
protected:
    Vec3f* m_posWork;           // 0x15C the place the stage gives it
    u8 m_unk160;                // 0x160
    char _161[3];
    float m_wait;               // 0x164
    StSeUtil::SeSeqInstance<1, 2> m_sePlayer; // 0x168
    SndID m_seIds[2];           // 0x1B4
    StSeUtil::UnkStruct m_seData[2]; // 0x1BC
    u8 m_sePhase;               // 0x1DC
    char _1dd[3];
    s32 m_seHandle;             // 0x1E0 the sound that plays while the wave goes (-1 = none)

public:
    grNorfairWave(const char* taskName);
    virtual ~grNorfairWave();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }

    static grNorfairWave* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairWave) == 0x1E4, "Class is wrong size!");

// The lava. It has 18 nodes with a collision of the lava (6 on the left, 6 in the center and 6 on the right) that burn.
class grNorfairMagma : public grNorfair {
protected:
    u8 m_tblIndex;              // 0x15C the row of the table of the lava that is in use
    char _15d[3];
    float m_accel;              // 0x160
    float m_accelPrev;          // 0x164
    float m_offset;             // 0x168 a random shift of the height
    float m_goal;               // 0x16C the height the lava goes to
    float m_unk170;             // 0x170
    float m_height;             // 0x174 the height of the lava
    float m_unk178;             // 0x178
    stDataMultiContainer* m_tblLevelAcc; // 0x17C the table of the lava (from the stage)
    Vec3f* m_posMagmaWork;      // 0x180 the place of the lava (to the stage)
    Vec3f* m_posLimitWork;      // 0x184 the part of the stage the camera shows
    u32 m_nodeColl[18];         // 0x188 the nodes of the collisions
    u8 m_hasHit;                // 0x1D0
    u8 m_attackSet;             // 0x1D1
    char _1d2[2];
    ykData* m_attackWork;       // 0x1D4 the work of the hit object (two numbers)
    s32 m_dangerZone;           // 0x1D8

public:
    grNorfairMagma(const char* taskName);
    virtual ~grNorfairMagma();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateYakumono(float deltaFrame);
    virtual void updateAI(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateLevel(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setAttackDetails(u8 index, u32 nodeIndex, Vec3f* offset);
    virtual void setTblLevelAcc(stDataMultiContainer* tblLevelAcc) { m_tblLevelAcc = tblLevelAcc; }
    virtual void setPosMagmaWork(Vec3f* posMagmaWork) { m_posMagmaWork = posMagmaWork; }
    virtual void setPosLimitWork(Vec3f* posLimitWork) { m_posLimitWork = posLimitWork; }

    static grNorfairMagma* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grNorfairMagma) == 0x1DC, "Class is wrong size!");

// HYPOTHESIS: inline helper of the original (a tolerance test).
static inline bool norfairIsNearZero(float value) {
    bool result = false;
    if ((float)fabs(value) < 1e-5f) {
        result = true;
    }
    return result;
}
