#pragma once

#include <StaticAssert.h>
#include <types.h>

class aiInput;
class aiStat;

// The CPU's action selector: picks an action id from the loaded AI script and tracks per-action cooldowns.
// HYPOTHESIS: layout recovered from the small accessors only; size from aiInput's allocation.
class aiAct {
public:
    char unk0[0x60];
    void* m_script;      // 0x60: loaded AI script data
    char unk64[0xC];
    aiInput* m_input;    // 0x70
    char unk74[4];
    u16 m_id;            // 0x78: action id being run
    char unk7A[2];
    u16 m_prevId;        // 0x7C: previous non-zero action id
    char unk7E[0x1E];
    u16 m_reflectDone;   // 0x9C: frames the reflect-done penalty lasts
    float m_reflectRate; // 0xA0
    float m_defendAirRate; // 0xA4
    char unkA8[0x6C];

    ~aiAct();
    void act_change(int id, void* target, int unk1, int unk2);
    void script_reload(void* script);
    void set_id(u16 id);
    void setReflectDone();
    void addDefendAirRate(float amount);
};
static_assert(sizeof(aiAct) == 0x114, "Class is wrong size!");
