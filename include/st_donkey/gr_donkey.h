#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick_ladder.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_vector.h>
#include <st/se_util.h>
#include <snd/snd_3d_generator.h>
#include <types.h>
#include <yk/yakumono.h>

// MATCH-ONLY: the original scales vectors with paired singles (inline asm in the shared vector code).
static inline void donkeyVec3Scale(register Vec3f* pOut, register const Vec3f* v, register float c) {
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

// 8 bytes of work memory the hit objects allocate for their hit setup (HYPOTHESIS: filled in by the module builder).
struct grDonkeyWork {
    u32 unk0;
    u32 unk4;
    grDonkeyWork() : unk0(0), unk4(0) { }
};

// 75m grounds. Like the other stages that follow the arcade game, the grounds do not share a lot: every ground is a small
// state machine on a Yakumono that the stage hands pointers to the data it shares through the virtual set...Work functions
// right after it creates them. The model scale of the stage is compensated for in grDonkey::updateCallBack.
class grDonkey : public grYakumono {
protected:
    u8 m_state;                // 0x150
    float m_timer;             // 0x154
    Vec3f m_scaleBase;         // 0x158 scale of the root node of the model (read from the model once)

public:
    grDonkey(const char* taskName) __attribute__((never_inline));
    virtual ~grDonkey() __attribute__((never_inline));
    virtual void update(float deltaFrame);
    virtual void updateScaleBase(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
};
static_assert(sizeof(grDonkey) == 0x164, "Class is wrong size!");

// The backdrop. It owns the 43 nodes whose positions the stage (and through it the other grounds) uses as spawn points,
// goal points and floors.
class grDonkeyMainBg : public grDonkey {
    u8 m_motion;               // 0x164
    Vec3f* m_posGimmickWork;   // 0x168 positions of the nodes (to the stage)
    u32 m_node[43];            // 0x16C node indices, in the order of stDonkey::m_posGimmick

public:
    grDonkeyMainBg(const char* taskName);
    virtual ~grDonkeyMainBg();
    virtual void processAnim();
    virtual bool setNode();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    static grDonkeyMainBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyMainBg) == 0x218, "Class is wrong size!");

// One of the elevators ("Ashiba"). It rides between the two ends of the stage positions it was given at a speed that is
// the stage-wide lift speed times its rate (negative = moves down).
class grDonkeyLift : public grDonkey {
    Vec3f* m_posWork;          // 0x164 the two ends of the track (from the stage)
    float m_rate;              // 0x168 fraction of the lift speed, the sign is the direction
    Vec3f m_pos;               // 0x16C current position

public:
    grDonkeyLift(const char* taskName);
    virtual ~grDonkeyLift();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setRate(float rate) { m_rate = rate; }
    static grDonkeyLift* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyLift) == 0x178, "Class is wrong size!");

// A digit of the score counter. It shows the digit of the number the stage gives it at a decimal place.
class grDonkeyScore : public grDonkey {
    Vec3f* m_posWork;          // 0x164
    int* m_scoreWork;          // 0x168 the score (or the high score) to show
    int m_lastScore;           // 0x16C the score as it was drawn the last time
    u8 m_type;                 // 0x170 decimal place of this digit (2 = 100000s ... 7 = 1s)

public:
    grDonkeyScore(const char* taskName);
    virtual ~grDonkeyScore();
    virtual void update(float deltaFrame);
    virtual void updateNumber();
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setScoreWork(int* scoreWork) { m_scoreWork = scoreWork; }
    virtual void setType(u8 type) { m_type = type; }
    static grDonkeyScore* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyScore) == 0x174, "Class is wrong size!");

// Donkey Kong. He walks along the top girder, throws a barrel (the attack of this ground is his body, the barrels are the
// fireballs) and beats his chest, turning darker the closer he is to the next throw.
class grDonkeyKong : public grDonkey {
    u8 m_firstThrow;           // 0x164 1 until the first throw
    Vec3f* m_posWork;          // 0x168 where he stands (from the stage)
    Vec3f m_offset;            // 0x16C offset of the model from that position
    u8 m_hasYakumono;          // 0x178
    u8 m_attackEnabled;        // 0x179
    grDonkeyWork* m_work;      // 0x17C the work of the Yakumono (two words)
    u8* m_stateWork;           // 0x180 state byte of Donkey Kong (3 = throwing position)
    u8* m_stateJackWork;       // 0x184 state bytes of Jack A and B
    u8 m_motion;               // 0x188
    float m_motionFrames;      // 0x18C frame count of the current motion
    snd3DGenerator m_snd;      // 0x190
    float m_seTimer;           // 0x198
    s32 m_seId;                // 0x19C
    s32 m_seIdRoar;            // 0x1A0
    s32 m_dangerZone;          // 0x1A4

public:
    grDonkeyKong(const char* taskName);
    virtual ~grDonkeyKong();
    virtual void update(float deltaFrame);
    virtual void updateColor();
    virtual void updateMove(float deltaFrame);
    virtual void updateYakumono();
    virtual void setHit();
    virtual void setAttack();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setStateJackWork(u8* stateJackWork) { m_stateJackWork = stateJackWork; }
    virtual void updateCallBack(float deltaFrame);
    static grDonkeyKong* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyKong) == 0x1A8, "Class is wrong size!");

// Jack, the arcade game's kidnapper, jumps along the lower floor. He is a hit object that attacks when his body moves
// (HYPOTHESIS: A and B are the two spawn sides).
class grDonkeyJack : public grDonkey {
    u8 m_firstSpawn;           // 0x164
    Vec3f* m_posWork;          // 0x168
    Vec3f m_offset;            // 0x16C
    u8 m_hasYakumono;          // 0x178
    u8 m_attackEnabled;        // 0x179
    u8 m_attackSide;           // 0x17A 1 = side attack, 0 = down attack
    grDonkeyWork* m_work;      // 0x17C
    u8* m_stateWork;           // 0x180
    u8 m_motion;               // 0x184
    float m_unk188;            // 0x188
    float m_motionFrames;      // 0x18C
    snd3DGenerator m_snd;      // 0x190
    s32 m_seId;                // 0x198
    s32 m_seIdLand;            // 0x19C
    StSeUtil::SeSeqInstance<1, 1> m_seSeq; // 0x1A0
    SndID m_seSeqId;           // 0x1DC
    StSeUtil::UnkStruct m_seSeqData[5]; // 0x1E0

public:
    grDonkeyJack(const char* taskName);
    virtual ~grDonkeyJack();
    virtual void update(float deltaFrame);
    virtual void updateYakumono();
    virtual void updateMove(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setAttackDown();
    virtual void setAttackSide();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void updateCallBack(float deltaFrame);
    static grDonkeyJack* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyJack) == 0x230, "Class is wrong size!");

// One of the ladders: the stage builds the data of the ladder (area, restrictions) and the ground adds the area to its
// Yakumono and a trigger that follows it.
class grDonkeyLadder : public grGimmickLadder {
    Vec3f* m_posWork;          // 0x1A0
    u8 m_triggerMade;          // 0x1A4

public:
    grDonkeyLadder(const char* taskName);
    virtual ~grDonkeyLadder();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void updateYakumono(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    static grDonkeyLadder* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyLadder) == 0x1A8, "Class is wrong size!");

// A fireball that jumps along the girders (type 0 = A, 1 = B). It is a hit object with an attack.
class grDonkeyFireBall : public grDonkey {
    Vec3f* m_posWork;          // 0x164 the five waypoints (from the stage)
    u8* m_stateWork;           // 0x168 state byte (3 = go)
    Vec3f m_pos;               // 0x16C current position
    Vec3f m_posTgt;            // 0x178 position it moves to
    Vec3f m_rot;               // 0x184 rotation
    u8 m_dir;                  // 0x190 0 = to the left, 1 = to the right
    u8 m_type;                 // 0x191
    u8 m_waypoint;             // 0x192 index of the waypoint it is at
    u8 m_hasYakumono;          // 0x193
    u8 m_attackEnabled;        // 0x194
    grDonkeyWork* m_work;      // 0x198
    s32 m_dangerZone;          // 0x19C

public:
    grDonkeyFireBall(const char* taskName);
    virtual ~grDonkeyFireBall();
    virtual void update(float deltaFrame);
    virtual void updateYakumono();
    virtual void updateActive(float deltaFrame);
    virtual void updateMoveX(float deltaFrame);
    virtual void updateMoveY(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void selectPosTgt();
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setType(u8 type) { m_type = type; }
    virtual void updateCallBack(float deltaFrame);
    static grDonkeyFireBall* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyFireBall) == 0x1A0, "Class is wrong size!");

// A bonus item of the arcade game (parasol, handbag, hat). Fighters collect it by standing on it.
class grDonkeyItem : public grDonkey {
protected:
    Vec3f* m_posWork;          // 0x164
    u8* m_stateWork;           // 0x168 state byte of the item (3 = on offer, 4 = taken)
    u8 m_type;                 // 0x16C

public:
    grDonkeyItem(const char* taskName);
    virtual ~grDonkeyItem();
    virtual void update(float deltaFrame);
    virtual void updateArea();
    virtual void updateActive(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setType(u8 type) { m_type = type; }
    virtual void updateCallBack(float deltaFrame);
    static grDonkeyItem* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyItem) == 0x170, "Class is wrong size!");

// The "800" popping up where an item was taken.
class grDonkeyItemScore : public grDonkeyItem {
    snd3DGenerator m_snd;      // 0x170

public:
    grDonkeyItemScore(const char* taskName);
    virtual ~grDonkeyItemScore();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    static grDonkeyItemScore* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDonkeyItemScore) == 0x178, "Class is wrong size!");
