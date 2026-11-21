#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the gimmick record of the moving ground (field names follow grGimmickMotionGroundData): the motion path data, an
// animation speed per difficulty level, and the sound/effect slots of the default (0-2), moving (3-7) and end (8-10) phases.
struct grAdventureMoveGroundData {
    grGimmickMotionPathData m_motionPathData;       // 0x0
    float m_motionRatios[15];                       // 0x8
    u8 unk44[4];
    grGimmick::SimpleEffectData m_defaultSE[3];     // 0x48
    grGimmick::SimpleEffectData m_defaultEffect[3]; // 0x6C
    u8 unk90[0x90];
    grGimmick::SimpleEffectData m_moveSE[5];        // 0x120
    grGimmick::SimpleEffectData m_moveEffect[5];    // 0x15C
    grGimmick::SimpleEffectData m_endSE[3];         // 0x198
    grGimmick::SimpleEffectData m_endEffect[3];     // 0x1BC
    stTriggerData m_isValidTrigger;                 // 0x1E0
    stTriggerData m_motionTrigger;                  // 0x1E4
    stTriggerData m_landingTrigger;                 // 0x1E8
};
static_assert(sizeof(grAdventureMoveGroundData) == 0x1EC, "Class is wrong size!");

// HYPOTHESIS: The moving-ground base of the ring lift (sora_melee gr_adventure_move_ground.o, not decompiled yet). Only the
// layout and the virtual the derived gimmicks override are declared here.
class grAdventureMoveGround : public grYakumono {
protected:
    grAdventureMoveGroundData* unk150; // 0x150, HYPOTHESIS: the gimmick record; the derived gimmicks set its motion speed per difficulty level

public:
    static grAdventureMoveGround* create(int mdlIndex, const char* taskName);
    grAdventureMoveGround(const char* taskName);
    virtual ~grAdventureMoveGround();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual void createMoveGroundMotionPath();
    virtual void setInitializeFlag();
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);

    void changeMotionPathRandomSE(u32 range);
};
static_assert(sizeof(grAdventureMoveGround) == 0x154, "Class is wrong size!");
