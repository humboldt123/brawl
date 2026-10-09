#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_vector.h>
#include <so/collision/so_collision_hit_part.h>
#include <st/st_trigger_observe.h>
#include <types.h>
#include <yk/yakumono.h>

#include <st_dxgreens/st_dxgreens.h>

class stTrigger;

// The two words of data the Yakumono of the attacks is given (nothing is read from it).
struct grDxGreensWork {
    u32 unk0;
    u32 unk4;
    grDxGreensWork() {
        unk0 = 0;
        unk4 = 0;
    }
};

// The ground that all the grounds of the stage are made from: the only thing it adds is the call that sets the ground up
// for the match.
class grDxGreens : public grYakumono {
public:
    grDxGreens(const char* taskName);
    virtual ~grDxGreens();
    static grDxGreens* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGreens) == 0x150, "Class is wrong size!");

// One of the thirty blocks that hang over the stage. The stage owns the data of the block; the ground waits until the
// block is released, falls with growing speed to its rest position and breaks when a fighter hits it from below.
class grDxGreensBlock : public grDxGreens {
    u8 m_state;                       // 0x150 0 = hidden, 1 = waits, 2 = falls / at rest
    char _151[3];
    float m_timer;                    // 0x154
    float m_speed;                    // 0x158 the speed of the fall
    Vec3f* m_posLimit;                // 0x15C the limits of the camera (the block starts at the lower one)
    stDxGreensBlockData* m_blockData; // 0x160 the data of the block (from the stage)
    u8 m_landed;                      // 0x164 1 once it hit its rest position
    char _165[3];

public:
    grDxGreensBlock(const char* taskName);
    virtual ~grDxGreensBlock();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updatePos(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setBlockDataWork(stDxGreensBlockData* blockData) { m_blockData = blockData; }
    virtual void setPosLimitWork(Vec3f* posLimit) { m_posLimit = posLimit; }
    virtual void receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision);
    static grDxGreensBlock* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGreensBlock) == 0x168, "Class is wrong size!");

// The ground whose nodes ("BlockN1".."BlockN30") give the stage the positions of the blocks. Its Yakumono has a hit
// sphere on every node (the blocks take damage) and a second Yakumono holds the attacks of the breaking blocks.
class grDxGreensBlockPos : public grDxGreens {
    stDxGreensBlockData* m_blockData;  // 0x150 (from the stage)
    Vec3f* m_posBlock;                 // 0x154 the positions of the blocks (to the stage)
    u8 m_posMade;                      // 0x158
    u8 m_attackOn[30];                 // 0x159 1 = the attack of the block is on
    char _177;
    float m_posX[30];                  // 0x178 x position of the nodes (for the attacks)
    u8 m_yakumonoMade;                 // 0x1F0
    char _1f1[3];
    soCollisionHitData* m_hitData;               // 0x1F4 the thirty hit spheres (HYPOTHESIS: names)
    soCollisionHitData::Simple* m_hitSimple;     // 0x1F8 the same thirty in the simple form with their nodes
    soSet<soCollisionHitData::Simple>* m_simpleSets; // 0x1FC one set of one element per sphere
    ykDataGroup* m_dataGroups;                   // 0x200 one data group per sphere
    ykData* m_data;                              // 0x204 the data of the hit Yakumono (the thirty data groups)
    grDxGreensWork* m_workAttack;                // 0x208 the (empty) data of the Yakumono with the attacks
    Yakumono* m_yakumonoAttack;                  // 0x20C the Yakumono with the attacks
    u32 m_node[30];                    // 0x210 node indices of "BlockN1".."BlockN30" (in the order of the nodes)

public:
    grDxGreensBlockPos(const char* taskName);
    virtual ~grDxGreensBlockPos();
    virtual void preExit();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updatePos(float deltaFrame);
    virtual void setHit();
    virtual void setAttack(u8 column, u8 row);
    virtual void setBlockDataWork(stDxGreensBlockData* blockData) { m_blockData = blockData; }
    virtual void setBlockPosWork(Vec3f* posBlock) { m_posBlock = posBlock; }
    virtual bool setNode();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    static grDxGreensBlockPos* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGreensBlockPos) == 0x288, "Class is wrong size!");

// Whispy Woods: blows wind to one side (towards the fighters that are further away), now and then breathes in and drops a
// bunch of apples (the item Item_Stage_Apple).
class grDxGreensWhispy : public grDxGreens {
    u8 m_phase;                       // 0x150 0 / 1 = blowing from the left / right, 2 = ..., 4 = breathing in
    char _151[3];
    float m_timer;                    // 0x154 time until the next blow
    u32 m_effect;                     // 0x158 handle of the effect of the wind
    u8 m_motion;                      // 0x15C
    char _15d[3];
    float m_windFrame;                // 0x160 frame of the end of the wind
    float m_motionFrames;             // 0x164 frame count of the current motion
    u8 m_windCount;                   // 0x168 counts down the repeats of the wind
    char _169[3];
    stTrigger* m_trigger;             // 0x16C the wind trigger (from the stage)
    grGimmickWindData* m_windData;    // 0x170

public:
    grDxGreensWhispy(const char* taskName);
    virtual ~grDxGreensWhispy();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void setupWind(int direction);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setTrigger(stTrigger* trigger) { m_trigger = trigger; }
    virtual void setWindDataWork(grGimmickWindData* windData) { m_windData = windData; }
    virtual bool isWindToLeft();
    static grDxGreensWhispy* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxGreensWhispy) == 0x174, "Class is wrong size!");
