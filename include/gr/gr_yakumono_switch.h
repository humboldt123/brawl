#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the gimmick data the switch reads (offsets from grYakumonoSwitch::startup/setOff/setInitializeFlag).
struct grYakumonoSwitchData {
    Vec2f m_pos;
    float m_rotZ;
    float m_motionRatio;
    char unk10[4];
    float m_offMotionRatioScale;
    char unk18[4];
    grGimmickMotionPathData m_motionPathData;
    char unk24[4];
    stTriggerData m_motionPathTriggerData;
    stTriggerData m_isValidTriggerData;
    char unk30[4];
    float m_difficultyMotionRatios[15];
};
static_assert(sizeof(grYakumonoSwitchData) == 0x70, "Class is wrong size!");

class grYakumonoSwitch : public grYakumono {
protected:
    // HYPOTHESIS: points at the switch trigger's flag (stTrigger::m_isValid in sora_adv_stage's caller).
    bool* m_isOn;
    int m_state;
    float m_158;
    grYakumonoSwitchData* m_switchData;

public:
    // HYPOTHESIS: parameter types from the sora_adv_stage caller and the stores to the members.
    grYakumonoSwitch(bool* isOn, const char* taskName);
    virtual ~grYakumonoSwitch();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setInitializeFlag();
    virtual void setSimpleEffectVisibleProduction();
    virtual void judgeTriggerState();
    // HYPOTHESIS: the argument is tested with a signed compare; nonzero plays the switch sound and effect.
    virtual void setOn(int withEffect);
    virtual void setOff(int withEffect);
};
static_assert(sizeof(grYakumonoSwitch) == 0x160, "Class is wrong size!");
