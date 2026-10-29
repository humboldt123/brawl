#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick_motion_path.h>
#include <sora_adv_stage/gr_adventure_move_ground.h>
#include <sora_adv_stage/gr_adventure_notify_gimmick_trigger_status.h>
#include <types.h>

// HYPOTHESIS: The type name is from the sora_adv_stage map (createGimmickFunc<23grAdventureRingLiftData, ...>); only the motion
// speeds per difficulty level (HYPOTHESIS: at 0x8) are read by the recovered functions.
struct grAdventureRingLiftData {
    u8 unk0[8];
    float m_motionSpeeds[15]; // 0x8
};

class grAdventureRingLift : public grAdventureMoveGround, public grAdventureNotifyGimmickTriggerStatus {
protected:
    int m_state;            // 0x164, HYPOTHESIS: 0 preparation, 1 entry, 2 standby, 3 start, 4 moving, 5 stop
    float m_animSpeed;      // 0x168
    u8 m_hasNodeAnim;       // 0x16C
    float m_animFrame;      // 0x170

public:
    virtual void notifyEventOnTrigger(int flag);
    virtual void notifyEventOffTrigger(int flag);
    virtual ~grAdventureRingLift();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void updateCallback(u32 index);

    static grAdventureRingLift* create(int mdlIndex, const char* taskName);

    void frowPreparation(float deltaFrame);
    void frowEntry(float deltaFrame);
    void frowStandby(float deltaFrame);
    void frowStart(float deltaFrame);
    void frowMoving(float deltaFrame);
    void frowStop(float deltaFrame);
};
static_assert(sizeof(grAdventureRingLift) == 0x174, "Class is wrong size!");
