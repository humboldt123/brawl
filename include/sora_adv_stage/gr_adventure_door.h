#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<17grGimmickDoorData, ...>); only the
// trigger that setInitializeFlag reads is named.
struct grGimmickDoorData {
    union {
        u8 unk0[0x50];
        struct {
            u8 unk0Padding[0x37];
            u8 unk37;
            u8 unk38[0xC];
            u8 unk44;
            u8 unk45[3];
            int unk48;
            u8 unk4C[4];
        };
#ifdef ST_ADVENTURE2_IMPL
        struct {
            u8 _pad30[0x30];
            u32 m_jumpData;
            u8 _pad34[2];
            u8 m_mdlIndex;
        };
#endif
    };
    stTriggerData m_validTrigger; // 0x50
};

// HYPOTHESIS: the adventure door (sora_adv_stage gr_adventure_door.o); most of its members are not described yet.
class grAdventureDoor : public grYakumono {
protected:
    union {
        u8 unk150[0x10];           // 0x150
        struct {
            u8 unk150State;
            u8 unk151[3];
            float unk154;
            float unk158;
            float unk15C;
        };
    };
    grGimmickDoorData* m_data; // 0x160
    union {
        u8 unk164[0x3C];           // 0x164
        struct {
            u32 unk164Data;
            int unk168;
            u8 unk16C[0x28];
            int unk194;
            int unk198;
            u8 unk19C[4];
        };
    };

public:
    grAdventureDoor(const char* taskName);
    static grAdventureDoor* create(int mdlIndex, u32 jumpData, const char* taskName);
    virtual ~grAdventureDoor();
    virtual void setInitializeFlag();
    virtual void update(float deltaFrame);
    // HYPOTHESIS: jump data is the unsigned key searched by stAdventure2.
    virtual void setJumpData(u32 jumpData);
    virtual void openTheDoor();
    virtual void EachDoorTypeEffect();
};
static_assert(sizeof(grAdventureDoor) == 0x1A0, "Class is wrong size!");
