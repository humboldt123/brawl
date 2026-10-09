#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <st_mariopast/nw4r_scnproc.h>
#include <snd/snd_3d_generator.h>
#include <so/collision/so_collision_hit_part.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_mariopast/st_mariopast.h>

class grCollisionJoint;
class grCollStatus;

// The base ground of the stage.
class grMarioPast : public grYakumono {
protected:
    u8 m_state;            // 0x150
    char _151[3];
    float unk154;          // 0x154

public:
    grMarioPast(const char* taskName);
    virtual ~grMarioPast();
    static grMarioPast* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMarioPast) == 0x158, "Class is wrong size!");

// The background model: torches that burn and a ground that scrolls with the animation. In the first stage (m_stage 0) the
// lifts of the other stage are not there.
class grMarioPastBg : public grMarioPast {
    float* m_frameLocator;      // 0x158 the frame of the stage animation (from the locator ground)
    Vec3f* m_posGimmick;        // 0x15C the positions the grounds publish (to the stage)
    u32 m_node[12];             // 0x160 node indices of the locators ("LiftLocator01".., "TorchLocator02"..)
    u8 m_stage;                 // 0x190
    u8 m_torchEffect;           // 0x191 1 until the lights are turned off
    char _192[2];
    snd3DGenerator m_snd[6];    // 0x194 the sound sources of the torches (the first one doubles as the stage's)
    int m_sndId[6];             // 0x1C4

public:
    grMarioPastBg(const char* taskName);
    virtual ~grMarioPastBg();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setFrameLocatorWork(float* frame) { m_frameLocator = frame; }
    virtual void setPosGimmickWork(Vec3f* posGimmick) { m_posGimmick = posGimmick; }
    virtual void setStage(u8 stage) { m_stage = stage; }
    static grMarioPastBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMarioPastBg) == 0x1DC, "Class is wrong size!");

// The locators of the platforms and the scenery: follows the animation and gives the stage the positions of its nodes.
class grMarioPastBgLocator : public grMarioPast {
    float* m_frameLocator;      // 0x158
    Vec3f* m_posGimmick;        // 0x15C
    u8 m_stage;                 // 0x160
    char _161[3];
    u32 m_node[6];              // 0x164
    // (the nodes: scenery locator, two platform locators A (two nodes each) and one more)

public:
    grMarioPastBgLocator(const char* taskName);
    virtual ~grMarioPastBgLocator();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void setFrameLocatorWork(float* frame) { m_frameLocator = frame; }
    virtual void setPosGimmickWork(Vec3f* posGimmick) { m_posGimmick = posGimmick; }
    virtual void setStage(u8 stage) { m_stage = stage; }
    static grMarioPastBgLocator* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMarioPastBgLocator) == 0x17C, "Class is wrong size!");

// One of the six lifts (three on each side): they ride up and down between the limits of the camera.
class grMarioPastLift : public grMarioPast {
    Vec3f* m_posWork;           // 0x158 the start and end position of the lift (from the stage)
    Vec3f* m_posLimit;          // 0x15C the camera limits
    u8* m_ctrlId;               // 0x160 which of the three lifts is riding
    Vec3f m_pos;                // 0x164
    u8 m_id;                    // 0x170
    char _171[3];

public:
    grMarioPastLift(const char* taskName);
    virtual ~grMarioPastLift();
    virtual void update(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosWork(Vec3f* posWork) { m_posWork = posWork; }
    virtual void setPosLimitWork(Vec3f* posLimit) { m_posLimit = posLimit; }
    virtual void setCtrlIDWork(u8* ctrlId) { m_ctrlId = ctrlId; }
    virtual void setID(u8 id) { m_id = id; }
    static grMarioPastLift* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grMarioPastLift) == 0x174, "Class is wrong size!");

// The blocks of one of the two models. The ground draws them itself (a quad per side) and moves them when they are bumped;
// the Yakumono it creates has a hit sphere for every group of blocks that stand on the same height.
class grMarioPastBlockPosLite : public grYakumono {
public:
    nw4r::g3d::ScnProc* m_scnProc;             // 0x150 the scene object whose draw function draws the blocks
    nw4r::g3d::ResFile m_resFile;              // 0x154 the file of the textures
    u8 m_stage;                                // 0x158
    u8 m_group;                                // 0x159
    char _15a[2];
    u32 m_num;                                 // 0x15C number of blocks
    stMarioPastBlockDt* m_blocks;              // 0x160
    grCollisionJoint* m_joints;                // 0x164 the joints of the collision of the blocks
    Vec3f* m_posGimmick;                       // 0x168
    Vec3f* m_limit;                            // 0x16C
    Matrix m_mtx;                              // 0x170
    snd3DGenerator m_snd;                      // 0x1A0
    soCollisionHitData::Simple* m_hitSimple[2]; // 0x1A8 the hit spheres (one per block) of the two models
    soSet<soCollisionHitData::Simple>* m_simpleSets[2]; // 0x1B0 one set of the spheres of a row of blocks per row
    ykDataGroup* m_dataGroups[2];              // 0x1B8
    ykData* m_data[2];                         // 0x1C0
    int m_rowNum[2];                           // 0x1C8 number of rows (heights)
    int m_rowHit[2][10];                       // 0x1D0 number of blocks of every row
    u8 _220_end[0];

    grMarioPastBlockPosLite(const char* taskName);
    virtual ~grMarioPastBlockPosLite();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision);
    void updateBlocks(float deltaFrame);
    void makeItem(stMarioPastBlockDt* block);
    void setHatenaHideBlockCollision(u32 index, int on);
    void initCollisionJoint();
    void initYaku();
    void drawProcBlocks();
    static void drawProc(nw4r::g3d::ScnProc* proc, bool opa);
    static grMarioPastBlockPosLite* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grMarioPastBlockPosLite) == 0x220, "Class is wrong size!");
