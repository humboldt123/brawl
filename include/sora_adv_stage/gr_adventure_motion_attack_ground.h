#pragma once

#include <StaticAssert.h>
#include <gr/gr_adventure_motion_ground.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<31grGimmickMotionAttackGroundData, ...>);
// the motion-ground record followed by the attack that the ground sets.
struct grGimmickMotionAttackGroundData {
    grGimmickMotionGroundData m_motionGroundData; // 0x0
    grGimmick::AttackData m_attackData;           // 0x1E4
};

// The animated ground that attacks what it touches (sora_adv_stage gr_adventure_motion_attack_ground.o).
class grAdventureMotionAttackGround : public grAdventureMotionGround {
protected:
    grGimmickMotionAttackGroundData* m_attackGroundData; // 0x154
    u32 unk158;                                          // 0x158
    u32 unk15C;                                          // 0x15C

public:
    grAdventureMotionAttackGround(const char* taskName);
    virtual ~grAdventureMotionAttackGround();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setValid(u32 unk1);
    virtual void setValidAttachedEffect(u32 unk1);
    virtual void setMotionAttack();

    static grAdventureMotionAttackGround* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureMotionAttackGround) == 0x160, "Class is wrong size!");
