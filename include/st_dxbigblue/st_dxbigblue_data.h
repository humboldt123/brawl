#pragma once

// The parameters of Big Blue (stage data file, 0x144 bytes). Nothing is named by the original, so the fields are the offsets.
// HYPOTHESIS: the roles in the comments are read off how the stage and the grounds use the values.
struct stDxBigBlueData {
    float unk00;    // 0x00 the x position the courses start at (the course in front of the camera)
    float unk04;
    float unk08;
    float unk0C;
    float unk10;
    float unk14;
    u32 unk18;
    float unk1C;
    float unk20;    // 0x20 the car must have this much room to the car in front before it goes on
    float unk24;    // 0x24 the top speed of a car
    float unk28;    // 0x28 the acceleration of a car
    float unk2C;
    float unk30;
    float unk34;
    float unk38;
    float unk3C;
    float unk40;
    float unk44;
    float unk48;    // 0x48 how fast the cars speed up in the other direction
    float unk4C;    // 0x4C the most they can speed up
    float unk50;    // 0x50 the speed the car's model turns at (when it is going backwards)
    float unk54;    // 0x54 the speed the car's model turns at (going forwards)
    float unk58;
    float unk5C;    // 0x5C the frames a car waits before it goes on (lowest)
    float unk60;    // 0x60 the frames a car waits before it goes on (highest)
    float unk64;
    float unk68;    // 0x68 how far outside of the camera limit things are removed
    float unk6C;    // 0x6C the speed the courses move at
    float unk70;
    float unk74;
    float unk78;
    float unk7C;
    float unk80;
    float unk84;
    float unk88;
    float unk8C;
    float unk90;
    float unk94;
    float unk98;
    float unk9C;
    float unkA0;
    float unkA4;
    float unkA8;
    float unkAC;
    float unkB0;
    float unkB4;
    float unkB8;
    float unkBC;
    float unkC0;
    float unkC4;
    float unkC8;
    float unkCC;
    float unkD0;
    float unkD4;
    float unkD8;
    u32 unkDC;
    u32 unkE0;
    float unkE4;
    float unkE8;
    float unkEC;
    float unkF0;
    float unkF4;
    float unkF8;
    float unkFC;
    float unk100;
    float unk104;
    float unk108;
    float unk10C;
    float unk110;
    float unk114;
    float unk118;
    u32 unk11C;
    u32 unk120;
    float unk124;
    float unk128;
    float unk12C;
    float unk130;
    float unk134;
    float unk138;
    float unk13C;
    float unk140;
};
static_assert(sizeof(stDxBigBlueData) == 0x144, "stDxBigBlueData size");
