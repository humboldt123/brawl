#pragma once

#include <ai/ai_mgr.h>
#include <ai/ai_stat.h>
#include <types.h>

// One attack the AI knows about (entry of the ATKD block); HYPOTHESIS: 0x18 bytes, id first.
struct tag_AI_ATK_DATA {
    u16 m_id;
    char unk2[0x16];
};

// ATKD block header followed by its entries.
struct tag_AI_ATK_DATA_HED {
    char unk0[4];
    int m_num;
    char unk8[8];
    tag_AI_ATK_DATA m_data[1];

    tag_AI_ATK_DATA* searchAtkData(int id); // HYPOTHESIS: int (the id is compared signed)
};

// Per-character AI parameter block loaded from the AIPD / ATKD data files.
// HYPOTHESIS: only the command bit masks the aiParam::chkCmd* helpers read are named.
struct aiParData {
    char unk0[0x160];
    u32 m_fallCmds;     // 0x160
    char unk164[4];
    u32 m_reviseCmds;   // 0x168
    u32 m_reflectCmds;  // 0x16C
    int m_actList[24];  // 0x170: offset of each action list (tag_AI_PAR_ACT_LIST entries ending with cmd 0) from the start of the block, 0 = none
    char unk1D0[0x10];
    int m_situationList[1]; // 0x1E0: offset of each situation entry from the start of the block; HYPOTHESIS: count unknown
};

// One entry of an action list; HYPOTHESIS: the two weights are blended by CPU level (high at rank 100, low at rank 0).
struct tag_AI_PAR_ACT_LIST {
    u8 m_cmd;        // command number + 1, 0 ends the list
    u8 m_ratioHigh;
    u8 m_ratioLow;
};

// Jump parameters of one character (HYPOTHESIS: possibly a larger view of aiReferPar).
struct aiJumpPar {
    char unk0[0x1C];
    float m_jumpHigh;    // 0x1C
    float m_jumpHighSp;  // 0x20
    float m_jumpLength;  // 0x24
};

class aiParam {
public:
    enum {
        NumActLists = 24,
    };

    aiParData* m_par;    // 0x00
    char* m_atkData;     // 0x04: ATKD block, NULL if absent
    s8 m_rank;           // 0x08: CPU level, clamped to 0..100
    u32 m_disableCmds;   // 0x0C: commands the AI may not use (bit per command)
    float m_rankRate;    // 0x10: (rank - 20) / 2 clamped to 0..30

    aiParam();
    void setRank(int rank, u32 force, int update);
    void par_work_update();
    void init_par(aiParData* par, int kind, char* atkData);
    bool chkCmdRevise(int cmd);
    bool chkCmdFall(int cmd);
    bool chkCmdReflect(int cmd);
    void set_disable_cmd(int cmd);
    u16 enum_act_list_id(int list, int n); // HYPOTHESIS: the n-th enabled action of an action list (0 = none)
    int select_situation(int start, int end, aiChrIdx* chr, aiChrIdx* target); // HYPOTHESIS: parameter types from chk_situation's use

    static bool check_revision(char* data);
    static float get_down_atk_r(aiChrIdx* chr);
    static u8 get_down_atk_f(aiChrIdx* chr);
    static float get_cliff_atk_r(aiChrIdx* chr);
    static u8 get_cliff_atk_f(aiChrIdx* chr);
    static float get_width(aiChrIdx* chr);
    static float get_height(aiChrIdx* chr);
    static float get_air_high(aiChrIdx* chr);
    static float get_air_length(aiChrIdx* chr);
    static float get_jump_high(aiJumpPar* par, aiStat* stat);
    static float get_jump_high_sp(aiJumpPar* par, aiStat* stat);
    static float get_jump_length(aiJumpPar* par, aiStat* stat);
};

int get_motnum_chr(ftKind kind); // HYPOTHESIS: int return
