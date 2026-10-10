#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_yakumono.h>
#include <gr/collision/gr_collision_joint.h>
#include <math.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/ut/ut_LinkList.h>
#include <st_mariopast/nw4r_scnproc.h>
#include <snd/snd_3d_generator.h>
#include <so/collision/so_collision_hit_part.h>
#include <string.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_famicom/st_famicom.h>

// The base ground of the stage. The grounds that move have the state (the step of their sequence) and a timer.
class grFamicom : public grYakumono {
protected:
    u8 m_state;                 // 0x150
    char _151[3];
    float m_timer;              // 0x154

public:
    grFamicom(const char* taskName);
    virtual ~grFamicom();
};
static_assert(sizeof(grFamicom) == 0x158, "Class is wrong size!");

// The background. The places of the things of the stage are the places of its nodes. It also finds out which side of the
// stage the fighters are on, to land the green balls.
class grFamicomBg : public grFamicom {
protected:
    Vec3f* m_posYukaWork;       // 0x158 the places of the seven floors (to the stage)
    Vec3f* m_posPowWork;        // 0x15C the place of the POW block
    Vec3f* m_posEnemyWork;      // 0x160 the places the enemies come from and go to (four)
    Vec3f* m_posBallWork;       // 0x164 the places of the green balls (four)
    Vec3f* m_posLimitWork;      // 0x168 the part of the stage the camera shows
    float* m_landingWork;       // 0x16C how long a fighter stood on each of the four floors (from the stage)
    u8* m_landingFlgWork;       // 0x170 the fighters stand on the right side
    u8* m_stateAreaWork;        // 0x174 the state of the area under the floors
    u32 m_nodeYuka[7];          // 0x178 the nodes of the floors
    u32 m_nodePow;              // 0x194
    u32 m_nodeEnemy[4];         // 0x198
    u32 m_nodeBall[4];          // 0x1A8
    float m_landingTimer[4];    // 0x1B8

public:
    grFamicomBg(const char* taskName) __attribute__((never_inline));
    virtual ~grFamicomBg();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateArea(float deltaFrame);
    virtual void checkLR(int count, Vec3f* positions, bool* result);
    virtual void setPosYukaWork(Vec3f* posYukaWork) { m_posYukaWork = posYukaWork; }
    virtual void setPosPowWork(Vec3f* posPowWork) { m_posPowWork = posPowWork; }
    virtual void setPosEnemyWork(Vec3f* posEnemyWork) { m_posEnemyWork = posEnemyWork; }
    virtual void setPosBallWork(Vec3f* posBallWork) { m_posBallWork = posBallWork; }
    virtual void setPosLimitWork(Vec3f* posLimitWork) { m_posLimitWork = posLimitWork; }
    virtual void setLandingWork(float* landingWork) { m_landingWork = landingWork; }
    virtual void setLandingFlgWork(u8* landingFlgWork) { m_landingFlgWork = landingFlgWork; }
    virtual void setStateAreaWork(u8* stateAreaWork) { m_stateAreaWork = stateAreaWork; }

    static grFamicomBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grFamicomBg) == 0x1C8, "Class is wrong size!");

// A green ball that rolls over the stage from one side to the other.
class grFamicomBall : public grFamicom {
protected:
    Vec3f* m_posWork;           // 0x158 the places of the balls (from the stage)
    u8* m_stateWork;            // 0x15C the state of the ball (to the stage)
    u8* m_ltoRWork;             // 0x160 it rolls from the left to the right
    float m_posX;               // 0x164 the place of the ball (x)
    float m_posXPrev;           // 0x168
    u32 m_nodeBall;             // 0x16C
    u8 m_motion;                // 0x170
    char _171[3];
    float m_prevFrame;          // 0x174 the frame of the last update
    u8 m_hasHit;                // 0x178
    u8 m_attackSet;             // 0x179
    char _17a[2];
    ykData* m_attackWork;       // 0x17C the work of the hit object (two numbers)
    snd3DGenerator m_snd;       // 0x180
    s32 m_seHandle;             // 0x188 the sound that plays while it rolls (-1 = none)
    s32 m_dangerZone;           // 0x18C

public:
    grFamicomBall(const char* taskName) __attribute__((never_inline));
    virtual ~grFamicomBall();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual bool setNode();
    virtual void setHit();
    virtual void setAttack();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setLtoRWork(u8* ltoRWork) { m_ltoRWork = ltoRWork; }

    static grFamicomBall* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grFamicomBall) == 0x190, "Class is wrong size!");

// A fighter that stands on a floor and is remembered by it (the ground of the floor keeps a list of them).
struct grFamicomYukaActor : public nw4r::ut::LinkListNode {
    grCollStatus* m_status;     // 0x08 the collision status of the fighter
    u8 m_life;                  // 0x0C frames it stays on the list after it left
    char _0d[3];
};

// One corner of a floor: where it is, how far it is pushed down (by the fighter standing on it) and for how long.
struct grFamicomYukaVtx {
    Vec3f m_pos;                // 0x00
    Vec3f m_offset;             // 0x0C
    u8 m_active;                // 0x18
    char _19[3];
    float m_timer;              // 0x1C
};
static_assert(sizeof(grFamicomYukaVtx) == 0x20, "Class is wrong size!");

// A floor of the stage (a row of blocks). The ground draws the blocks itself (the scene object of the ground has the draw
// function), moves the corners of the floor down when a fighter stands on it and tells the stage when a fighter hit it.
class grFamicomYuka : public grFamicom {
protected:
    Vec3f* m_posWork;           // 0x158 the place of the floor (from the stage)
    stFamicomEnemyData* m_enemyDataWork; // 0x15C the enemies (from the stage)
    stFamicomYukaToss* m_tossData; // 0x160 what the fighters did to this floor (to the stage)
    u8 m_unk164;                // 0x164
    char _165[3];
    u32 m_node[30];             // 0x168 the nodes of the blocks
    u8 m_type;                  // 0x1E0
    char _1e1[3];
    snd3DGenerator m_snd;       // 0x1E4
    nw4r::ut::LinkList<grFamicomYukaActor, 0> m_actors; // 0x1EC the fighters on the floor
    grFamicomYukaVtx m_vtx[62]; // 0x1F8 the corners of the blocks (two rows)
    nw4r::g3d::ScnProc* m_scnProc; // 0x9B8 the scene object that draws the floor
    u8 m_unk9bc;                // 0x9BC
    u8 m_unk9bd;                // 0x9BD
    u8 m_ctrl;                  // 0x9BE how many blocks the floor has
    char _9bf;
    float m_width;              // 0x9C0 size of a block
    float m_height;             // 0x9C4
    float m_long;               // 0x9C8
    u8 m_rotFlg;                // 0x9CC
    char m_ctrlName[0x83];      // 0x9CD the prefix of the nodes of the blocks
    nw4r::g3d::ResFile m_resFile; // 0xA50 the textures of the floor

public:
    grFamicomYuka(const char* taskName) __attribute__((never_inline));
    virtual ~grFamicomYuka();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateColl(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateVtx(float deltaFrame);
    virtual void updateYukaData(float deltaFrame);
    virtual void updateActor(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setEnemyDataWork(stFamicomEnemyData* enemyDataWork) { m_enemyDataWork = enemyDataWork; }
    virtual void setYukaTossData(stFamicomYukaToss* tossData) { m_tossData = tossData; }
    virtual bool isValidResTex();
    virtual nw4r::g3d::ResTex getResTex(const char* name) { return m_resFile.GetResTex(name); }
    virtual grFamicomYukaVtx* getYukaData(int index) { return &m_vtx[index]; }
    virtual void setType(u8 type) { m_type = type; }
    virtual void setCtrl(u8 ctrl) { m_ctrl = ctrl; }
    virtual u8 getCtrl() { return m_ctrl; }
    virtual void setCtrlName(const char* name);
    virtual float getWidthBlock() { return m_width; }
    virtual float getHeightBlock() { return m_height; }
    virtual float getLongBlock() { return m_long; }
    virtual void setRotFlg(u8 rotFlg) { m_rotFlg = rotFlg; }
    virtual bool isActorAlready(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list, grCollStatus* status);
    virtual void clearActor(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list, int index);
    virtual void clearActorAll(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list);
    virtual grFamicomYukaActor* getActor(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list, grCollStatus* status);
    virtual grFamicomYukaActor* getActor1(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list, int index);

    void drawProcYuka(float width, float height, Vec3f* pos0, Vec3f* pos1, int corner);
    static void drawProc(nw4r::g3d::ScnProc* proc, bool opa);
    static grFamicomYuka* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grFamicomYuka) == 0xA54, "Class is wrong size!");

// The POW block: a fighter who jumps against it from below shakes the stage (quake) and knocks the enemies on the floors over.
class grFamicomPow : public grFamicom {
protected:
    float m_blink;              // 0x158 frames until the block flips its look (it blinks while it is hit)
    Vec3f* m_posWork;           // 0x15C the place of the block (from the stage)
    u8* m_stateWork;            // 0x160 the state of the block (to the stage)
    u8 m_hits;                  // 0x164 how often it was hit (3 = used up)
    char _165[3];
    u32 m_nodeColl[2];          // 0x168 the nodes of the collision
    float m_cooldown[7];        // 0x170 frames until the same fighter can hit it again
    u8 m_hasHit;                // 0x18C
    char _18d[3];
    soCollisionHitData* m_hitData;           // 0x190 the hit sphere
    soCollisionHitData::Simple* m_hitSimple; // 0x194 its form for the hit object
    soSet<soCollisionHitData>* m_hitSet;     // 0x198
    ykDataGroup* m_dataGroup;                // 0x19C
    ykData* m_data;                          // 0x1A0
    snd3DGenerator m_snd;       // 0x1A4

public:
    grFamicomPow(const char* taskName) __attribute__((never_inline));
    virtual ~grFamicomPow();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateYakumono(float deltaFrame);
    virtual void updatePow(float deltaFrame);
    virtual void updatePreBuild(float deltaFrame);
    virtual void updateColl(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void pow();
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }

    static grFamicomPow* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grFamicomPow) == 0x1AC, "Class is wrong size!");

// The base of the enemies of the stage (the shellcreeper and the sidestepper): their state machine is the same, the
// subclasses have their own animations, attacks and what they do when they are hit.
class grFamicomEnemy : public grFamicom {
protected:
    u8 m_motion;                // 0x158
    char _159[3];
    float m_actionTimer;        // 0x15C
    float m_attackTimer;        // 0x160
    float m_speed;              // 0x164
    float m_fallSpeed;          // 0x168
    u8 m_firstStep;             // 0x16C
    u8 m_moving;                // 0x16D
    u8 m_turnState;             // 0x16E
    char _16f;
    float m_hitDir;             // 0x170 the side the last hit came from
    u8 m_level;                 // 0x174 how angry the enemy is (it walks faster)
    u8 m_index;                 // 0x175 its record in the table of the stage
    u8 m_landCount;             // 0x176
    char _177;
    stFamicomEnemyData* m_enemyData; // 0x178 the records of the enemies (from the stage)
    Vec3f* m_posCtrlWork;       // 0x17C the places the enemies come from and go to (from the stage)
    Vec3f* m_posLimitWork;      // 0x180 the part of the stage the camera shows
    stFamicomYukaToss* m_tossData; // 0x184 what the fighters did to the floors
    u8 m_hasHit;                // 0x188
    u8 m_attackSet;             // 0x189
    char _18a[2];
    soCollisionHitData* m_hitData;           // 0x18C the hit sphere
    soCollisionHitData::Simple* m_hitSimple; // 0x190
    soSet<soCollisionHitData>* m_hitSet;     // 0x194
    ykDataGroup* m_dataGroup;                // 0x198
    ykData* m_data;                          // 0x19C
    snd3DGenerator m_snd;       // 0x1A0
    s32 m_itemId;               // 0x1A8 the item the enemy carries (0 = none)
    u8 m_unk1ac;                // 0x1AC
    u8 m_fixPosition;           // 0x1AD the position is set after the collision
    char _1ae[2];
    s32 m_dangerZone;           // 0x1B0
    Vec3f m_scale;              // 0x1B4 the size of the model at the start

public:
    grFamicomEnemy(const char* taskName) __attribute__((never_inline));
    virtual ~grFamicomEnemy();
    virtual void update(float deltaFrame);
    virtual void processFixPosition();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void onInflict(soCollisionLog* collisionLog, u32 unk2, float power);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack() { }
    virtual void toItem() { }
    virtual void toGround() { }
    virtual void requestAngry() { }
    virtual void requestDown() { }
    virtual void requestMove() { }
    virtual void requestReMove() { }
    virtual void requestReMoveAttack() { }
    virtual void requestTurn() { }
    virtual void requestItem() { }
    virtual void requestBlowOff();
    virtual void requestFall();
    virtual void setEnemyDataWork(stFamicomEnemyData* enemyData) { m_enemyData = enemyData; }
    virtual void setYukaTossData(stFamicomYukaToss* tossData) { m_tossData = tossData; }
    virtual void setPosCtrlWork(Vec3f* posCtrlWork) { m_posCtrlWork = posCtrlWork; }
    virtual void setPosLimitWork(Vec3f* posLimitWork) { m_posLimitWork = posLimitWork; }
    virtual void setIndex(u8 index) { m_index = index; }
    virtual bool isAppear();
    virtual bool isBottomLine();
    virtual bool isExistItem();
    virtual bool isRemoveItem();
    virtual bool isTurnGround();
    virtual bool isTurn();
    virtual void playSEAppear() { }
    virtual void playSEDown() { }
};
static_assert(sizeof(grFamicomEnemy) == 0x1C0, "Class is wrong size!");

// The shellcreeper: it is flipped over when a fighter hits the floor under it, and gets up angrier.
class grFamicomKame : public grFamicomEnemy {
public:
    // (inline: the original has no constructor of its own)
    grFamicomKame(const char* taskName) : grFamicomEnemy(taskName) {
        m_motion = 0xC;
    }
    virtual ~grFamicomKame();
    virtual void updateMotion(float deltaFrame);
    virtual void setAttack();
    virtual void toItem();
    virtual void toGround();
    virtual void requestDown();
    virtual void requestMove();
    virtual void requestReMove();
    virtual void requestReMoveAttack();
    virtual void requestTurn();
    virtual void playSEAppear();
    virtual void playSEDown();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void requestDown2();

    static grFamicomKame* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grFamicomKame) == 0x1C0, "Class is wrong size!");

// The sidestepper: it needs two hits from below, and gets angry after the first one.
class grFamicomKani : public grFamicomEnemy {
protected:
    u8 m_angry;                 // 0x1C0

public:
    // (inline: the original has no constructor of its own)
    grFamicomKani(const char* taskName) : grFamicomEnemy(taskName) {
        m_motion = 0xC;
        m_angry = 0;
    }
    virtual ~grFamicomKani();
    virtual void updateMotion(float deltaFrame);
    virtual void setAttack();
    virtual void toItem();
    virtual void toGround();
    virtual void requestAngry();
    virtual void requestDown();
    virtual void requestMove();
    virtual void requestReMove();
    virtual void requestReMoveAttack();
    virtual void requestTurn();
    virtual void playSEAppear();
    virtual void playSEDown();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void requestDown2();

    static grFamicomKani* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grFamicomKani) == 0x1C4, "Class is wrong size!");
