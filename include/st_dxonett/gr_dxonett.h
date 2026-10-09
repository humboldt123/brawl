#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <st/se_util.h>
#include <types.h>

// Onett (DX "Onett") grounds. They are Yakumono grounds that carry a state byte and a timer; the stage hands them the
// pointers to the data they share (car state, camera limits, positions of the cars) through the virtual set...Work
// functions right after it creates them.
class grDxOnett : public grYakumono {
protected:
    u8 m_state;                // 0x150
    float m_timer;             // 0x154
    Vec3f* m_posGimmickWork;   // 0x158 positions the stage keeps (the nodes of the road the cars drive on)

public:
    grDxOnett(const char* taskName);
    virtual ~grDxOnett();
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    static grDxOnett* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxOnett) == 0x15C, "Class is wrong size!");

// The backdrop. It publishes the positions of the four car nodes ("CarLeftPositionA/B", "CarRightPositionA/B") and of
// the smoke generator ("kemuriGen") to the stage, and puffs smoke now and then.
class grDxOnettBg : public grDxOnett {
    u32 m_nodeCarLeftA;        // 0x15C "CarLeftPositionA"
    u32 m_nodeCarRightA;       // 0x160 "CarRightPositionA"
    u32 m_nodeCarLeftB;        // 0x164 "CarLeftPositionB"
    u32 m_nodeCarRightB;       // 0x168 "CarRightPositionB"
    u32 m_nodeSmoke;           // 0x16C "kemuriGen"
    u8* m_stateWork;           // 0x170 byte of the stage that tells whether a fighter is near the smoke
    u32 m_effectHandle;        // 0x174
    float m_nearTimer;         // 0x178

public:
    grDxOnettBg(const char* taskName) : grDxOnett(taskName) {
        m_nodeCarLeftA = 0;
        m_nodeCarRightA = 0;
        m_nodeCarLeftB = 0;
        m_nodeCarRightB = 0;
        m_nodeSmoke = 0;
        m_stateWork = NULL;
        m_effectHandle = 0;
        m_nearTimer = 0.0f;
    }
    virtual ~grDxOnettBg();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateArea(float deltaFrame);
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    static grDxOnettBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxOnettBg) == 0x17C, "Class is wrong size!");

// The sign that warns about the cars ("CautionR"): it follows the camera while the stage announces a car.
class grDxOnettWarning : public grDxOnett {
    u8* m_stateWork;           // 0x15C byte of the stage: 1 = the warning is wanted, 2 = it is over
    u8 m_animId;               // 0x160 current animation (1 = none)
    float m_animFrames;        // 0x164 frame count of the current animation

public:
    grDxOnettWarning(const char* taskName);
    virtual ~grDxOnettWarning();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    static grDxOnettWarning* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxOnettWarning) == 0x168, "Class is wrong size!");

// The trees by the road. A landing fighter bends them (springs them down and back); type 0/1 is tree A/B.
class grDxOnettAshiba : public grDxOnett {
    u8 m_type;                 // 0x15C
    float m_offsetX;           // 0x160 offset of the node the platform follows
    float m_offsetY;           // 0x164
    float m_offsetZ;           // 0x168
    float m_speed;             // 0x16C speed of the spring
    u8 m_landed;               // 0x170 a fighter just landed hard enough to start the spring
    float m_push;              // 0x174 pull on the spring from the fighters standing on it
    float m_target;            // 0x178 where the spring wants to be
    u8 m_landCount;            // 0x17C fighters that landed this frame
    u8 m_landCountPrev;        // 0x17D fighters that landed last frame

public:
    grDxOnettAshiba(const char* taskName);
    virtual ~grDxOnettAshiba();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision);
    virtual void updateLanding(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setType(u8 type) { m_type = type; }
    virtual void setPosOffset(float x, float y, float z) {
        m_offsetX = x;
        m_offsetY = y;
        m_offsetZ = z;
    }
    static grDxOnettAshiba* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxOnettAshiba) == 0x180, "Class is wrong size!");

// One of the four cars (blue car, pink car, taxi, bus) that drive along the road in front of the stage.
class grDxOnettCar : public grDxOnett {
    float m_moveSpeed;         // 0x15C
    u8 m_flipped;              // 0x160 the car spins (it was hit by a fighter)
    Vec3f* m_posLimitWork;     // 0x164 camera limits the stage keeps
    u8* m_curWork;             // 0x168 which car is on the road now
    u8* m_revWork;             // 0x16C which car is waiting
    u8* m_stateWork;           // 0x170
    u8* m_stateAttackWork;     // 0x174
    u8 m_type;                 // 0x178 which kind of car it is (4 = none)
    snd3DGenerator m_snd;      // 0x17C
    u8 m_soundA;               // 0x184
    u8 m_soundB;               // 0x185

public:
    grDxOnettCar(const char* taskName);
    virtual ~grDxOnettCar();
    virtual void update(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void setPosLimitWork(Vec3f* posLimitWork) { m_posLimitWork = posLimitWork; }
    virtual void setCurWork(u8* curWork) { m_curWork = curWork; }
    virtual void setRevWork(u8* revWork) { m_revWork = revWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setStateAttackWork(u8* stateAttackWork) { m_stateAttackWork = stateAttackWork; }
    virtual void setType(u8 type) { m_type = type; }
    static grDxOnettCar* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxOnettCar) == 0x188, "Class is wrong size!");

// 8 bytes of work memory the attack allocates for its hit setup (HYPOTHESIS: filled in by the module builder).
struct grDxOnettAttackWork {
    u32 unk0;
    u32 unk4;
    grDxOnettAttackWork() : unk0(0), unk4(0) { }
};

// The attack of the cars: a box that follows the road and hurts the fighters the car runs over.
class grDxOnettAttack : public grDxOnett {
    u8* m_curWork;             // 0x15C which car is on the road now
    u8* m_stateWork;           // 0x160
    u8 m_hasYakumono;          // 0x164
    u8 m_attackSet;            // 0x165
    grDxOnettAttackWork* m_work; // 0x168

public:
    grDxOnettAttack(const char* taskName);
    virtual ~grDxOnettAttack();
    virtual void update(float deltaFrame);
    virtual void onInflict(soCollisionLog* collisionLog, u32 flags, float power);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setCurWork(u8* curWork) { m_curWork = curWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    static grDxOnettAttack* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxOnettAttack) == 0x16C, "Class is wrong size!");

// The billboards ("Kanban") and the awning ("HisashiB") above the road: they bend when fighters land on them.
class grDxOnettKanban : public grDxOnett {
    u8* m_levelWork;           // 0x15C how bent the sign is (0..3)
    u8* m_countWork;           // 0x160 landings left until the next level
    u8* m_motionFlgWork;       // 0x164 a landing started a motion
    u8 m_blink;                // 0x168
    s8 m_blinkCount;           // 0x169
    u8 m_isHisashiB;           // 0x16A this ground is the awning
    u8 m_animId;               // 0x16B current animation (5 = none)
    float m_animFrames;        // 0x16C frame count of the current animation
    StSeUtil::SeSeqInstance<1, 2> m_sePlayer; // 0x170
    SndID m_seIds[2];          // 0x1BC
    StSeUtil::UnkStruct m_seData[2]; // 0x1C4

public:
    grDxOnettKanban(const char* taskName);
    virtual ~grDxOnettKanban();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision);
    virtual void updateMotion(float deltaFrame);
    virtual void updateMotionHisashiB(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setLevelWork(u8* levelWork) { m_levelWork = levelWork; }
    virtual void setCountWork(u8* countWork) { m_countWork = countWork; }
    virtual void setMotionFlgWork(u8* motionFlgWork) { m_motionFlgWork = motionFlgWork; }
    virtual void setHisashiB(u8 isHisashiB) { m_isHisashiB = isHisashiB; }
    static grDxOnettKanban* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxOnettKanban) == 0x1E4, "Class is wrong size!");
