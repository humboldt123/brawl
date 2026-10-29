#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <types.h>

// HYPOTHESIS: The switch base of the Subspace switches (sora_melee gr_yakumono_switch.o, not decompiled yet). Only the
// layout and the virtuals it adds to grYakumono are declared.
class grYakumonoSwitch : public grYakumono {
protected:
    u32 unk150;          // 0x150
    int m_switchState;   // 0x154, HYPOTHESIS: 1 while the switch is on
    float m_onTimer;     // 0x158
    void* m_switchData;  // 0x15C

public:
    virtual ~grYakumonoSwitch();
    virtual void judgeTriggerState();
    virtual void setOn(bool isTrigger);
    virtual void setOff(bool isTrigger);
};
static_assert(sizeof(grYakumonoSwitch) == 0x160, "Class is wrong size!");

// HYPOTHESIS: The type name is from the sora_adv_stage map (createGimmickFunc<25grGimmickAttackSwitchData, ...>); only the
// word whose address onDamage passes on is placed.
struct grGimmickAttackSwitchData {
    u8 unk0[0x70];
    u32 unk70;
};

class grAdventureAttackSwitch : public grYakumonoSwitch {
protected:
    float m_coolTime;                       // 0x160, HYPOTHESIS: the switch ignores damage while it is above 0
    grGimmickAttackSwitchData* m_data;      // 0x164
    u8 unk168[0x28];
    u32 unk190[5];                          // 0x190

public:
    virtual ~grAdventureAttackSwitch();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setOn(bool isTrigger);
    virtual void setOff(bool isTrigger);
};
static_assert(sizeof(grAdventureAttackSwitch) == 0x1A4, "Class is wrong size!");
