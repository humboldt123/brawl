#pragma once

#include <StaticAssert.h>
#include <gr/gr_adventure_motion_ground.h>
#include <mt/mt_vector.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<31grGimmickMotionGroundAttachData, ...>);
// the motion-ground record followed by the placement that startup applies.
struct grGimmickMotionGroundAttachData {
    grGimmickMotionGroundData m_motionGroundData; // 0x0
    Vec3f m_pos;                                  // 0x1E4
    Vec3f m_rot;                                  // 0x1F0
};

class grAdventureMotionGroundAttach : public grAdventureMotionGround {
protected:
    grGimmickMotionGroundAttachData* m_attachData; // 0x154
    Vec2f unk158;                                  // 0x158

public:
    grAdventureMotionGroundAttach(const char* taskName) : grAdventureMotionGround(taskName), unk158(Vec2f(0.0f, 0.0f)) { }
    virtual ~grAdventureMotionGroundAttach();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setInitializeFlag();

    static grAdventureMotionGroundAttach* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureMotionGroundAttach) == 0x160, "Class is wrong size!");
