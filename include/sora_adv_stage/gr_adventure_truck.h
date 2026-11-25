#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<18grGimmickTruckData, ...>); only the
// trigger that setInitializeFlag reads is named.
struct grGimmickTruckData {
    union {
        u8 unk0[0x1E0];
        struct {
            float unk0_float;
            u8 unk4[0x1DC];
        };
#ifdef ST_ADVENTURE2_IMPL
        struct {
            u8 _pad44[0x44];
            u8 m_mdlIndex;
        };
#endif
    };
    stTriggerData m_validTrigger; // 0x1E0
    u8 unk1E4[8];
    grGimmick::AttackData unk1EC;
    u8 unk244[0x2C];
    stTriggerData unk270;
    Vec2f unk274;
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
    virtual void setVisibilityByClipping(u32 visible);
    virtual void setVisibilityAttachedEffect(u32 visible);
    void startTruck();
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);

    static grAdventureTruck* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureTruck) == 0x1C4, "Class is wrong size!");
