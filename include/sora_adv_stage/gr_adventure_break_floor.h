#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<23grGimmickBreakFloorData, ...>); only the
// two triggers that the floor reads and sets are placed.
struct grGimmickBreakFloorData {
    u8 unk0[0x19C];
    stTriggerData m_validTrigger; // 0x19C
    stTriggerData m_breakTrigger; // 0x1A0
};

// The floor of the Subspace stages that breaks after a fighter lands on it (sora_adv_stage gr_adventure_break_floor.o).
class grAdventureBreakFloor : public grGimmick {
protected:
    grGimmickBreakFloorData* m_data; // 0x14C
    u8 m_isLanded;                   // 0x150, set when a fighter lands
    float m_timer;                   // 0x154, HYPOTHESIS: counts the frames after the landing
    u32 unk158;                      // 0x158

public:
    grAdventureBreakFloor(const char* taskName) : grGimmick(taskName) {
        m_data = NULL;
        m_isLanded = 0;
        m_timer = 0.0f;
    }
    virtual ~grAdventureBreakFloor();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setInitializeFlag();

    static grAdventureBreakFloor* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureBreakFloor) == 0x15C, "Class is wrong size!");
