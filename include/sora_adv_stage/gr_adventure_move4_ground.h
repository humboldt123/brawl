#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick_motion_path.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<24grGimmickMove4GroundData, ...>); only the fields its recovered methods read are named.
struct grGimmickMove4GroundData {
    float m_motionSpeed;        // 0x0, HYPOTHESIS: the frame speed of a resumed path
    u8 unk4[0x20C];
    stTriggerData m_validTrigger;    // 0x210
    stTriggerData m_pathTriggers[4]; // 0x214, HYPOTHESIS: a set trigger puts its path at its end at startup
    stTriggerData m_landTrigger;     // 0x224, HYPOTHESIS: set when a fighter lands on the ground
};

// HYPOTHESIS: the adventure ground that moves along one of four motion paths (sora_adv_stage gr_adventure_move4_ground.o).
class grAdventureMove4Ground : public grYakumono {
protected:
    grGimmickMotionPath* m_motionPaths[4]; // 0x150
    grGimmickMove4GroundData* m_data;      // 0x160

public:
    grAdventureMove4Ground(const char* taskName) : grYakumono(taskName) {
        m_data = NULL;
        unkF8 = 100.0f;
        for (int i = 0; i < 4; i++) {
            m_motionPaths[i] = NULL;
        }
    }
    virtual ~grAdventureMove4Ground();
    virtual void update(float deltaFrame);
    virtual void setVisibility(u32 unk1);
    virtual void setInitializeFlag();
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);

    static grAdventureMove4Ground* create(int mdlIndex, const char* taskName);

    void changePath(u8 index);
    void stopPath(u8 index);
};
static_assert(sizeof(grAdventureMove4Ground) == 0x164, "Class is wrong size!");
