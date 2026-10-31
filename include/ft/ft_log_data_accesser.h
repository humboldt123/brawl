#pragma once

// Local copy of BrawlHeaders' ft/ft_log_data_accesser.h that shadows it (-I include comes first).
// The original only declares the size (0x24c); the member tables below come from the accessors'
// addressing in the sora_melee REL, so they can be written as real code.

#include <StaticAssert.h>
#include <types.h>

// HYPOTHESIS: field meanings unknown. The layout is the 117 sections that ftLogDataAccesser::setup binds: a run of
// floats and ints (some interleaved) followed by 20 flag bytes, with nothing between them.
struct ftLogData {
    float unk0; // 0x0
    float unk4; // 0x4
    int unk8; // 0x8
    float unkC; // 0xC
    float unk10; // 0x10
    float unk14; // 0x14
    float unk18[7]; // 0x18
    int unk34; // 0x34
    int unk38; // 0x38
    float unk3C; // 0x3C
    float unk40; // 0x40
    float unk44; // 0x44
    int unk48; // 0x48
    int unk4C; // 0x4C
    int unk50; // 0x50
    int unk54; // 0x54
    int unk58; // 0x58
    int unk5C; // 0x5C
    int unk60; // 0x60
    int unk64; // 0x64
    int unk68; // 0x68
    int unk6C; // 0x6C
    int unk70; // 0x70
    int unk74; // 0x74
    int unk78; // 0x78
    float unk7C; // 0x7C
    float unk80; // 0x80
    int unk84; // 0x84
    int unk88; // 0x88
    int unk8C; // 0x8C
    int unk90; // 0x90
    int unk94; // 0x94
    float unk98; // 0x98
    int unk9C; // 0x9C
    int unkA0; // 0xA0
    int unkA4; // 0xA4
    int unkA8; // 0xA8
    int unkAC; // 0xAC
    int unkB0; // 0xB0
    int unkB4; // 0xB4
    int unkB8; // 0xB8
    int unkBC; // 0xBC
    int unkC0; // 0xC0
    int unkC4; // 0xC4
    int unkC8; // 0xC8
    int unkCC; // 0xCC
    int unkD0; // 0xD0
    int unkD4; // 0xD4
    int unkD8; // 0xD8
    int unkDC; // 0xDC
    int unkE0; // 0xE0
    int unkE4; // 0xE4
    int unkE8; // 0xE8
    int unkEC; // 0xEC
    int unkF0; // 0xF0
    int unkF4; // 0xF4
    int unkF8; // 0xF8
    int unkFC; // 0xFC
    int unk100; // 0x100
    int unk104; // 0x104
    int unk108; // 0x108
    int unk10C; // 0x10C
    int unk110; // 0x110
    int unk114; // 0x114
    int unk118; // 0x118
    int unk11C; // 0x11C
    int unk120; // 0x120
    int unk124; // 0x124
    int unk128; // 0x128
    int unk12C; // 0x12C
    int unk130; // 0x130
    int unk134; // 0x134
    int unk138; // 0x138
    float unk13C; // 0x13C
    float unk140; // 0x140
    float unk144; // 0x144
    float unk148; // 0x148
    float unk14C; // 0x14C
    float unk150; // 0x150
    float unk154; // 0x154
    int unk158; // 0x158
    float unk15C; // 0x15C
    int unk160; // 0x160
    float unk164; // 0x164
    int unk168; // 0x168
    int unk16C; // 0x16C
    int unk170; // 0x170
    int unk174; // 0x174
    int unk178; // 0x178
    int unk17C; // 0x17C
    int unk180; // 0x180
    int unk184; // 0x184
    int unk188; // 0x188
    float unk18C; // 0x18C
    int unk190; // 0x190
    int unk194; // 0x194
    int unk198; // 0x198
    bool unk19C; // 0x19C
    bool unk19D; // 0x19D
    bool unk19E; // 0x19E
    bool unk19F; // 0x19F
    bool unk1A0; // 0x1A0
    bool unk1A1; // 0x1A1
    bool unk1A2; // 0x1A2
    bool unk1A3; // 0x1A3
    bool unk1A4; // 0x1A4
    bool unk1A5; // 0x1A5
    bool unk1A6; // 0x1A6
    bool unk1A7; // 0x1A7
    bool unk1A8; // 0x1A8
    bool unk1A9; // 0x1A9
    bool unk1AA; // 0x1AA
    bool unk1AB; // 0x1AB
    bool unk1AC; // 0x1AC
    bool unk1AD; // 0x1AD
    bool unk1AE; // 0x1AE
    bool unk1AF; // 0x1AF
};
static_assert(sizeof(ftLogData) == 0x1b0, "Class is wrong size!");

// Per-fighter match statistics, addressed as (category, slot). Each category is a pointer into the
// matching section of ftLogData (see setup()).
class ftLogDataAccesser {
    // The three tables are one run of 117 pointers (setup() binds each to a section of ftLogData);
    // m_count[i] is the number of elements behind pointer i, used by reset().
    // HYPOTHESIS: the table sizes (22 floats, 75 ints, 20 flags) follow from the pointer runs in the constructor.
    float* m_floatTable[22]; // 0x000
    int* m_intTable[75];     // 0x058
    bool* m_flagTable[20];   // 0x184
    u8 m_count[117];         // 0x1d4
public:
    ftLogDataAccesser();
    ~ftLogDataAccesser();

    void addFloat(float val, u32 category, u32 slot);
    void addInt(int val, u32 category, u32 slot);

    float getFloat(u32 category, u32 slot);
    int getInt(u32 category, u32 slot);
    bool isFlag(u32 category, u32 slot);

    void offFlag(u32 category, u32 slot);
    void onFlag(u32 category, u32 slot);
    void setFlag(bool val, u32 category, u32 slot);
    void setFloat(float val, u32 category, u32 slot);
    void setInt(int val, u32 category, u32 slot);

    void setup(ftLogData* logData);
    void reset();
};
static_assert(sizeof(ftLogDataAccesser) == 0x24c, "Class is wrong size!");
