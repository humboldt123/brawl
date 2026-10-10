#pragma once

#include <StaticAssert.h>
#include <ai/ai_mgr.h>
#include <gf/gf_archive.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_ladder.h>
#include <gr/gr_gimmick_spring.h>
#include <gr/gr_yakumono.h>
#include <math.h>
#include <mt/mt_vector.h>
#include <ms/ms_message.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_3d_generator.h>
#include <so/area/so_area_module_impl.h>
#include <so/collision/so_collision_hit_part.h>
#include <st/se_util.h>
#include <st/st_data_container.h>
#include <st/st_trigger.h>
#include <string.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_pictchat/st_pictchat.h>

// The base ground of the stage. The grounds that move have the state (the step of their sequence) and a timer.
class grPictchat : public grYakumono {
protected:
    u8 m_state;                 // 0x150
    char _151[3];
    float m_timer;              // 0x154

public:
    grPictchat(const char* taskName);
    virtual ~grPictchat();
};
static_assert(sizeof(grPictchat) == 0x158, "Class is wrong size!");

// The background. It shows the name of the fighter whose picture is drawn, in the font of the stage, and turns the floor of
// the model off (the joint of its collision) when the picture is the last one (0x13).
class grPictchatBg : public grPictchat {
protected:
    Message* m_message;         // 0x158
    void* m_msgData;            // 0x15C the messages of the stage
    u8 m_charIndex;             // 0x160 the player whose name is shown (0xFF = none yet)
    char _161[3];
    u8* m_pictIDWork;           // 0x164 the picture that is drawn (from the stage)
    grCollisionJoint* m_joint;  // 0x168 the joint of the collision of this ground

public:
    // (inline: the original has no constructor of its own)
    grPictchatBg(const char* taskName) : grPictchat(taskName) {
        m_message = NULL;
        m_msgData = NULL;
        m_charIndex = 0xFF;
        m_pictIDWork = NULL;
        m_joint = NULL;
    }
    virtual ~grPictchatBg();
    virtual void update(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual void updateMessage(float deltaFrame);
    virtual void setMsgData(void* msgData);
    virtual void setPictIDWork(u8* pictIDWork);

    static grPictchatBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatBg) == 0x16C, "Class is wrong size!");

// The data of the collision that a picture has (the frames at which its parts get hard).
struct grPictchatCollCtrl {
    u8 m_count;                 // 0x00 how many joints
    char _1[3];
    float m_frames[1];          // 0x04 ... the frame of each joint
};

// A picture (the ground of it is made from a model with the name of the picture). The picture is drawn when the stage says
// so (the id of the picture is the one of the work), it is shown for a while, and it is erased; the ground has the states of
// that: 0 init, 1 wait, 2 draw, 3 loop (the picture is there), 4 erase. The collisions of the picture are in the joints of the
// collision and they get hard one after the other while the picture is drawn.
class grPictchatPict : public grPictchat {
protected:
    Vec3f* m_posGimmickWork;    // 0x158 the places of the gimmicks (from the stage)
    u8* m_pictIDWork;           // 0x15C the picture that is drawn (from the stage)
    u8 m_pictID;                // 0x160 the picture of this ground
    char _161[3];
    u8* m_pictCountWork;        // 0x164 how many pictures are there (from the stage)
    u8 m_pictCount;             // 0x168 what is added to it when this is drawn
    u8 m_type;                  // 0x169 the sort of the picture (8 = none of the special ones)
    char _16a[2];
    u8* m_stateWork;            // 0x16C a state of the stage for the picture
    stDataContainer* m_collCtrlTbl; // 0x170 the data of the collision of the picture (from the stage)
    grCollisionJoint** m_joints; // 0x174 the joints that get hard
    u8 m_jointNum;              // 0x178
    u8 m_jointIndex;            // 0x179
    char m_nodeHeader[0x102];   // 0x17A the name of the picture in the model ("P001")
    u8* m_stateAttackWork;      // 0x27C the states of the attacks of the picture (from the stage)
    u8 m_yakumonoSet;           // 0x280
    u8 m_attackSet;             // 0x281
    char _282[2];
    ykData* m_data;             // 0x284
    u8 m_motion;                // 0x288
    char _289[3];
    float m_frame;              // 0x28C
    float m_frameCount;         // 0x290 the length of the animation that is bound

public:
    // (inline: the original has no constructor of its own)
    grPictchatPict(const char* taskName);
    virtual ~grPictchatPict();
    virtual void update(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual void updatePict(float deltaFrame);
    virtual void updatePictInit(float deltaFrame);
    virtual void updatePictWait(float deltaFrame);
    virtual void updatePictDraw(float deltaFrame);
    virtual void updatePictDrawDetails(float deltaFrame);
    virtual void updatePictLoop(float deltaFrame);
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void updatePictElase(float deltaFrame);
    virtual void updatePictElaseDetails(float deltaFrame);
    virtual void updateCollision(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void clearAttackAll();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork);
    virtual void setPictIDWork(u8* pictIDWork);
    virtual void setPictID(u8 pictID);
    virtual void setPictCountWork(u8* pictCountWork);
    virtual void setPictCount(u8 pictCount);
    virtual void setType(u8 type);
    virtual void setStateWork(u8* stateWork);
    virtual void setStateAttackWork(u8* stateAttackWork);
    virtual void setNodeHeader(const char* nodeHeader);
    virtual void setTblCollCtrlAcc(stDataContainer* tblCollCtrl);

    // The joints of the picture are made soft again (the bits of the hard floor are cleared).
    void clearJoints() {
        if (m_joints != NULL) {
            u32 i = 0;
            u8 count = m_jointNum;
            for (; i != count; i++) {
                grCollisionJoint* joint = m_joints[i];
                joint->m_0x54_6 = joint->m_0x54_4 = joint->m_0x54_7 = false;
            }
        }
    }

    static grPictchatPict* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict) == 0x294, "Class is wrong size!");

// The spikes (P007): six hit spheres on the nodes of the picture, and the danger zones for the computer players.
class grPictchatPict007 : public grPictchatPict {
protected:
    s32 m_dangerZone0;          // 0x294
    s32 m_dangerZone1;          // 0x298
    u32 m_nodeDamage[6];        // 0x29C the nodes of the spikes

public:
    grPictchatPict007(const char* taskName);
    virtual ~grPictchatPict007();
    virtual bool setNode();
    virtual void updateJoint(float deltaFrame);
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void updatePictElaseDetails(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void clearAttackAll();
    virtual void setAttack();
    virtual void setAttack1(int index);

    static grPictchatPict007* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict007) == 0x2B4, "Class is wrong size!");

// The spring (P008): a spring on the left (type 0) or on the right (type 1) that shoots the fighters up.
class grPictchatPict008 : public grPictchatPict {
protected:
    Vec3f* m_posSpringWork;     // 0x294 the place of the spring (to the stage)
    u8* m_flgSpringWork;        // 0x298 the flag that says the spring is there (to the stage)
    snd3DGenerator m_snd;       // 0x29C

public:
    grPictchatPict008(const char* taskName);
    virtual ~grPictchatPict008();
    virtual void processAnim();
    virtual void updatePictDraw(float deltaFrame);
    virtual void updatePictLoop(float deltaFrame);
    virtual void setPosSpringWork(Vec3f* posSpringWork);
    virtual void setFlgSpringWork(u8* flgSpringWork);

    static grPictchatPict008* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict008) == 0x2A4, "Class is wrong size!");

// The clock (P009): it ticks while it is there.
class grPictchatPict009 : public grPictchatPict {
protected:
    snd3DGenerator m_snd;       // 0x294
    s32 m_seHandle;             // 0x29C

public:
    grPictchatPict009(const char* taskName);
    virtual ~grPictchatPict009();
    virtual void updatePictDraw(float deltaFrame);
    virtual void updatePictLoop(float deltaFrame);

    static grPictchatPict009* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict009) == 0x2A0, "Class is wrong size!");

// The ferris wheel (P011).
class grPictchatPict011 : public grPictchatPict {
protected:
    snd3DGenerator m_snd;       // 0x294
    s32 m_seHandle;             // 0x29C

public:
    grPictchatPict011(const char* taskName);
    virtual ~grPictchatPict011();
    virtual void updatePictDrawDetails(float deltaFrame);
    virtual void updatePictLoopDetails(float deltaFrame);

    static grPictchatPict011* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict011) == 0x2A0, "Class is wrong size!");

// The roller coaster (P012): it hurts the fighters it hits and makes the sound of the rails.
class grPictchatPict012 : public grPictchatPict {
protected:
    u32 m_nodeCoaster;          // 0x294 the node of the car
    u8 m_side;                  // 0x298 the side of the center the car was on the last time (1 = right)
    char _299[3];
    Vec3f m_posPrev;            // 0x29C the place of the car the last time
    snd3DGenerator m_snd;       // 0x2A8

public:
    grPictchatPict012(const char* taskName);
    virtual ~grPictchatPict012();
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void setAttack();
    virtual void setAttack1(int index);

    static grPictchatPict012* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict012) == 0x2B0, "Class is wrong size!");

// The missile (P013, type 2 and 3 are the two of them): it flies in from a side and bursts into an explosion when it is hit.
class grPictchatPict013 : public grPictchatPict {
protected:
    Vec3f* m_posBombWork;       // 0x294 the place of the missile (from the stage)
    Vec3f m_rot;                // 0x298 how it is turned
    float m_speedX;             // 0x2A4 how far it flies in a frame
    snd3DGenerator m_snd;       // 0x2A8
    s32 m_seHandle;             // 0x2B0
    s32 m_dangerZone;           // 0x2B4

public:
    grPictchatPict013(const char* taskName);
    virtual ~grPictchatPict013();
    virtual void onInflict(soCollisionLog* collisionLog, u32 unk1, float unk2);
    virtual void updateJoint(float deltaFrame);
    virtual void updatePictWait(float deltaFrame);
    virtual void updatePictDrawDetails(float deltaFrame);
    virtual void updatePictLoop(float deltaFrame);
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void updatePictElaseDetails(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setAttack();
    virtual void elase();
    virtual void setPosBombWork(Vec3f* posBombWork);

    static grPictchatPict013* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict013) == 0x2B8, "Class is wrong size!");

// The breath (P014): the wind of it is on while the picture is drawn.
class grPictchatPict014 : public grPictchatPict {
protected:
    snd3DGenerator m_snd;       // 0x294
    s32 m_seHandle;             // 0x29C
    stTrigger* m_trigger;       // 0x2A0 the trigger of the wind (from the stage)

public:
    grPictchatPict014(const char* taskName);
    virtual ~grPictchatPict014();
    virtual void updatePictDrawDetails(float deltaFrame);
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void setTrigger(stTrigger* trigger);

    static grPictchatPict014* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict014) == 0x2A4, "Class is wrong size!");

// The flower (P015): it sways and it has a sound sequence.
class grPictchatPict015 : public grPictchatPict {
protected:
    StSeUtil::SeSeqInstance<1, 2> m_seSeq; // 0x294
    SndID m_seSeqId[2];         // 0x2E0
    StSeUtil::UnkStruct m_seSeqData[3]; // 0x2E8
    snd3DGenerator m_snd;       // 0x318

public:
    grPictchatPict015(const char* taskName);
    virtual ~grPictchatPict015();
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void setAttack();
    virtual void setAttack1(int index);

    static grPictchatPict015* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict015) == 0x320, "Class is wrong size!");

// The ladder (P022): it tells the stage that it is there (the flag of the ladder) and its place is the one of its nodes.
class grPictchatPict022 : public grPictchatPict {
protected:
    Vec3f* m_posHashigoWork;    // 0x294 the places of the two ladders (to the stage)
    u8* m_flgHashigoWork;       // 0x298 the flag that says the ladders are there (to the stage)

public:
    grPictchatPict022(const char* taskName);
    virtual ~grPictchatPict022();
    virtual void processAnim();
    virtual void updatePictDraw(float deltaFrame);
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void setPosHashigoWork(Vec3f* posHashigoWork);
    virtual void setFlgHashigoWork(u8* flgHashigoWork);

    static grPictchatPict022* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict022) == 0x29C, "Class is wrong size!");

// The fire (P025).
class grPictchatPict025 : public grPictchatPict {
protected:
    snd3DGenerator m_snd;       // 0x294
    s32 m_seHandle;             // 0x29C
    s32 m_dangerZone0;          // 0x2A0
    s32 m_dangerZone1;          // 0x2A4

public:
    grPictchatPict025(const char* taskName);
    virtual ~grPictchatPict025();
    virtual void updatePictDrawDetails(float deltaFrame);
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void updatePictElaseDetails(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void setAttack();
    virtual void setAttack1(int index);

    static grPictchatPict025* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict025) == 0x2A8, "Class is wrong size!");

// The spear (P027): four hit spheres.
class grPictchatPict027 : public grPictchatPict {
protected:
    s32 m_dangerZone[4];        // 0x294

public:
    grPictchatPict027(const char* taskName);
    virtual ~grPictchatPict027();
    virtual void updatePictLoopDetails(float deltaFrame);
    virtual void updatePictElaseDetails(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void setAttack();
    virtual void setAttack1(int index);

    static grPictchatPict027* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatPict027) == 0x2A4, "Class is wrong size!");

// The ladder of the picture (it is there while the picture is).
class grPictchatLadder : public grGimmickLadder {
protected:
    u8 m_state;                 // 0x1A0
    char _1a1[3];
    Vec3f* m_posWork;           // 0x1A4
    u8* m_pictIDWork;           // 0x1A8
    u8 m_pictID;                // 0x1AC
    char _1ad[3];
    u8* m_flgWork;              // 0x1B0 the flag of the stage that says the ladder is there
    u8 m_triggerMade;           // 0x1B4
    u8 m_startFlag;             // 0x1B5
    char _1b6[2];

public:
    grPictchatLadder(const char* taskName);
    virtual ~grPictchatLadder();
    virtual void processGameProc();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork);
    virtual void setPictIDWork(u8* pictIDWork);
    virtual void setPictID(u8 pictID);
    virtual void setFlgWork(u8* flgWork);

    static grPictchatLadder* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatLadder) == 0x1B8, "Class is wrong size!");

// The spring of the picture.
class grPictchatSpring : public grGimmickSpring {
protected:
    u8 m_state;                 // 0x19C
    char _19d[3];
    float m_timer;              // 0x1A0
    u8* m_pictIDWork;           // 0x1A4
    u8 m_pictID;                // 0x1A8
    char _1a9[3];
    Vec3f* m_posWork;           // 0x1AC the place of the spring (from the picture)
    u8* m_stateWork;            // 0x1B0 the state of the spring (from the picture)
    u8* m_flgWork;              // 0x1B4 the flag that says the spring is there (from the picture)
    u8 m_triggerMade;           // 0x1B8
    u8 m_startFlag;             // 0x1B9
    char _1ba[2];

public:
    grPictchatSpring(const char* taskName);
    virtual ~grPictchatSpring();
    virtual void processGameProc();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual void getTopNode(Vec3f* pos);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPictIDWork(u8* pictIDWork);
    virtual void setPictID(u8 pictID);
    virtual void setPosWork(Vec3f* posWork);
    virtual void setStateWork(u8* stateWork);
    virtual void setFlgWork(u8* flgWork);

    static grPictchatSpring* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatSpring) == 0x1BC, "Class is wrong size!");

// The side bar: the lamps of it show how many pictures there were (23 lamps).
class grPictchatSideBar : public grPictchat {
protected:
    Vec3f* m_posGimmickWork;    // 0x158 the places of the gimmicks (from the stage)
    u32 m_nodeLamp[23];         // 0x15C the nodes of the lamps
    u8* m_pictCountWork;        // 0x1B8 how many pictures were shown (from the stage)

public:
    grPictchatSideBar(const char* taskName);
    virtual ~grPictchatSideBar();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void setPosGimmickWork(Vec3f* posGimmickWork);
    virtual void setPictCountWork(u8* pictCountWork);

    static grPictchatSideBar* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatSideBar) == 0x1BC, "Class is wrong size!");

// One lamp of the side bar (the type says which: 4 - 7).
class grPictchatSideBarLamp : public grPictchat {
protected:
    Vec3f* m_posWork;           // 0x158 the places of the lamps (from the stage)
    float unk15C;               // 0x15C
    float unk160;               // 0x160
    float unk164;               // 0x164
    u8 m_type;                  // 0x168
    char _169[3];

public:
    grPictchatSideBarLamp(const char* taskName);
    virtual ~grPictchatSideBarLamp();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork);
    virtual void setType(u8 type);

    static grPictchatSideBarLamp* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatSideBarLamp) == 0x16C, "Class is wrong size!");

// The attack of the stage (the ground with the hit object for the pictures that hurt).
class grPictchatAttack : public grPictchat {
protected:
    Vec3f* m_posWork;           // 0x158
    u8* m_stateWork;            // 0x15C the state of the attack (from the stage, 4 = on)
    u8* m_pictIDWork;           // 0x160
    u8 m_pictID;                // 0x164
    u8 m_yakumonoSet;           // 0x165
    u8 m_attackOn;              // 0x166
    char _167;
    ykData* m_data;             // 0x168

public:
    grPictchatAttack(const char* taskName);
    virtual ~grPictchatAttack();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setPosWork(Vec3f* posWork);
    virtual void setStateWork(u8* stateWork);
    virtual void setPictIDWork(u8* pictIDWork);
    virtual void setPictID(u8 pictID);
};
static_assert(sizeof(grPictchatAttack) == 0x16C, "Class is wrong size!");

class grPictchatAttack007 : public grPictchatAttack {
protected:
    u8 m_index;                 // 0x16C
    char _16d[3];

public:
    grPictchatAttack007(const char* taskName);
    virtual ~grPictchatAttack007();
    virtual void setAttack();
    virtual void setIndex(u8 index);

    static grPictchatAttack007* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPictchatAttack007) == 0x170, "Class is wrong size!");
