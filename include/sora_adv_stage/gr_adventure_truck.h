#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<18grGimmickTruckData, ...>); only the
// trigger that setInitializeFlag reads is named.
struct grGimmickTruckData {
    u8 unk0[0x1E0];
    stTriggerData m_validTrigger; // 0x1E0
};

// HYPOTHESIS: the adventure mine truck (sora_adv_stage gr_adventure_truck.o); only the members its constructor clears are listed.
class grAdventureTruck : public grYakumono {
protected:
    grGimmickTruckData* m_data; // 0x150
    u32 unk154;      // 0x154, HYPOTHESIS: the scene model index used for the attached effect
    u8 unk158;       // 0x158
    float unk15C;    // 0x15C
    u8 unk160[0xC];  // 0x160
    float unk16C;    // 0x16C
    u8 unk170;       // 0x170
    u8 unk171[0x43]; // 0x171
    u32 unk1B4;      // 0x1B4
    u32 unk1B8;      // 0x1B8
    u32 unk1BC;      // 0x1BC
    u8 unk1C0;       // 0x1C0

public:
    grAdventureTruck(const char* taskName) : grYakumono(taskName) {
        m_data = NULL;
        unk154 = 0;
        unk158 = 0;
        unk15C = 0.0f;
        unk16C = 1.0f;
        unk170 = 1;
        unk1B4 = 0;
        unk1B8 = 0;
        unk1C0 = 0;
        m_transparencySettings = 0;
    }
    virtual ~grAdventureTruck();
    virtual void setInitializeFlag();

    static grAdventureTruck* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureTruck) == 0x1C4, "Class is wrong size!");
