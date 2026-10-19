#pragma once

#include <ai/ai_mgr.h>
#include <mt/mt_vector.h>
#include <types.h>

// One CPU character's view of the match (position, speed, floor, transition flags); 0x1C4 bytes, one per aiChrIdx.
// HYPOTHESIS: only the offsets used so far are named, the rest is still unknown.
class aiStat {
public:
    aiChrIdx m_chrIdx;       // 0x00: -1 = unused
    u8 m_unk1;               // 0x01
    u8 m_unk2;               // 0x02
    s8 m_enableAway;         // 0x03: chkEnableAway cache, -1 = not computed yet
    u8 m_unk4;               // 0x04
    char unk5[3];
    int m_unk8;              // 0x08
    char unkC[0x40];
    aiInput* m_input;        // 0x4C: the aiInput driving this character (0 for humans)
    u32 m_flags;             // 0x50
    u32 m_transition;        // 0x54: transition bits (chk_transition)
    u32 m_chrStat;           // 0x58: character state bits (chk_chr_stat)
    Vec3f m_pos;             // 0x5C
    Vec3f m_scale;           // 0x68: m_scale.m_x is multiplied by the character width in aiParam::get_width
    Vec3f m_center;          // 0x74: HYPOTHESIS: body centre used for distances
    Vec2f m_speed;           // 0x80
    Vec2f m_accel;           // 0x88
    Vec2f m_unk90;           // HYPOTHESIS: a Vec2f (zeroed through a temporary like speed/accel)
    Vec2f m_unk98;
    float m_lr;              // 0xA0: facing, +1 right / -1 left
    float m_unkA4;
    u16 m_unkA8;
    u16 m_unkAA;
    int m_chrKind;           // 0xAC
    int m_unkB0;
    int m_unkB4;
    int m_unkB8;
    struct aiFloorInfo* m_floor; // 0xBC
    u8 m_unkC0;              // 0xC0
    char unkC1[3];
    float m_unkC4;
    int m_team;              // 0xC8: HYPOTHESIS: team id passed to chk_fl_danger / aiMgr::chkDangerZone
    int m_unkCC;
    int m_unkD0;
    int m_unkD4;
    int m_unkD8;
    float m_unkDC;
    int m_unkE0;
    char unkE4[0xD5];
    u8 m_unk1B9;
    u8 m_situation;          // 0x1BA: 1 = ground, 2 = air, 6/7 = special
    u8 m_unk1BB;
    u8 m_unk1BC;
    u8 m_unk1BD;
    char unk1BE[6];

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
    void init(int chrIdx, int chrKind, aiInput* input, int unk);
    void change(int chrIdx, int chrKind, aiInput* input, int unk);
    void get_tgt_vector(float scale, Vec3f* out, aiChrIdx* target);
    float get_range(float scale, aiChrIdx* target);
    float get_distance(float scale, aiChrIdx* target);
    float get_range_floor(float scale, Vec3f* pos);
    bool chkDangerZoneAway();

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

bool chk_fl_danger(aiFloorInfo* floor, int team);
float calc_arraive_f(float dist, float vel, float acc);
