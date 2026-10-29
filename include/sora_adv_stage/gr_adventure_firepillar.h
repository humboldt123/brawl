#pragma once

#include <StaticAssert.h>
#include <sora_adv_stage/gr_adventure_motion_attack_ground.h>
#include <sora_adv_stage/gr_adventure_notify_gimmick_trigger_status.h>
#include <types.h>

// HYPOTHESIS: the adventure fire pillar, an attacking motion ground switched by a trigger (sora_adv_stage gr_adventure_firepillar.o).
class grAdventureFirePillar : public grAdventureMotionAttackGround, public grAdventureNotifyGimmickTriggerStatus {
protected:
    int m_state;      // 0x170, HYPOTHESIS: the frow* state, 3 at creation
    float unk174;     // 0x174
    float unk178;     // 0x178
    float unk17C;     // 0x17C
    int unk180;       // 0x180
    int unk184;       // 0x184
    u8 unk188;        // 0x188
    float unk18C;     // 0x18C
    float unk190;     // 0x190
    float unk194;     // 0x194
    float unk198;     // 0x198

public:
    grAdventureFirePillar(const char* taskName) : grAdventureMotionAttackGround(taskName) {
        m_state = 3;
        unk174 = 0.0f;
        unk178 = 0.0f;
        unk17C = 0.0f;
        unk180 = -1;
        unk184 = -1;
        unk188 = 0;
        unk18C = 5.0f;
        unk190 = 10.0f;
        unk194 = 0.15f;
        unk198 = 0.15f;
        m_transparencySettings = 0x1F;
    }
    virtual void updateStatus();
    virtual void notifyEventOnTrigger(int flag);
    virtual void notifyEventOffTrigger(int flag);
    virtual ~grAdventureFirePillar();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void dbDispInvalidatedByCameraClippingSphere();

    static grAdventureFirePillar* create(int mdlIndex, const char* taskName);

    void gimmickStop();
    void frowStarting(float deltaFrame);
    void frowStarted(float deltaFrame);
    void frowStopping(float deltaFrame);
    void frowStopped(float deltaFrame);
};
static_assert(sizeof(grAdventureFirePillar) == 0x19C, "Class is wrong size!");
