#pragma once

#include <StaticAssert.h>
#include <gr/gr_calc_world_callback.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <mt/mt_vector.h>
#include <types.h>

class grCollisionJoint;
class grCollStatus;

// The shared vector code tests every component against a small epsilon before it takes the length.
static inline bool homerunIsSmall(float value) {
    bool small = false;
    if ((float)fabs(value) < 0.00001f) {
        small = true;
    }
    return small;
}

static inline bool homerunIsZero(const Vec3f& v) {
    bool zero = false;
    if (homerunIsSmall(v.m_x) && homerunIsSmall(v.m_y) && homerunIsSmall(v.m_z)) {
        zero = true;
    }
    return zero;
}

// Base of the grounds of the Home-Run Contest. They are Yakumono grounds that read their positions and flags through
// pointers into the stage object (the stage hands them over with the virtual set...Work functions).
class grHomerun : public grYakumono {
protected:
    u8 m_state;    // 0x150: 0 start, 1 first update, 2 running, 8 waiting to be shown
    float m_timer; // 0x154

public:
    grHomerun(const char* taskName);
    virtual ~grHomerun();
};
static_assert(sizeof(grHomerun) == 0x158, "Class is wrong size!");

// The backdrop: the stadium model with the barrier the sandbag is hit against. It owns one collision joint (the barrier
// wall) and publishes the camera scene frame.
class grHomerunBg : public grHomerun {
    float* m_posLimitWork;       // 0x158
    Vec3f m_pos;                 // 0x15C: translation of the first callback node (HYPOTHESIS: never written)
    u8* m_stateBarrierWork;      // 0x168
    float* m_hpBarrierWork;      // 0x16C
    u32* m_taskIdSandBagWork;    // 0x170
    u32* m_taskIdHomerunBatWork; // 0x174
    float* m_speedSandBagWork;   // 0x178
    u8* m_landingSandBagWork;    // 0x17C
    grCollisionJoint* m_joint;   // 0x180
    float* m_frameSceneWork;     // 0x184
    u32 m_node[3];               // 0x188
    float m_lastHp;              // 0x194

public:
    grHomerunBg(const char* taskName);
    virtual ~grHomerunBg();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void receiveCollMsg_Wall(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual bool setNode();
    virtual void setMotionFrame(float frame, u32 animIndex);
    virtual void updateJoint(float deltaFrame);
    virtual void updateScroll(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosLimitWork(float* work) { m_posLimitWork = work; }
    virtual void setStateBarrierWork(u8* work) { m_stateBarrierWork = work; }
    virtual void setHPBarrierWork(float* work) { m_hpBarrierWork = work; }
    virtual void setTaskIDSandBagWork(u32* work) { m_taskIdSandBagWork = work; }
    virtual void setTaskIDHomerunBatWork(u32* work) { m_taskIdHomerunBatWork = work; }
    virtual void setSpeedSandBagWork(float* work) { m_speedSandBagWork = work; }
    virtual void setLandingSandBagWork(u8* work) { m_landingSandBagWork = work; }
    virtual void setFrameSceneWork(float* work) { m_frameSceneWork = work; }

    static grHomerunBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHomerunBg) == 0x198, "Class is wrong size!");

// The scrolling pieces of the background (ground strips, sky, stadium, floor). Every piece keeps pointers to arrays of
// positions that the stage owns, and moves them with the scroll speed of the sandbag.
class grHomerunLoop : public grHomerun {
protected:
    Vec3f* m_posGroundWork;  // 0x158
    Vec3f* m_posFloorWork;   // 0x15C
    Vec3f* m_posSkyWork;     // 0x160
    Vec3f* m_posScoreWork;   // 0x164
    float* m_posLimitWork;   // 0x168
    u32* m_scoreWork;        // 0x16C
    float* m_scrollWork;     // 0x170
    float* m_scrollRateWork; // 0x174
    u8 m_type;               // 0x178
    char _179[3];
    float* m_frameSceneWork; // 0x17C

public:
    grHomerunLoop(const char* taskName);
    virtual ~grHomerunLoop();
    virtual void processFixPosition();
    virtual void fixpos();
    virtual void setPosGroundWork(Vec3f* work) { m_posGroundWork = work; }
    virtual void setPosFloorWork(Vec3f* work) { m_posFloorWork = work; }
    virtual void setPosSkyWork(Vec3f* work) { m_posSkyWork = work; }
    virtual void setPosScoreWork(Vec3f* work) { m_posScoreWork = work; }
    virtual void setPosLimitWork(float* work) { m_posLimitWork = work; }
    virtual void setScoreWork(u32* work) { m_scoreWork = work; }
    virtual void setScrollWork(float* work) { m_scrollWork = work; }
    virtual void setScrollRateWork(float* work) { m_scrollRateWork = work; }
    virtual void setType(u8 type) { m_type = type; }
    virtual void setFrameSceneWork(float* work) { m_frameSceneWork = work; }
};
static_assert(sizeof(grHomerunLoop) == 0x180, "Class is wrong size!");

// The stadium stands (six pieces).
class grHomerunLoopStadium : public grHomerunLoop {
    u8 m_index;      // 0x180: the piece that is moved to the front next
    char _181[3];
    Vec3f m_pos[6];  // 0x184
    u32 m_node[6];   // 0x1CC

public:
    grHomerunLoopStadium(const char* taskName);
    virtual ~grHomerunLoopStadium();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void fixpos();
    virtual void updateScroll(float deltaFrame);
    virtual void fixposScroll();
    virtual void fixposCallBack();

    static grHomerunLoopStadium* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHomerunLoopStadium) == 0x1E4, "Class is wrong size!");

// The ground strips (twelve pieces).
class grHomerunLoopGround : public grHomerunLoop {
    u8 m_index;      // 0x180
    char _181[3];
    u32 m_node[12];  // 0x184

public:
    grHomerunLoopGround(const char* taskName);
    virtual ~grHomerunLoopGround();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void fixpos();
    virtual void updateScroll(float deltaFrame);
    virtual void fixposScroll();
    virtual void fixposCallBack();

    static grHomerunLoopGround* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHomerunLoopGround) == 0x1B4, "Class is wrong size!");

// The sky: one scrolling piece per type (2 to 5).
class grHomerunLoopSky : public grHomerunLoop {
    Vec3f m_pos;     // 0x180

public:
    grHomerunLoopSky(const char* taskName);
    virtual ~grHomerunLoopSky();
    virtual void update(float deltaFrame);
    virtual void fixpos();
    virtual void updateScroll(float deltaFrame);
    virtual void fixposScroll();
    virtual void fixposCallBack();

    static grHomerunLoopSky* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHomerunLoopSky) == 0x18C, "Class is wrong size!");

// One floor piece (yellow, red or white; twelve of them are laid out in a row).
class grHomerunLoopFloor : public grHomerunLoop {
    u8 m_index;      // 0x180
    char _181[3];

public:
    grHomerunLoopFloor(const char* taskName);
    virtual ~grHomerunLoopFloor();
    virtual void update(float deltaFrame);
    virtual void fixpos();
    virtual void updateScroll(float deltaFrame);
    virtual void fixposScroll();
    virtual void fixposCallBack();
    virtual void setIndex(u8 index) { m_index = index; }

    static grHomerunLoopFloor* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHomerunLoopFloor) == 0x184, "Class is wrong size!");

// The score boards (A to F) that show the distance of the throw.
class grHomerunScore : public grHomerun {
    Vec3f* m_posWork;        // 0x158
    Vec3f* m_posNumberWork;  // 0x15C
    Vec3f* m_posZeroWork;    // 0x160
    float* m_posLimitWork;   // 0x164
    u32* m_scoreWork;        // 0x168
    float* m_scoreSandBagWork; // 0x16C
    float* m_scrollWork;     // 0x170
    float* m_scrollRateWork; // 0x174
    u32 m_node[4];           // 0x178

public:
    grHomerunScore(const char* taskName);
    virtual ~grHomerunScore();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateScore(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* work) { m_posWork = work; }
    virtual void setPosNumberWork(Vec3f* work) { m_posNumberWork = work; }
    virtual void setPosZeroWork(Vec3f* work) { m_posZeroWork = work; }
    virtual void setPosLimitWork(float* work) { m_posLimitWork = work; }
    virtual void setScoreWork(u32* work) { m_scoreWork = work; }
    virtual void setScoreSandBagWork(float* work) { m_scoreSandBagWork = work; }
    virtual void setScrollWork(float* work) { m_scrollWork = work; }
    virtual void setScrollRateWork(float* work) { m_scrollRateWork = work; }

    static grHomerunScore* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHomerunScore) == 0x188, "Class is wrong size!");

// One digit of a score board.
class grHomerunNumber : public grHomerun {
    Vec3f* m_posWork;   // 0x158
    u32* m_scoreWork;   // 0x15C
    u32 m_lastScore;    // 0x160
    u8 m_type;          // 0x164: 0x12 shows the ones, 0x13 the tens, 0x14 the hundreds, 0x15 the thousands
    char _165[3];
    u32 m_node[10];     // 0x168

public:
    grHomerunNumber(const char* taskName);
    virtual ~grHomerunNumber();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateNumber(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void show(u8 index);
    virtual void hide(u8 index);
    virtual void hideAll();
    virtual void setPosWork(Vec3f* work) { m_posWork = work; }
    virtual void setScoreWork(u32* work) { m_scoreWork = work; }
    virtual void setType(u8 type) { m_type = type; }

    static grHomerunNumber* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHomerunNumber) == 0x190, "Class is wrong size!");

// The barrier in front of the sandbag.
class grHomerunBarrier : public grHomerun {
    u8* m_stateWork;    // 0x158
    float* m_hpWork;    // 0x15C
    u8 m_barrierState;  // 0x160
    u8 m_nextState;     // 0x161
    char _162[2];
    float m_pos[3];     // 0x164
    u8 m_motion;        // 0x170
    char _171[3];
    float m_motionTimer; // 0x174

public:
    grHomerunBarrier(const char* taskName);
    virtual ~grHomerunBarrier();
    virtual void update(float deltaFrame);
    virtual void updateState(float deltaFrame);
    virtual void updateScroll(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 index, bool loop, bool force, float* frameCount);
    virtual void setStateWork(u8* work) { m_stateWork = work; }
    virtual void setHPWork(float* work) { m_hpWork = work; }

    static grHomerunBarrier* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grHomerunBarrier) == 0x178, "Class is wrong size!");

