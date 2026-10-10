#pragma once

#include <StaticAssert.h>
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
#include <types.h>
#include <yk/yakumono.h>

#include <st_metalgear/st_metalgear.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// sora_melee, unnamed: plays a sound effect of the stage, with a position on the screen (-1 left, 1 right)
extern "C" void fn_27_224DB8(int sndId, float pan);

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grMetalgearHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// The base ground of the stage. It has only the state and the timer that most of the grounds use.
class grMetalgear : public grYakumono {
protected:
    u8 m_state;                 // 0x150
    char _151[3];
    float m_timer;              // 0x154

public:
    grMetalgear(const char* taskName);
    virtual ~grMetalgear();

    static grMetalgear* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgear) == 0x158, "Class is wrong size!");

// The exclamation mark that shows up over the search light when it has found a fighter.
class grMetalgearExclamation : public grMetalgear {
public:
    Vec3f* m_posWork;           // 0x158 the place of the mark (from the stage)
    u8* m_stateWork;            // 0x15C the state of the search (from the stage)

    grMetalgearExclamation(const char* taskName);
    virtual ~grMetalgearExclamation();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);

    static grMetalgearExclamation* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgearExclamation) == 0x160, "Class is wrong size!");

// The three big enemies of the stage have most of their code in common: they appear, walk or move in front of the stage and
// leave. The stage tells which one it is (m_type: 0 = the two Gekko, 1 = Ray, 2 = Rex).
class grMetalgearMetalgear : public grMetalgear {
protected:
    Vec3f* m_posWork;           // 0x158 the places the stage gives it
    u8* m_stateWork;            // 0x15C the state of the metalgear (from the stage)
    Vec3f m_pos;                // 0x160
    Vec3f m_rot;                // 0x16C
    Vec3f m_rotNext;            // 0x178 HYPOTHESIS: where the rotation goes
    u8 m_turns;                 // 0x184 how often the rotation changes
    u8 m_type;                  // 0x185
    u8 m_number;                // 0x186 which one of the two Gekko
    u8 m_motion;                // 0x187
    float m_frame;              // 0x188
    float m_frameCount;         // 0x18C
    u8 m_motionEnd;             // 0x190 the motion is over
    u8 m_loop;                  // 0x191
    s32 m_seIndex;              // 0x194 the sequence of the sound that plays (-1 = none)

public:
    grMetalgearMetalgear(const char* taskName);
    virtual ~grMetalgearMetalgear();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setType(u8 type) { m_type = type; }
    virtual void setNumber(u8 number) { m_number = number; }
};
static_assert(sizeof(grMetalgearMetalgear) == 0x198, "Class is wrong size!");

// The two Gekko jump around in front of the stage.
class grMetalgearGekko : public grMetalgearMetalgear {
protected:
    Vec3f m_posStart;           // 0x198
    Vec3f m_posEnd;             // 0x1A4
    bool m_isWall;              // 0x1B0
    char _1b1[3];
    s32 m_seHandle;             // 0x1B4 the sound that plays while it comes
    StSeUtil::SeSeqInstance<5, 5> m_sePlayer; // 0x1B8
    SndID m_seIds[5];           // 0x264
    StSeUtil::UnkStruct m_seData[22]; // 0x278

public:
    grMetalgearGekko(const char* taskName);
    virtual ~grMetalgearGekko();
    virtual void updateActive(float deltaFrame);
    virtual void selectTgt();
    virtual bool isWall();
    virtual bool isEnableTgt();

    static grMetalgearGekko* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgearGekko) == 0x3D8, "Class is wrong size!");

// Ray swims in the back of the stage.
class grMetalgearRay : public grMetalgearMetalgear {
protected:
    StSeUtil::SeSeqInstance<2, 1> m_sePlayer; // 0x198
    SndID m_seIds[1];           // 0x1E0
    StSeUtil::UnkStruct m_seData[2]; // 0x1E4

public:
    grMetalgearRay(const char* taskName);
    virtual ~grMetalgearRay();
    virtual void updateActive(float deltaFrame);

    static grMetalgearRay* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgearRay) == 0x204, "Class is wrong size!");

// Rex stands in the back of the stage and shoots.
class grMetalgearRex : public grMetalgearMetalgear {
protected:
    StSeUtil::SeSeqInstance<5, 10> m_sePlayer; // 0x198
    SndID m_seIds[10];          // 0x294
    StSeUtil::UnkStruct m_seData[33]; // 0x2BC

public:
    grMetalgearRex(const char* taskName);
    virtual ~grMetalgearRex();
    virtual void updateActive(float deltaFrame);

    static grMetalgearRex* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgearRex) == 0x4CC, "Class is wrong size!");

// The stage model: the roof breaks and the beams of the search light are nodes of it.
class grMetalgearMainBg : public grMetalgear {
protected:
    Vec3f* m_posGimmickWork;    // 0x158 the places the stage gives it
    u8* m_stateWork;            // 0x15C
    u8 m_motion;                // 0x160
    char _161[3];
    float m_frameCount;         // 0x164
    u32 m_nodeYaneNormal;       // 0x168
    u32 m_nodeYaneCrash;        // 0x16C
    u32 m_nodeMetalgear;        // 0x170
    u32 m_nodeGekko;            // 0x174
    u32 m_nodeSLightA;          // 0x178
    u32 m_nodeSLightB;          // 0x17C
    grCollisionJoint* m_jointLeft;  // 0x180
    grCollisionJoint* m_jointRight; // 0x184
    u8* m_stateWallWork;        // 0x188

public:
    grMetalgearMainBg(const char* taskName);
    virtual ~grMetalgearMainBg();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual float getMotionFrame(u32 index);
    virtual void updateJoint(float deltaFrame);
    virtual void updateBreak(float deltaFrame);
    virtual void updateColl(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setStateWallWork(u8* stateWallWork) { m_stateWallWork = stateWallWork; }

    static grMetalgearMainBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgearMainBg) == 0x18C, "Class is wrong size!");

// One of the four walls of the stage: it can be hit, breaks, shakes and rises again.
class grMetalgearWall : public grMetalgear {
protected:
    float unk158;               // 0x158
    float unk15C;               // 0x15C
    float unk160;               // 0x160
    Vec3f m_pos;                // 0x164
    Vec3f* m_posGimmickWork;    // 0x170 the places the stage gives it (the first ones are the nodes of the wall)
    float* m_rotZWork;          // 0x174 the angle of the wall (from the stage)
    float m_life;               // 0x178 how much the wall stands
    float m_motionFrame;        // 0x17C
    float m_shakeTimer;         // 0x180
    Vec3f m_shake;              // 0x184
    u8* m_stateWork;            // 0x190
    u8 m_type;                  // 0x194 0 = left up, 1 = left down, 2 = right up, 3 = right down
    char _195[3];
    float m_collPoint[4];       // 0x198 two points of the collision line (x, y)
    u32 m_nodeEffect;           // 0x1A8
    float m_effectTimer;        // 0x1AC
    u8 m_motion;                // 0x1B0
    char _1b1[3];
    float m_frameCount;         // 0x1B4
    u8 m_hasHit;                // 0x1B8
    char _1b9[3];
    soCollisionHitData* m_hitData;           // 0x1BC the hit box
    soCollisionHitData::Simple* m_hitSimple; // 0x1C0 its form for the hit object
    soSet<soCollisionHitData>* m_hitSet;     // 0x1C4
    ykDataGroup* m_dataGroup;                // 0x1C8
    ykData* m_data;                          // 0x1CC
    snd3DGenerator m_snd;       // 0x1D0
    grCollisionJoint* m_joint;  // 0x1D8
    s32 m_vtxIndex;             // 0x1DC

public:
    grMetalgearWall(const char* taskName);
    virtual ~grMetalgearWall();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateBreak(float deltaFrame);
    virtual void updateShake(float deltaFrame);
    virtual void updateCollision(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    virtual void setRotZWork(float* rotZWork) { m_rotZWork = rotZWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setType(u8 type) { m_type = type; }

    static grMetalgearWall* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgearWall) == 0x1E0, "Class is wrong size!");

// The search light: it moves over the stage, looks for a fighter and follows it.
class grMetalgearSearch : public grMetalgear {
protected:
    Vec3f* m_posGimmickWork;    // 0x158 the places the stage gives it (the third one is the light)
    u8* m_stateWork;            // 0x15C
    u8* m_stateExclamationWork; // 0x160
    Vec3f m_move;               // 0x164 where the light moves to
    u8 m_first;                 // 0x170
    char _171[3];
    s32* m_tgtWork;             // 0x174 the fighter the light follows (from the stage)
    snd3DGenerator m_snd;       // 0x178
    s32 m_seHandle;             // 0x180 the sound that plays while the light follows a fighter

public:
    grMetalgearSearch(const char* taskName);
    virtual ~grMetalgearSearch();
    virtual void update(float deltaFrame);
    virtual void updateArea(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual bool getPlayerPosition(int playerNo, Vec3f* pos, int kind);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setStateExclamationWork(u8* stateExclamationWork) { m_stateExclamationWork = stateExclamationWork; }
    virtual void setTgtWork(s32* tgtWork) { m_tgtWork = tgtWork; }

    static grMetalgearSearch* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgearSearch) == 0x184, "Class is wrong size!");

// The part of the roof that hurts: it moves between two places of the stage while the walls stand.
class grMetalgearAttack : public grMetalgear {
protected:
    Vec3f* m_posWork;           // 0x158 the places the stage gives it
    Vec3f* m_posLimitWork;      // 0x15C the part of the stage the camera shows
    float* m_rotWork;           // 0x160 the angle of the wall (from the stage)
    Vec3f m_pos;                // 0x164
    u8* m_stateWork;            // 0x170
    u8 m_type;                  // 0x174 7 = left, 8 = right
    u8 m_hasYakumono;           // 0x175
    u8 m_attackEnabled;         // 0x176
    char _177;
    u32* m_work;                // 0x178

public:
    grMetalgearAttack(const char* taskName);
    virtual ~grMetalgearAttack();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateCollision(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setPosLimitWork(Vec3f* posLimitWork) { m_posLimitWork = posLimitWork; }
    virtual void setRotWork(float* rotWork) { m_rotWork = rotWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setType(u8 type) { m_type = type; }

    static grMetalgearAttack* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMetalgearAttack) == 0x17C, "Class is wrong size!");

// HYPOTHESIS: inline helpers of the original (a tolerance test and a clamp that the grounds of the stage share).
static inline bool metalgearIsNearZero(float value) {
    bool result = false;
    if ((float)fabs(value) < 1e-5f) {
        result = true;
    }
    return result;
}

static inline float metalgearLength(Vec3f& v) {
    float length = v.m_z * v.m_z + (v.m_x * v.m_x + v.m_y * v.m_y);
    if ((float)fabs(length) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = length * rsqrtf(length);
    }
    return length;
}
