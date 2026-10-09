#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_vector.h>
#include <so/collision/so_collision_hit_part.h>
#include <types.h>
#include <yk/yakumono.h>

// Yoster Island (DX) grounds: Yakumono grounds with a state byte and a timer. The stage hands them the pointers to the
// data they share through the virtual set...Work functions right after it creates them.
class grDxYorster : public grYakumono {
protected:
    u8 m_state;                // 0x150
    float m_timer;             // 0x154

public:
    grDxYorster(const char* taskName);
    virtual ~grDxYorster();
    static grDxYorster* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxYorster) == 0x158, "Class is wrong size!");

// The backdrop. Two timers keep the left and right half of the island from being used while a fighter stands next to
// the block nodes.
class grDxYorsterBg : public grDxYorster {
    Vec3f* m_posGimmickWork;   // 0x158 positions of the block nodes (from the stage)
    u8* m_stateLWork;          // 0x15C
    u8* m_stateRWork;          // 0x160
    float m_timerL;            // 0x164 frames until the left half is free again
    float m_timerR;            // 0x168

public:
    grDxYorsterBg(const char* taskName) : grDxYorster(taskName) {
        m_posGimmickWork = NULL;
        m_stateLWork = NULL;
        m_stateRWork = NULL;
        m_timerL = 0.0f;
        m_timerR = 0.0f;
    }
    virtual ~grDxYorsterBg();
    virtual void update(float deltaFrame);
    virtual void updateArea(float deltaFrame);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    virtual void setStateLWork(u8* stateLWork) { m_stateLWork = stateLWork; }
    virtual void setStateRWork(u8* stateRWork) { m_stateRWork = stateRWork; }
    static grDxYorsterBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxYorsterBg) == 0x16C, "Class is wrong size!");

// One of the nine rotating blocks. A fighter hitting it from below makes it turn over (motion 1), after which it comes
// back (motion 0 = idle, 2 = the second turn).
class grDxYorsterBlock : public grDxYorster {
    Vec3f* m_pos;              // 0x158 node position the block follows (from the stage)
    u8* m_stateWork;           // 0x15C state byte of the left/right half the block belongs to
    u8 m_index;                // 0x160 which of the nine blocks this is
    u8 m_motion;               // 0x161 current motion (3 = none)
    float m_motionFrames;      // 0x164 frame count of the current motion
    u8* m_dmg;                 // 0x168 per-block damage byte of the stage

public:
    grDxYorsterBlock(const char* taskName);
    virtual ~grDxYorsterBlock();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPos(Vec3f* pos) { m_pos = pos; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setIndex(int index) { m_index = index; }
    virtual void setDmg(u8* dmg) { m_dmg = dmg; }
    virtual void receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision);
    static grDxYorsterBlock* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxYorsterBlock) == 0x16C, "Class is wrong size!");

// The block position ground: its nodes ("KurukuruBlockA".."I") give the stage the positions of the nine blocks, and a
// Yakumono with nine hit areas on those nodes tells which block a fighter hit.
class grDxYorsterBlockPos : public grDxYorster {
    u8 m_yakumonoMade;         // 0x158
    Vec3f* m_posGimmickWork;   // 0x15C positions of the block nodes (to the stage)
    u8* m_dmgWork;             // 0x160 one damage byte per block
    soCollisionHitData* m_hitData;               // 0x164 the nine hit spheres (HYPOTHESIS: names)
    soCollisionHitData::Simple* m_hitSimple;     // 0x168 the same nine in the simple form with their nodes
    soSet<soCollisionHitData::Simple>* m_simpleSet; // 0x16C
    ykDataGroup* m_dataGroup;                    // 0x170
    ykData* m_data;                              // 0x174
    u32 m_node[9];             // 0x178 node indices of "KurukuruBlockA".."I"

public:
    grDxYorsterBlockPos(const char* taskName);
    virtual ~grDxYorsterBlockPos();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual bool setNode();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    virtual void setDmgWork(u8* dmgWork) { m_dmgWork = dmgWork; }
    static grDxYorsterBlockPos* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDxYorsterBlockPos) == 0x19C, "Class is wrong size!");
