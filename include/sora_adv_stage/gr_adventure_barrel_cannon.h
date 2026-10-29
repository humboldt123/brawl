#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <types.h>

// HYPOTHESIS: the adventure barrel cannon (sora_adv_stage gr_adventure_barrel_cannon.o); only the members its constructor sets
// are listed.
class grAdventureBarrelCannon : public grYakumono {
protected:
    u8 unk150[0xC];  // 0x150
    u32 unk15C;      // 0x15C
    u32 unk160;      // 0x160
    u8 unk164;       // 0x164, HYPOTHESIS: the cannon kind that create stores
    u8 unk165[7];    // 0x165
    u8 unk16C;       // 0x16C
    u8 unk16D;       // 0x16D
    u8 unk16E;       // 0x16E
    float unk170;    // 0x170
    u8 unk174;       // 0x174
    u8 unk175;       // 0x175
    int unk178;      // 0x178
    float unk17C;    // 0x17C
    u8 unk180;       // 0x180
    u8 unk181;       // 0x181
    int unk184;      // 0x184
    float unk188;    // 0x188
    u8 unk18C;       // 0x18C
    u8 unk18D;       // 0x18D
    int unk190;      // 0x190
    float unk194;    // 0x194
    u8 unk198;       // 0x198
    u8 unk199;       // 0x199
    int unk19C;      // 0x19C
    float unk1A0;    // 0x1A0
    u8 unk1A4;       // 0x1A4
    float unk1A8;    // 0x1A8
    int unk1AC;      // 0x1AC
    int unk1B0;      // 0x1B0
    u8 unk1B4[0x2C]; // 0x1B4
    u32 unk1E0;      // 0x1E0
    u32 unk1E4;      // 0x1E4
    u32 unk1E8;      // 0x1E8

public:
    grAdventureBarrelCannon(const char* taskName);
    virtual ~grAdventureBarrelCannon();
};
static_assert(sizeof(grAdventureBarrelCannon) == 0x1EC, "Class is wrong size!");
