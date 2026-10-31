#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <types.h>

class aiInput;
class aiStat;
struct AiClBlock;

// One command of an AI script: a type byte and the size of the command in bytes (the next command follows it).
// HYPOTHESIS: layout from tag_AI_SCRIPT_CMD::if_skip only; types 6/7 open a conditional block, 8 is its else, 9 its end.
struct tag_AI_SCRIPT_CMD {
    u8 m_type;   // 0x0
    u8 unk1;
    u16 m_size;  // 0x2

    tag_AI_SCRIPT_CMD* if_skip();
};

// One AI script: an id, then its commands.
struct tag_AI_SCRIPT {
    u32 m_id;    // 0x0
};

// A pack of AI scripts (a fighter's own pack or the common one): the scripts are found through offsets from the pack.
// HYPOTHESIS: layout from tag_AI_SCRIPT_PACK::get_script only.
struct tag_AI_SCRIPT_PACK {
    u32 unk0;
    u32 m_numScripts;  // 0x4
    char unk8[8];
    u32 m_offsets[1];  // 0x10: m_numScripts offsets of the scripts from the start of the pack

    // HYPOTHESIS: looks in this pack first, then in common; an id with none of the bits 0xFFFF800F set (or one that
    // is not found) is looked up again by its 0x7FF0 bits.
    tag_AI_SCRIPT* get_script(tag_AI_SCRIPT_PACK* common, u32 id);
};

// The CPU's action selector: picks an action id from the loaded AI script and tracks per-action cooldowns.
// HYPOTHESIS: layout recovered from the small accessors only; size from aiInput's allocation.
class aiAct {
public:
    // HYPOTHESIS: action ids; 0x6031.. and 0x6034.. are the command ranges ai_act_id2cmd maps.
    enum actId {};

    float m_unk0[24];      // 0x00
    void* m_script;        // 0x60: loaded AI script data
    void* m_unk64;         // 0x64
    void* m_unk68;         // 0x68
    void* m_unk6C;         // 0x6C
    aiInput* m_input;      // 0x70
    aiStat* m_stat;        // 0x74: the aiStat of the controlled character
    u16 m_id;              // 0x78: action id being run
    u16 m_unk7A;
    u16 m_prevId;          // 0x7C: previous non-zero action id
    u16 m_unk7E;
    u8 m_unk80;
    u8 m_unk81;
    u16 m_unk82;
    float m_unk84;
    float m_unk88;
    float m_unk8C;
    u8 m_unk90;
    u8 m_unk91;
    u8 m_unk92;
    u8 m_unk93;
    int m_unk94;
    float m_unk98;         // 0x98: -1 when reset
    u16 m_reflectDone;     // 0x9C: frames the reflect-done penalty lasts
    float m_reflectRate;   // 0xA0
    float m_defendAirRate; // 0xA4
    int m_unkA8;
    int m_unkAC;
    Vec3f m_unkB0;
    Vec3f m_unkBC;
    Vec3f m_unkC8;
    u16 m_unkD4[4];
    float m_unkDC;         // 0xDC: 1 when reset
    // The goal that set_goal_inf records: a position and the floor blocks of the goal and of the way there.
    Vec2f m_goalPos;       // 0xE0
    Vec2f m_goalEdgeL;     // 0xE8: the goal block's walkable ends
    Vec2f m_goalEdgeR;     // 0xF0
    u32 m_goalJointId;     // 0xF8: the goal block's joint (tag_AiClJointList::m_id), 0 = static floor or none
    int m_goalUnk48;       // 0xFC: the goal block's AiClBlock::m_unk48
    u32 m_viaJointId;      // 0x100: the same for the block on the way
    int m_viaUnk48;        // 0x104
    int m_unk108;
    int m_unk10C;
    float m_unk110;

    aiAct(aiInput* input, aiStat* stat);
    ~aiAct();
    void act_change(int id, void* target, int unk1, int unk2);
    void script_reload(void* script);
    void set_id(u16 id);
    // HYPOTHESIS: the parameter order; the blocks come in r4/r5 and the position in f1/f2.
    void set_goal_inf(AiClBlock* goal, AiClBlock* via, float x, float y);
    void setReflectDone();
    void addDefendAirRate(float amount);
};
static_assert(sizeof(aiAct) == 0x114, "Class is wrong size!");

int ai_act_id2cmd(aiAct::actId id);
