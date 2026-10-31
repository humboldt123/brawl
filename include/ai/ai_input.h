#pragma once

#include <ai/ai_mgr.h>
#include <ai/ai_param.h>
#include <ip/ip_human.h>
#include <sr/sr_common.h>
#include <types.h>

class aiAct;

// One row of the AI system parameter table, indexed by CPU type (HYPOTHESIS layout).
struct aiCpuTypePar {
    u16 m_unk0;
    u8 m_unk2;
    u8 pad3;
};

struct aiSystemPar {
    char unk0[0x1D];
    // BYTES: read as lbz then extsb into the argument register (an unsigned byte converted to s8 at the use); an s8
    // member loads straight into the argument register.
    u8 m_childRank;        // 0x1D: the rank a sub-character (Nana) controller starts with (HYPOTHESIS)
    char unk1E[2];
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
    aiChrIdx m_swingChr;   // 0x41
    aiChrIdx m_chrIdx;     // 0x42
    aiChrIdx m_unk43;      // 0x43
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
    aiChrIdx m_target;     // 0x110: the character slot (aiChrIdx) the CPU is after; its own slot = none (HYPOTHESIS)
    u8 m_unk111;
    u8 m_unk112;
    u8 m_cpuType;          // 0x113
    u8 m_unk114;
    u8 unk115;
    u16 m_unk116;
    float m_unk118;        // 0x118

    // HYPOTHESIS: the parameter names follow how the constructor uses them; parent is the main character's controller
    // when this one controls the sub-character (Nana).
    aiInput(u32 contNo, int unk2C, int player, int ftKind, s8 state, aiInput* parent, Heaps::HeapType heap);
    virtual ~aiInput();
    virtual void update();
    virtual Vec2f getStickMain() const;
    virtual Vec2f getStickSub() const;
    virtual ipPadButton getButton() const;
    virtual ipPadTrigger getTrigger() const;
    virtual s32 getContNo() const;

    // Loads a fighter's AI pac: the parameter part goes to m_par and the script part to m_act. owned = the controller frees
    // the data when it is replaced; check = verify the parameter revision first.
    // HYPOTHESIS: parameter names follow how the function uses them (callers pass the archive from the fighter data provider).
    void pacRegist(void* data, int size, int owned, int check);
    void reset();
    void change_md(int mode, void* target, int unk1, int unk2);
    void changeFtKind(int ftKind);
    void setCpuType(int cpuType);
    void setEnable(u32 enable);
    void swingChkReset(int force);
    void swingChkSet(int hit);
    s8 get_fix_target(float* distance);
    // HYPOTHESIS: parameter names; frames <= 0 = pick the reaction delay at random from the AI parameters.
    void damageStart(soDamage* damage, s16 frames, int unk);
    // HYPOTHESIS: target = the character that was hit by this CPU's attack.
    void swingHit(aiChrIdx* target, soDamage* damage);
};
