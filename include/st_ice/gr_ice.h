#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
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

#include <st_ice/st_ice.h>

// The length of a vector (the originals have it inline: the sum of the squares is turned into the length with the reciprocal
// square root, and zero for a vector of nothing).
static inline float grIceVecLength(Vec3f* v) {
    float lengthSq = v->m_z * v->m_z + v->m_x * v->m_x + v->m_y * v->m_y;
    if (1.17549435e-38f < __fabs(lengthSq)) {
        return lengthSq * rsqrtf(lengthSq);
    }
    return 0.0f;
}

// The base ground of the stage. The grounds that move have the state (the step of their sequence) and a timer.
class grIce : public grYakumono {
protected:
    u8 m_state;                 // 0x150
    char _151[3];
    float m_timer;              // 0x154

public:
    grIce(const char* taskName);
    virtual ~grIce();

    static grIce* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIce) == 0x158, "Class is wrong size!");

// The sign that warns about the vegetables coming from above (it plays a sound sequence while it is shown).
class grIceWarning : public grIce {
protected:
    u8* m_stateWork;            // 0x158 the state of the sign (from the stage, 5 = it is on)
    StSeUtil::SeSeqInstance<1, 1> m_seSeq; // 0x15C
    SndID m_seSeqId;            // 0x198
    StSeUtil::UnkStruct m_seSeqData[6]; // 0x19C

public:
    // (inline: the original has no constructor of its own)
    grIceWarning(const char* taskName) : grIce(taskName), m_seSeq() {
        m_stateWork = NULL;
        // The sound of the sign: the same sound every 20 frames from the start (frames 0 - 100).
        m_seSeqId = static_cast<SndID>(0x1C8C);
        m_seSeqData[0].id = static_cast<SndID>(0x1C8C);
        m_seSeqData[0].unk4 = 0.0f;
        m_seSeqData[0].unk8 = 0.0f;
        m_seSeqData[0].unkC = 0.0f;
        m_seSeqData[1].id = static_cast<SndID>(0x1C8C);
        m_seSeqData[1].unk4 = 0.0f;
        m_seSeqData[1].unk8 = 20.0f;
        m_seSeqData[1].unkC = 0.0f;
        m_seSeqData[2].id = static_cast<SndID>(0x1C8C);
        m_seSeqData[2].unk4 = 0.0f;
        m_seSeqData[2].unk8 = 40.0f;
        m_seSeqData[2].unkC = 0.0f;
        m_seSeqData[3].id = static_cast<SndID>(0x1C8C);
        m_seSeqData[3].unk4 = 0.0f;
        m_seSeqData[3].unk8 = 60.0f;
        m_seSeqData[3].unkC = 0.0f;
        m_seSeqData[4].id = static_cast<SndID>(0x1C8C);
        m_seSeqData[4].unk4 = 0.0f;
        m_seSeqData[4].unk8 = 80.0f;
        m_seSeqData[4].unkC = 0.0f;
        m_seSeqData[5].id = static_cast<SndID>(0x1C8C);
        m_seSeqData[5].unk4 = 0.0f;
        m_seSeqData[5].unk8 = 100.0f;
        m_seSeqData[5].unkC = 0.0f;
        m_seSeq.registId(&m_seSeqId, 1);
        m_seSeq.registSeq(0, m_seSeqData, 6, Heaps::StageInstance);

    }
    virtual ~grIceWarning();
    virtual void update(float deltaFrame);
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }

    static grIceWarning* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceWarning) == 0x1FC, "Class is wrong size!");

// The background. It tells the stage where the nodes of the model are (the clouds, the water) and lets the fighters walk on
// it when the stage is on the part of the mountain with the floor.
class grIceBg : public grIce {
protected:
    Matrix* m_mtxGimmickWork;   // 0x158 the places of the nodes of the clouds and the ice (to the stage)
    Vec3f* m_posFishWork;       // 0x15C the places of the nodes of the water (two, to the stage)
    u8* m_stateWork;            // 0x160 the part of the mountain the scene is on (from the stage)

public:
    // (inline: the original has no constructor of its own)
    grIceBg(const char* taskName) : grIce(taskName) {
        m_mtxGimmickWork = NULL;
        m_posFishWork = NULL;
        m_stateWork = NULL;
    }
    virtual ~grIceBg();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork) { m_mtxGimmickWork = mtxGimmickWork; }
    virtual void setPosFishWork(Vec3f* posFishWork) { m_posFishWork = posFishWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }

    static grIceBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceBg) == 0x164, "Class is wrong size!");

// The platform that rolls (it tilts when a fighter lands on it and falls back).
class grIceRot : public grIce {
protected:
    Matrix* m_mtxWork;          // 0x158 the place of the node (from the stage)
    float m_unk15C;             // 0x15C
    float m_unk160;             // 0x160
    float m_angle;              // 0x164 how far the platform is turned
    float m_landTimer;          // 0x168 frames since a fighter landed on it
    float m_speed;              // 0x16C
    float m_dir;                // 0x170 the way it turns
    float m_accel;              // 0x174
    float m_maxSpeed;           // 0x178
    float m_scale;              // 0x17C
    snd3DGenerator m_snd;       // 0x180

public:
    grIceRot(const char* taskName) __attribute__((never_inline));
    virtual ~grIceRot();
    virtual void update(float deltaFrame);
    virtual void updateRot(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }

    static grIceRot* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceRot) == 0x188, "Class is wrong size!");

// The cloud platform that comes from one side, stays and goes away.
class grIceKumo : public grIce {
protected:
    Matrix* m_mtxWork;          // 0x158 the places of the two parts of the cloud (from the stage)
    float m_posX;               // 0x15C
    float m_posY;               // 0x160
    float m_posZ;               // 0x164
    Vec3f* m_limitWork;         // 0x168 the part of the stage the camera shows (from the stage)
    u8* m_stateWork;            // 0x16C the state of the cloud (to the stage)

public:
    // (inline: the original has no constructor of its own)
    grIceKumo(const char* taskName) : grIce(taskName) {
        m_mtxWork = NULL;
        m_posX = 0.0f;
        m_posY = 0.0f;
        m_posZ = 0.0f;
        m_limitWork = NULL;
        m_stateWork = NULL;
        grCalcWorldCallBack* callback = &m_calcWorldCallBack;
        if (callback == NULL) {
            return;
        }
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
    virtual ~grIceKumo();
    virtual void update(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setLimitWork(Vec3f* limitWork) { m_limitWork = limitWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }

    static grIceKumo* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceKumo) == 0x170, "Class is wrong size!");

// The ice block that floats down and bobs when a fighter lands on it.
class grIceIce : public grIce {
protected:
    Matrix* m_mtxWork;          // 0x158 the place of the node (from the stage)
    Matrix* m_mtxGimmickWork;   // 0x15C the place of the node of the water
    float m_posX;               // 0x160
    float m_posY;               // 0x164
    float m_posZ;               // 0x168
    float m_unk16C;             // 0x16C
    float m_bobY;               // 0x170 how far the block is pushed down
    float m_unk174;             // 0x174
    u8* m_stateWork;            // 0x178 the state of the block (to the stage)
    u8 m_sinking;               // 0x17C the block goes down
    char _17d[3];
    float m_landTimer;          // 0x180 frames since a fighter landed on it
    Vec3f* m_limitWork;         // 0x184 the part of the stage the camera shows (from the stage)
    snd3DGenerator m_snd;       // 0x188

public:
    grIceIce(const char* taskName) __attribute__((never_inline));
    virtual ~grIceIce();
    virtual void update(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateUpDown(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork) { m_mtxGimmickWork = mtxGimmickWork; }
    virtual void setLimitWork(Vec3f* limitWork) { m_limitWork = limitWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }

    static grIceIce* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceIce) == 0x190, "Class is wrong size!");

// The white bear that walks along the top of the mountain, stops and looks around.
class grIceBear : public grIce {
protected:
    Vec3f* m_posWork;           // 0x158 the places it walks to (from the stage)
    Vec3f* m_limitWork;         // 0x15C the part of the stage the camera shows (from the stage)
    u8* m_stateWork;            // 0x160 the state of the bear (0xE = nothing, to the stage)
    Vec3f m_pos;                // 0x164 where it is
    Vec3f m_rot;                // 0x170 where it looks (the y is the angle)
    float m_fallAccel;          // 0x17C
    float m_speedY;             // 0x180
    float m_ratio;              // 0x184 how far it is on the way between two places
    u8 m_bounce;                // 0x188 how many times it bounced
    u8 m_quakeSet;              // 0x189 the quake of the step was started
    u8 m_motion;                // 0x18A
    char _18b;
    float m_motionTimer;        // 0x18C frames until the animation is over
    float m_motionLength;       // 0x190 frames the animation has
    snd3DGenerator m_snd;       // 0x194

public:
    grIceBear(const char* taskName) __attribute__((never_inline));
    virtual ~grIceBear();
    virtual void update(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void calcPos(Vec3f* pos);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setLimitWork(Vec3f* limitWork) { m_limitWork = limitWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }

    static grIceBear* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceBear) == 0x19C, "Class is wrong size!");

// The fish that jumps out of the water, bites with its tail and eats a fighter that is in the area of its mouth. The
// states of the sequence are 0 (waiting), 1 (the stage says it comes), 6 (the splash) and 7 (the jump).
class grIceFish : public grIce {
protected:
    Vec3f* m_posWork;           // 0x158 the places of the water (from the stage)
    float* m_posXWork;          // 0x15C the place the fish jumps to (from the stage, the x is in the first number)
    Vec3f* m_limitWork;         // 0x160 the part of the stage the camera shows (from the stage)
    u8* m_stateWork;            // 0x164 the state of the fish (to the stage)
    u8 m_motion;                // 0x168
    char _169[3];
    float m_motionTimer;        // 0x16C frames until the animation is over
    u8 m_started;               // 0x170 the area of the mouth was made
    u8 m_attackSet;             // 0x171 the attack of the tail is on
    char _172[2];
    ykData* m_attackWork;       // 0x174
    Vec3f m_pos;                // 0x178 where the fish is
    u32 m_effectHandle;         // 0x184 the splash effect
    snd3DGenerator m_snd;       // 0x188
    u8 m_sePlayed;              // 0x190
    char _191[3];
    s32 m_dangerZone;           // 0x194 the id of the place the fighters have to avoid (-1 = none)
    s32 m_eatState;             // 0x198 1 while a fighter is in the mouth
    stTrigger* m_trigger;       // 0x19C the trigger of the area of the mouth
    soAreaData m_areaData;      // 0x1A0
    soSet<soAreaData> m_areaDataSet; // 0x1C0
    ykAreaData m_ykData;        // 0x1C8

public:
    grIceFish(const char* taskName) __attribute__((never_inline));
    virtual ~grIceFish();
    virtual void update(float deltaFrame);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setAttackTail();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setPosXWork(float* posXWork) { m_posXWork = posXWork; }
    virtual void setLimitWork(Vec3f* limitWork) { m_limitWork = limitWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void createEatArea();
    virtual void presentPosEvent();

    static grIceFish* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceFish) == 0x1D4, "Class is wrong size!");

// The icicle that hangs from the mountain, shakes when it is hit and falls when the stage says so (it hurts a fighter it
// falls on). There are three of them (the type is 0 - 2); only the one the stage chooses is shown.
class grIceTurara : public grIce {
protected:
    Matrix* m_mtxWork;          // 0x158 the place of the node (from the stage)
    Vec3f* m_posLimitWork;      // 0x15C the part of the stage the camera shows (from the stage)
    Vec3f m_pos;                // 0x160 where it is
    Vec3f m_rot;                // 0x16C how it is turned (degrees)
    u8* m_stateWork;            // 0x178 the state of the icicles (to the stage)
    u8 m_type;                  // 0x17C which of the three it is
    char _17d[3];
    float* m_hpWork;            // 0x180 how many hits the icicles hold (to the stage)
    float m_fallSpeed;          // 0x184
    float m_hitTimer;           // 0x188 frames until the next hit effect
    u8 m_hasHit;                // 0x18C the hit object was made
    u8 m_attackSet;             // 0x18D the attack is on
    char _18e[2];
    soCollisionHitData* m_hitData;           // 0x190 the hit sphere
    soCollisionHitData::Simple* m_hitSimple; // 0x194 its form for the hit object
    soSet<soCollisionHitData>* m_hitSet;     // 0x198
    ykDataGroup* m_dataGroup;                // 0x19C
    ykData* m_data;                          // 0x1A0
    snd3DGenerator m_snd;       // 0x1A4

public:
    grIceTurara(const char* taskName) __attribute__((never_inline));
    virtual ~grIceTurara();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setPosLimitWork(Vec3f* posLimitWork) { m_posLimitWork = posLimitWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setType(u8 type) { m_type = type; }
    virtual void setHPWork(float* hpWork) { m_hpWork = hpWork; }

    static grIceTurara* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceTurara) == 0x1AC, "Class is wrong size!");

// The floor that breaks (it shakes when it is hit, breaks when the hits are too many and builds up again after a time).
class grIceBreak : public grIce {
protected:
    float m_preBuild;           // 0x158 frames until the blink of the floor that is building up changes
    float m_hitTimer;           // 0x15C frames until the next hit effect
    Matrix* m_mtxWork;          // 0x160 the place of the node (from the stage)
    float m_hp;                 // 0x164 how many hits are left (0 = broken)
    float m_shakeTimer;         // 0x168 frames the floor shakes
    Vec3f m_shake;              // 0x16C how far the floor is shaken
    u8 m_hasHit;                // 0x178 the hit object was made
    char _179[3];
    soCollisionHitData* m_hitData;           // 0x17C the hit sphere
    soCollisionHitData::Simple* m_hitSimple; // 0x180 its form for the hit object
    soSet<soCollisionHitData>* m_hitSet;     // 0x184
    ykDataGroup* m_dataGroup;                // 0x188
    ykData* m_data;                          // 0x18C
    snd3DGenerator m_snd;       // 0x190

public:
    grIceBreak(const char* taskName) __attribute__((never_inline));
    virtual ~grIceBreak();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateYakumono(float deltaFrame);
    virtual void updatePreBuild(float deltaFrame);
    virtual void updateShake(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }

    static grIceBreak* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceBreak) == 0x198, "Class is wrong size!");

// The mountain. Its model shakes and moves as the stage goes through its parts (the scroll up the mountain, the avalanche that
// ends with a crash), and it hands the places of the nodes (the clouds, the ice, the bears) to the stage. The state of the stage
// (0 - 4) says which part the mountain is on; the state of the ground is the step in that part.
class grIceYama : public grIce {
protected:
    u8 m_unk158;                // 0x158
    char _159[3];
    Matrix* m_mtxGimmickWork;   // 0x15C the places of the nodes of the mountain (to the stage)
    Vec3f* m_posBearWork;       // 0x160 the places of the nodes of the bear (to the stage)
    u8* m_stateWork;            // 0x164 the part of the mountain the scene is on (from the stage)
    u8* m_stateBearWork;        // 0x168 the state of the bear (from and to the stage)
    u8 m_prevStage;             // 0x16C the part of the last update
    u8 m_unk16D;                // 0x16D
    char _16e[2];
    float m_speed;              // 0x170 the speed of the shake
    float m_limit;              // 0x174 how far the mountain moves
    float m_offsetX;            // 0x178
    float m_offsetY;            // 0x17C
    float m_offsetZ;            // 0x180
    u32 m_nodeKaiten;           // 0x184 the nodes of the model
    u32 m_nodeKoware;           // 0x188
    u32 m_nodeTSide[3];         // 0x18C
    u32 m_nodeTurara;           // 0x198
    u32 m_nodeBear[5];          // 0x19C
    grCollisionJoint* m_joint;  // 0x1B0 the joint of the collision that moves with the mountain
    u8 m_hasHit;                // 0x1B4 the hit object was made
    u8 m_attackSet;             // 0x1B5 the attack is on
    char _1b6[2];
    ykData* m_attackWork;       // 0x1B8
    snd3DGenerator m_snd;       // 0x1BC
    s32 m_seHandleA;            // 0x1C4 the sound of the avalanche
    s32 m_seHandleB;            // 0x1C8
    u8 m_seState;               // 0x1CC (0xE = nothing)
    char _1cd[3];
    float m_seTimer;            // 0x1D0

public:
    grIceYama(const char* taskName) __attribute__((never_inline));
    virtual ~grIceYama();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateJoint();
    virtual void updateYakumono(float deltaFrame);
    virtual void updateState(float deltaFrame);
    virtual void updateSE(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setGravity(float up, float down);
    virtual void setCollisionAttr(u8 material);
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork) { m_mtxGimmickWork = mtxGimmickWork; }
    virtual void setPosBearWork(Vec3f* posBearWork) { m_posBearWork = posBearWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setStateBearWork(u8* stateBearWork) { m_stateBearWork = stateBearWork; }

    static grIceYama* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grIceYama) == 0x1D4, "Class is wrong size!");
