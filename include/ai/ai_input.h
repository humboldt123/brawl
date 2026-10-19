#pragma once

#include <ai/ai_mgr.h>
#include <ai/ai_param.h>
#include <ip/ip_human.h>
#include <types.h>

class aiAct;

// One row of the AI system parameter table, indexed by CPU type (HYPOTHESIS layout).
struct aiCpuTypePar {
    u16 m_unk0;
    u8 m_unk2;
    u8 pad3;
};

struct aiSystemPar {
    char unk0[0x20];
    aiCpuTypePar m_cpuType[25];
};

// The CPU controller of one fighter slot: it fills in an IpHuman-style input each frame from the AI's chosen mode.
// HYPOTHESIS: layout recovered from the constructor, reset and a few small accessors only.
class aiInput : public IpHuman {
public:
    int m_unk2C;           // 0x2C
    int m_ftKind;          // 0x30: fighter kind whose AI script is loaded
    aiInput* m_child;      // 0x34: the sub-character (Nana) controller of this controller
    void* m_pacData;       // 0x38: loaded AI pac, freed when owned (flag 0x40)
    u16 m_flags;           // 0x3C: 0x8000 = enabled, 0x40 = m_pacData is owned
    u8 m_unk3E;
    u8 m_unk3F;
    u8 m_swingState;       // 0x40
    s8 m_swingChr;         // 0x41
    aiChrIdx m_chrIdx;     // 0x42
    u8 m_unk43;
    aiAct* m_act;          // 0x44
    int m_md;              // 0x48: current mode (md_* index)
    int m_mdNext;          // 0x4C
    int m_unk50;
    int m_unk54;
    u16 m_unk58;
    u16 m_unk5A;
    u16 m_unk5C;
    u16 m_unk5E;
    u8 m_unk60;
    u8 m_unk61;
    u8 m_unk62;
    u8 m_unk63;
    u8 m_unk64;
    u8 m_unk65;
    u8 m_unk66;
    s8 m_unk67;
    u16 m_unk68;
    u16 m_unk6A;
    int m_unk6C;
    int m_unk70;
    u8 m_unk74;
    u8 m_unk75;
    u8 m_unk76;
    u8 m_unk77;
    float m_unk78;
    float m_unk7C;
    float m_unk80;
    int m_unk84;
    u16 m_unk88;
    u16 m_unk8A;
    float m_unk8C;
    aiParam m_par;         // 0x90
    char unkA4[0x6C];
    u8 m_unk110;           // 0x110
    u8 m_unk111;
    u8 m_unk112;
    u8 m_cpuType;          // 0x113
    u8 m_unk114;
    u8 unk115;
    u16 m_unk116;
    float m_unk118;        // 0x118

    void reset();
    void change_md(int mode, void* target, int unk1, int unk2);
    void changeFtKind(int ftKind);
    void setCpuType(int cpuType);
    void setEnable(u32 enable);
    void swingChkReset(int force);
    void swingChkSet(int hit);
};
