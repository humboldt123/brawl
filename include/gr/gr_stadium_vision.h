#pragma once

#include <StaticAssert.h>
#include <gr/gr_seq_yakumono.h>
#include <types.h>

// The big screen of Pokemon Stadium 2 ("stadium vision"). Only the part the stage reaches into is described; the class
// lives in sora_melee (create at 0x27AAF8, setDisplay at 0x27ADF8).
// HYPOTHESIS: the base class and every field name below; the offsets are the ones stDxPStadium writes/reads.
class grStadiumVision : public grSeqYakumono {
public:
    void* m_miscData;           // 0x15C: archive "Misc" entry 0x28 (set by the stage)
    char _160[4];               // 0x160
    int m_screenKind;           // 0x164: which picture the screen shows (0 none, 4 normal, 8 grass, 9 fire, 0xA water, 0xB rock)
    u8 m_screenMode;            // 0x168: 0xD / 0xE for the two "player view" modes, otherwise the vision kind
    char _169[0xB];             // 0x169
    float m_unk174;             // 0x174
    float m_unk178;             // 0x178
    float m_unk17C;             // 0x17C
    float m_unk180;             // 0x180
    float m_unk184;             // 0x184
    float m_unk188;             // 0x188
    float m_unk18C;             // 0x18C
    s16 m_unk190;               // 0x190
    s16 m_unk192;               // 0x192
    s16 m_unk194;               // 0x194
    s16 m_unk196;               // 0x196
    char _198[2];               // 0x198
    s16 m_unk19A;               // 0x19A
    s16 m_unk19C;               // 0x19C

    static grStadiumVision* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    void setDisplay(bool display);
};
