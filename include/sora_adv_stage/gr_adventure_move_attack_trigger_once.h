#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_motion_path.h>
#include <sora_adv_stage/gr_adventure_move_attack_ground.h>
#include <types.h>

// HYPOTHESIS: The type name is from the sora_adv_stage map (createGimmickFunc<34grGimmickMoveAttackTriggerOnceData, ...>); only
// the attack that frowStart sets is described.
struct grGimmickMoveAttackTriggerOnceData {
    u8 unk0[0x1EC];
    grGimmick::AttackData m_attackData; // 0x1EC
};

class grAdventureMoveAttackTriggerOnce : public grAdventureMoveAttackGround {
protected:
    int m_state; // 0x168, HYPOTHESIS: 0 standby, 1 start, 2 moving, 3 stopped

public:
    virtual ~grAdventureMoveAttackTriggerOnce();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);

    static grAdventureMoveAttackTriggerOnce* create(int mdlIndex, const char* taskName);

    void frowStandby(float deltaFrame);
    void frowStart(float deltaFrame);
    void frowMoving(float deltaFrame);
    void frowStopped(float deltaFrame);
};
static_assert(sizeof(grAdventureMoveAttackTriggerOnce) == 0x16C, "Class is wrong size!");
