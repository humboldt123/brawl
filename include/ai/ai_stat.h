#pragma once

#include <ai/ai_mgr.h>
#include <mt/mt_vector.h>
#include <types.h>

// One CPU character's view of the match (position, speed, floor, transition flags); 0x1C4 bytes, one per aiChrIdx.
// HYPOTHESIS: only the offsets used so far are named, the rest is still unknown.
class aiStat {
public:
    aiChrIdx m_chrIdx;       // 0x00: -1 = unused
    char unk1[0x4B];
    int m_input;             // 0x4C: the aiInput driving this character (0 for humans)
    u32 m_flags;             // 0x50
    u32 m_transition;        // 0x54: transition bits (chk_transition)
    u32 m_chrStat;           // 0x58: character state bits (chk_chr_stat)
    Vec3f m_pos;             // 0x5C
    float m_scale;           // 0x68: multiplied by the character width in aiParam::get_width
    char unk6C[0x14];
    float m_speedX;          // 0x80
    float m_speedY;          // 0x84
    float m_accelX;          // 0x88
    float m_accelY;          // 0x8C
    char unk90[0x10];
    float m_lr;              // 0xA0: facing, +1 right / -1 left
    char unkA4[8];
    int m_chrKind;           // 0xAC
    char unkB0[0xC];
    struct aiFloorInfo* m_floor; // 0xBC
    u8 m_unkC0;              // 0xC0
    char unkC1[0xF9];
    u8 m_situation;          // 0x1BA: 1 = ground, 2 = air, 6/7 = special
    char unk1BB[9];

    int chk_over_ground(); // HYPOTHESIS: int (the xori result is returned as-is)
    bool chk_stat_gr_free();
    bool chk_stat_air_free();
    bool chk_transition(u32 mask);
    bool chk_chr_stat(u32 mask, int chrKind);
    int chk_over_ground1(aiChrIdx* target);
    bool chk_near_deadarea();
    Vec2f getNearFloorEdge();
    bool chk_stat_fpass();
    bool chk_stat_ffall();
    float get_lr_tgt(aiChrIdx* target);
    void change(int chrIdx, int chrKind, int input, u8 unk);

    float calcArraiveFrameX(float dist);
    float calcArraiveFrameY(float dist);
    float calcArraivePosX(float frame);
    float calcArraivePosY(float frame);
};
static_assert(sizeof(aiStat) == 0x1C4, "Class is wrong size!");

// A floor segment the AI can stand on (HYPOTHESIS layout: +0x40 flag word).
struct aiFloorInfo {
    char unk0[0x18];
    Vec2f m_edgeL;  // 0x18
    Vec2f m_edgeR;  // 0x20
    char unk28[0x18];
    u32 m_flags;
};

float calc_arraive_f(float dist, float vel, float acc);

namespace aiParam {
float get_width(aiChrIdx* chr);
}
