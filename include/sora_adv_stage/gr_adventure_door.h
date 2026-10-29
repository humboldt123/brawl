#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <st/st_trigger.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map (createGimmickFunc<17grGimmickDoorData, ...>); only the
// trigger that setInitializeFlag reads is named.
struct grGimmickDoorData {
    u8 unk0[0x50];
    stTriggerData m_validTrigger; // 0x50
};

// HYPOTHESIS: the adventure door (sora_adv_stage gr_adventure_door.o); most of its members are not described yet.
class grAdventureDoor : public grYakumono {
protected:
    u8 unk150[0x10];           // 0x150
    grGimmickDoorData* m_data; // 0x160
    u8 unk164[0x3C];           // 0x164

public:
    virtual ~grAdventureDoor();
    virtual void setInitializeFlag();
};
static_assert(sizeof(grAdventureDoor) == 0x1A0, "Class is wrong size!");
