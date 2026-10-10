#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <snd/snd_3d_generator.h>
#include <snd/snd_system.h>
#include <st/se_util.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_kart/st_kart.h>

class grCollisionJoint;

// The length of a vector, written out like the original does (the operands are given in the order of the sum).
static inline float grKartLength(float a, float b, float c) {
    float lengthSq = a * a + b * b + c * c;
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

// The same for a vector (the original computes the sum as z * z + (x * x + y * y)).
static inline float grKartVecLength(const Vec3f& v) {
    float lengthSq = v.m_z * v.m_z + (v.m_x * v.m_x + v.m_y * v.m_y);
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

// Matrix helper of the main binary that has no name yet.
extern "C" void fn_8003F074(Matrix* mtx, float x, float y, float z); // HYPOTHESIS: adds a translation
// The setter for the pitch of a sound (sndSystem, no name yet).
extern "C" void fn_800778CC(sndSystem* system, s32 handleId, float pitch);

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grKartHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

// MATCH-ONLY: layout of soSet<T> (its members are private)
struct grKartSetView {
    void* m_elements;
    u32 m_size;
};

// The base ground of the stage.
class grKart : public grYakumono {
protected:
    u8 m_state;   // 0x150
    char _151[3];
    float m_timer; // 0x154

public:
    grKart(const char* taskName);
    virtual ~grKart();
    static grKart* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grKart) == 0x158, "Class is wrong size!");

// The road: the three parts of the road (the middle and the two sides) that the stage opens and closes.
class grKartBg : public grKart {
    u8* m_stateAIWork;            // 0x158 what the road does (from the stage)
    grCollisionJoint* m_joint[4]; // 0x15C the joints of the road ("AshibaCN", "AshibaLN", "AshibaRN", "lord")

public:
    grKartBg(const char* taskName);
    virtual ~grKartBg();
    virtual void update(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual void setStateAIWork(u8* stateAIWork) { m_stateAIWork = stateAIWork; }
    static grKartBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grKartBg) == 0x16C, "Class is wrong size!");

// Puts a model (a kart) to its place: the position and the rotation of the kart are applied to the world matrix of the
// model's node.
class grKartKartScnObjCallBack : public nw4r::g3d::IScnObjCallback {
    Vec3f m_rot; // 0x04
    Vec3f m_pos; // 0x10

public:
    grKartKartScnObjCallBack() {
        m_rot.m_x = 0.0f;
        m_rot.m_y = 0.0f;
        m_rot.m_z = 0.0f;
        m_pos.m_x = 0.0f;
        m_pos.m_y = 0.0f;
        m_pos.m_z = 0.0f;
    }
    virtual ~grKartKartScnObjCallBack() { }
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* object, u32 param, void* info);
    virtual void SetCallBackCondition(nw4r::g3d::ScnObj* object);
    virtual void setPos(float x, float y, float z);
    virtual void setRot(float x, float y, float z);
};
static_assert(sizeof(grKartKartScnObjCallBack) == 0x1C, "Class is wrong size!");

// A kart. It drives along the paths of the stage (four bezier curves: three lanes and one for the height), changes lanes by
// itself, speeds up when it falls behind and is thrown away when it is hit.
class grKartKart : public grKart {
    grKartKartScnObjCallBack m_scnObjCallback; // 0x158
    Vec3f unk174;                              // 0x174
    int unk180;                                // 0x180
    grFixedPathCollection* m_path;             // 0x184 the paths (from the stage)
    u8 m_team;                                 // 0x188 the number of the kart
    char _189[3];
    Vec3f m_ctrl[4][4];                        // 0x18C control points of the curves of the lanes (1, 2, 3 and the side one)
    float m_speed;                             // 0x24C
    Vec3f* m_limit;                            // 0x250 the area of the camera (from the stage)
    stKartState* m_kart;                       // 0x254 the state of all karts (from the stage)
    float m_curveRatio;                        // 0x258 how much longer the third lane is than the first one
    float m_aiTimer;                           // 0x25C
    float m_aiSideDist;                        // 0x260
    float m_aiSideRate;                        // 0x264
    u8 m_aiState;                              // 0x268 where the kart wants to be (0 = middle, 1 = left, 2 = right)
    char _269[3];
    snd3DGenerator m_snd;                      // 0x26C
    int m_seIdMain;                            // 0x274
    int m_seIdJump;                            // 0x278
    int m_seIdPass;                            // 0x27C
    int m_seIdDoppler;                         // 0x280

public:
    grKartKart(const char* taskName);
    virtual ~grKartKart();
    virtual void update(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateMoveSpeed(float deltaFrame);
    virtual void updateMoveSide(float deltaFrame);
    virtual void updateAI(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateSE(float deltaFrame);
    virtual bool setCtrlPosSideRate(float rate, u32 pathA, u32 pathB, u32 point, Vec3f* out);
    virtual bool setCtrlPos(u32 pathNo, u32 point, Vec3f* out);
    virtual u32 getCtrlDistance(float distance, u32 pathNo, u32 point);
    virtual void selectJumpPath();
    virtual void setPathHeader(grFixedPathCollection* path) { m_path = path; }
    virtual grFixedPathCollection* getPathHeader();
    virtual void setTeam(u8 team) { m_team = team; }
    virtual void setPosLimitWork(Vec3f* limit) { m_limit = limit; }
    virtual void setKartData(stKartState* kart) { m_kart = kart; }
    static grKartKart* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grKartKart) == 0x284, "Class is wrong size!");

// The hit area of a kart (the kart hits the fighters and the fighters can hit the kart).
class grKartAttack : public grKart {
    stKartState* m_kart;          // 0x158 the state of the kart (from the stage)
    u8 m_hasYakumono;             // 0x15C
    u8 m_attackEnabled;           // 0x15D
    char _15e[2];
    soCollisionHitData* m_hitData;               // 0x160
    soCollisionHitData::Simple* m_hitSimple;     // 0x164
    soSet<soCollisionHitData::Simple>* m_hitSet; // 0x168
    ykDataGroup* m_dataGroup;                    // 0x16C
    ykData* m_data;                              // 0x170

public:
    grKartAttack(const char* taskName);
    virtual ~grKartAttack();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setAttackUpper();
    virtual void setAttackSide();
    virtual void setKartData(stKartState* kart) { m_kart = kart; }
    static grKartAttack* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grKartAttack) == 0x174, "Class is wrong size!");

// The icon over a kart (it shows where the kart is on the map).
class grKartIcon : public grKart {
    Vec3f m_pos;                    // 0x158
    stKartState* m_kart;            // 0x164
    grFixedPathCollection* m_path;  // 0x168

public:
    grKartIcon(const char* taskName);
    virtual ~grKartIcon();
    virtual void update(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual bool setCtrlPos(u32 pathNo, u32 point, Vec3f* out);
    virtual void setPathHeader(grFixedPathCollection* path) { m_path = path; }
    virtual grFixedPathCollection* getPathHeader();
    virtual void setKartData(stKartState* kart) { m_kart = kart; }
    static grKartIcon* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grKartIcon) == 0x16C, "Class is wrong size!");

// The warning that shows up when the road opens a side.
class grKartWarning : public grKart {
    Vec3f m_pos;                             // 0x158
    u8* m_stateWork;                         // 0x164 what the warning shows (from the stage)
    u8 m_motion;                             // 0x168
    char _169[3];
    float m_motionTimer;                     // 0x16C
    StSeUtil::SeSeqInstance<1, 1> m_seSeq;   // 0x170
    SndID m_seId[1];                         // 0x1AC
    StSeUtil::UnkStruct m_seData[1];         // 0x1B0

public:
    grKartWarning(const char* taskName);
    virtual ~grKartWarning();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    static grKartWarning* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grKartWarning) == 0x1C0, "Class is wrong size!");
