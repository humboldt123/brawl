#pragma once

#include <StaticAssert.h>
#include <math.h>
#include <cm/cm_subject.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <st/se_util.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_halberd/st_halberd.h>

// The two words of data the Yakumono of the attacks is given (nothing is read from it).
struct grHalberdWork {
    u32 unk0;
    u32 unk4;
    grHalberdWork() {
        unk0 = 0;
        unk4 = 0;
    }
};

// Matrix helpers of the main binary that have no name yet.
extern "C" void fn_8003F03C(Matrix* mtx, float x, float y, float z); // HYPOTHESIS: sets the translation
extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z); // HYPOTHESIS: adds a translation
extern "C" void fn_8003EA9C(Matrix* mtx, float angle);               // HYPOTHESIS: rotation around X (radians)
extern "C" void fn_8003EBF4(Matrix* mtx, float angle);               // HYPOTHESIS: rotation around Z (radians)
extern "C" void fn_8003E828(Matrix* mtx, Vec3f* scale, Matrix* out); // HYPOTHESIS: scales the matrix

// The length of a vector, written out like the original does (the operands are given in the order of the sum).
static inline float grHalberdLength(float a, float b, float c) {
    float lengthSq = a * a + b * b + c * c;
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

// MATCH-ONLY: the original flips three bits of the joint's flag byte as a chain (HYPOTHESIS: they track "enabled").
static inline void grHalberdJointEnable(grCollisionJoint* joint) {
    joint->m_0x54_4 = true;
    joint->m_0x54_6 = joint->m_0x54_4;
}

static inline void grHalberdJointDisable(grCollisionJoint* joint) {
    joint->m_0x54_7 = false;
    joint->m_0x54_4 = joint->m_0x54_7;
    joint->m_0x54_6 = joint->m_0x54_4;
}

// The ground all the grounds of the stage are made from: it holds the state of the ground and pointers to the frame and the
// state of the story, both published by the stage.
class grHalberd : public grYakumono {
protected:
    u8 m_state;                // 0x150
    char _151[3];
    float m_timer;             // 0x154
    float* m_frameWork;        // 0x158 the frame of the stage animation (from the stage)
    u8* m_stateWork;           // 0x15C the part of the story (from the stage)

public:
    grHalberd(const char* taskName);
    virtual ~grHalberd();
    virtual void setFrameWork(float* frameWork) { m_frameWork = frameWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
};
static_assert(sizeof(grHalberd) == 0x160, "Class is wrong size!");

// The hull of the ship: the ground that gives the stage the positions of the dome, the warning and the rear gun and moves the
// collision of the two decks (the second one opens while the ship flies).
class grHalberdBg : public grHalberd {
    u8 m_effect;                   // 0x160 1 once the effect of the clouds was made
    char _161[3];
    Matrix* m_mtxGimmickWork;      // 0x164 the matrices of the nodes the other grounds use (to the stage)
    Vec3f* m_posTrainerWork;       // 0x168 the positions of the trainer (to the stage)
    u8 m_motion;                   // 0x16C
    char _16d[3];
    float m_motionTimer;           // 0x170 the frames the motion has left
    u32 m_nodeTrainer[4];          // 0x174 node indices of "ptPosition01HB" .. "ptPosition04HB"
    grCollisionJoint* m_joint[2];  // 0x184 the two joints of the deck
    u32 m_node[4];                 // 0x18C node indices (hull, dome, warning, rear gun)
    StSeUtil::SeSeqInstance<2, 11> m_seSeq; // 0x19C
    SndID m_seId[11];              // 0x284
    StSeUtil::UnkStruct m_seData0[16]; // 0x2B0
    StSeUtil::UnkStruct m_seData1[3];  // 0x3B0
    snd3DGenerator m_snd;          // 0x3E0

public:
    grHalberdBg(const char* taskName);
    virtual ~grHalberdBg();
    virtual void processAnim();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual void updateCollision(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork) { m_mtxGimmickWork = mtxGimmickWork; }
    virtual void setPosTrainerWork(Vec3f* posTrainerWork) { m_posTrainerWork = posTrainerWork; }
    static grHalberdBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdBg) == 0x3E8, "Class is wrong size!");

// The far backdrop (the sea of clouds).
class grHalberdEnkei : public grHalberd {
    Matrix* m_mtxWork;             // 0x160 the matrix of the backdrop (from the stage animation)
    u8 m_hasSrt;                   // 0x164 1 = the texture animation is not played
    u8 m_motion;                   // 0x165
    char _166[2];
    float m_motionTimer;           // 0x168

public:
    grHalberdEnkei(const char* taskName);
    virtual ~grHalberdEnkei();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setMotionFrame(float frame, u32 animIndex);
    virtual float getMotionFrame(u32 animIndex);
    static grHalberdEnkei* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdEnkei) == 0x16C, "Class is wrong size!");

// The dome of the ship that opens at the start of the story and closes again.
class grHalberdDome : public grHalberd {
    Matrix* m_mtxWork;             // 0x160 the matrix of the dome (from the stage animation)
    Matrix* m_mtxGimmickWork;      // 0x164 the matrices of the nodes (to the stage)
    Vec3f* m_posTrainerWork;       // 0x168 the positions of the trainer (to the stage)
    u8 m_motion;                   // 0x16C
    char _16d[3];
    float m_motionTimer;           // 0x170
    grCollisionJoint* m_joint[2];  // 0x174 the two joints of the floor of the dome
    u32 m_node[4];                 // 0x17C node indices of "ptPosition01Dome" .. "ptPosition04Dome"
    StSeUtil::SeSeqInstance<1, 3> m_seSeq; // 0x18C
    SndID m_seId[3];               // 0x1E8
    StSeUtil::UnkStruct m_seData[3]; // 0x1F4
    snd3DGenerator m_snd;          // 0x224

public:
    grHalberdDome(const char* taskName);
    virtual ~grHalberdDome();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual void updateCollision(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork) { m_mtxGimmickWork = mtxGimmickWork; }
    virtual void setPosTrainerWork(Vec3f* posTrainerWork) { m_posTrainerWork = posTrainerWork; }
    static grHalberdDome* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdDome) == 0x22C, "Class is wrong size!");

// The deck of the ship (the fighters walk on it).
class grHalberdStage : public grHalberd {
    u8 m_effect;                   // 0x160
    u8 m_motion;                   // 0x161
    char _162[2];
    float m_motionTimer;           // 0x164
    grCollisionJoint* m_joint;     // 0x168 the joint of the deck
    StSeUtil::SeSeqInstance<2, 4> m_seSeq; // 0x16C
    SndID m_seId[4];               // 0x1E4
    StSeUtil::UnkStruct m_seData0[10]; // 0x1F4
    StSeUtil::UnkStruct m_seData1[5];  // 0x294
    snd3DGenerator m_snd;          // 0x2E4

public:
    grHalberdStage(const char* taskName);
    virtual ~grHalberdStage();
    virtual void update(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    static grHalberdStage* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdStage) == 0x2EC, "Class is wrong size!");

// One of the two cannons on the deck: it aims at a fighter, charges and fires.
class grHalberdCannon : public grHalberd {
    float m_waitTimer;             // 0x160
    Matrix* m_mtxWork;             // 0x164 the matrix of the cannon (from the stage animation)
    Matrix* m_mtxGimmickWork;      // 0x168 the matrices of the nodes (to the stage)
    Vec3f* m_posTgtWork;           // 0x16C where the cannon aims (from the stage)
    int* m_tgtWork;                // 0x170 the number of the player the cannon aims at (from the stage)
    float m_speed;                 // 0x174 how fast the aim moves
    Vec3f m_rotStart;              // 0x178 the angles of the barrel the move starts from
    Vec3f m_rotGoal;               // 0x184 the angles of the barrel it moves to
    Vec3f m_rot;                   // 0x190 the angles of the barrel now
    u8 m_motion;                   // 0x19C
    char _19d[3];
    float m_motionTimer;           // 0x1A0
    StSeUtil::SeSeqInstance<2, 2> m_seSeq; // 0x1A4
    SndID m_seId[2];               // 0x1FC
    StSeUtil::UnkStruct m_seData0[2];  // 0x204
    StSeUtil::UnkStruct m_seData1[2];  // 0x224
    snd3DGenerator m_snd;          // 0x244
    s32 m_seIdEngine;              // 0x24C
    s32 m_seIdCharge;              // 0x250

public:
    grHalberdCannon(const char* taskName);
    virtual ~grHalberdCannon();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateRot();
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork) { m_mtxGimmickWork = mtxGimmickWork; }
    virtual void setPosTgtWork(Vec3f* posTgtWork) { m_posTgtWork = posTgtWork; }
    virtual void setTgtWork(int* tgtWork) { m_tgtWork = tgtWork; }
    virtual bool getPlayerPosition(int playerNo, Vec3f* pos);
    static grHalberdCannon* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdCannon) == 0x254, "Class is wrong size!");

// The laser of the ship that the stage fires at the fighters.
class grHalberdLaser : public grHalberd {
    Matrix* m_mtxWork;             // 0x160
    Vec3f* m_posTgtWork;           // 0x164
    u8 m_motion;                   // 0x168
    char _169[3];
    float m_timer2;                // 0x16C the frames the motion has left
    float m_length;                // 0x170 how long the whole motion is (set when the laser fires)
    u8 m_yakumonoMade;             // 0x174
    u8 m_attackOn;                 // 0x175
    char _176[2];
    grHalberdWork* m_work;         // 0x178 the (empty) data of the Yakumono
    int m_dangerZone;              // 0x17C the danger zone of the laser for the AI (-1 = none)

public:
    grHalberdLaser(const char* taskName);
    virtual ~grHalberdLaser();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setAttackFinal();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setPosTgtWork(Vec3f* posTgtWork) { m_posTgtWork = posTgtWork; }
    static grHalberdLaser* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdLaser) == 0x180, "Class is wrong size!");

// The target the fighters can break.
class grHalberdTarget : public grHalberd {
    Vec3f* m_posWork;              // 0x160
    u8 m_motion;                   // 0x164
    char _165[3];
    float m_motionTimer;           // 0x168
    snd3DGenerator m_snd;          // 0x16C
    s32 m_seId;                    // 0x174
    int m_dangerZone;              // 0x178 the danger zone of the target for the AI (-1 = none)

public:
    grHalberdTarget(const char* taskName);
    virtual ~grHalberdTarget();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    static grHalberdTarget* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdTarget) == 0x17C, "Class is wrong size!");

// The rear gun (the barrel).
class grHalberdHero2Hou : public grHalberd {
    Matrix* m_mtxWork;             // 0x160
    Matrix* m_mtxGimmickWork;      // 0x164
    Vec3f* m_posTgtWork;           // 0x168
    Vec3f m_unk16c;                // 0x16C
    Vec3f m_rotStart;              // 0x178 the angles of the barrel the move starts from
    Vec3f m_rotGoal;               // 0x184 the angles of the barrel it moves to
    Vec3f m_rot;                   // 0x190 the angles of the barrel now
    u8 m_soundPhase;               // 0x19C
    u8 m_motion;                   // 0x19D
    char _19e[2];
    float m_lastFrame;             // 0x1A0
    float m_motionTimer;           // 0x1A4
    snd3DGenerator m_snd;          // 0x1A8

public:
    grHalberdHero2Hou(const char* taskName);
    virtual ~grHalberdHero2Hou();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateRot();
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork) { m_mtxGimmickWork = mtxGimmickWork; }
    virtual void setPosTgtWork(Vec3f* posTgtWork) { m_posTgtWork = posTgtWork; }
    static grHalberdHero2Hou* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdHero2Hou) == 0x1B0, "Class is wrong size!");

// The shell of the rear gun.
class grHalberdHero2Dan : public grHalberd {
    Matrix* m_mtxWork;             // 0x160
    stHalberdMtx m_mtx0;           // 0x164
    stHalberdMtx m_mtx1;           // 0x194
    Vec3f m_pos;                   // 0x1C4
    Vec3f* m_posTgtWork;           // 0x1D0
    u8 m_motion;                   // 0x1D4
    char _1d5[3];
    float m_flightTimer;           // 0x1D8
    u8 m_yakumonoMade;             // 0x1DC
    u8 m_unk1dd;                   // 0x1DD
    char _1de[2];
    grHalberdWork* m_work;         // 0x1E0 the (empty) data of the Yakumono
    cmSubject m_subject;           // 0x1E4
    snd3DGenerator m_snd;          // 0x268
    s32 m_seId;                    // 0x270
    int m_dangerZone;              // 0x274 the danger zone of the shell for the AI (-1 = none)

public:
    grHalberdHero2Dan(const char* taskName);
    virtual ~grHalberdHero2Dan();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack(int index);
    virtual void bomb();
    virtual void onInflict(soCollisionLog* collisionLog, u32 unk2, float power);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setPosTgtWork(Vec3f* posTgtWork) { m_posTgtWork = posTgtWork; }
    static grHalberdHero2Dan* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdHero2Dan) == 0x278, "Class is wrong size!");

// One piece of the arm that grabs the fighters (bone, joint or hand).
class grHalberdArm : public grHalberd {
    Matrix* m_mtxWork;             // 0x160
    u8* m_stateAttackWork;         // 0x164
    u8 m_type;                     // 0x168 0 = bone, 1 = joint, 2 = hand
    u8 m_motion;                   // 0x169
    char _16a[2];
    float m_unk16c;                // 0x16C
    float m_motionTimer;           // 0x170
    u8 m_yakumonoMade;             // 0x174
    u8 m_attackOn;                 // 0x175
    char _176[2];
    grHalberdWork* m_work;         // 0x178 the (empty) data of the Yakumono
    snd3DGenerator m_snd;          // 0x17C

public:
    grHalberdArm(const char* taskName);
    virtual ~grHalberdArm();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setStateAttackWork(u8* stateAttackWork) { m_stateAttackWork = stateAttackWork; }
    virtual void setType(u8 type) { m_type = type; }
    static grHalberdArm* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdArm) == 0x184, "Class is wrong size!");

// The warning of the ship (the siren and the lamps).
class grHalberdWarning : public grHalberd {
    Matrix* m_mtxWork;             // 0x160
    u8 m_motion;                   // 0x164
    char _165[3];
    float m_lastFrame;             // 0x168
    float m_motionTimer;           // 0x16C
    StSeUtil::SeSeqInstance<2, 2> m_seSeq; // 0x170
    SndID m_seId[2];               // 0x1C8
    StSeUtil::UnkStruct m_seData0[1];  // 0x1D0
    StSeUtil::UnkStruct m_seData1[1];  // 0x1E0
    snd3DGenerator m_snd;          // 0x1F0

public:
    grHalberdWarning(const char* taskName);
    virtual ~grHalberdWarning();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    static grHalberdWarning* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHalberdWarning) == 0x1F8, "Class is wrong size!");
