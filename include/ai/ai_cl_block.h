#pragma once

#include <StaticAssert.h>
#include <gr/collision/gr_collision_handle.h>
#include <mt/mt_vector.h>
#include <types.h>

// One collision joint (a moving part of the stage) whose floor blocks the CPU AI tracks; aiMgr keeps an array of these.
// HYPOTHESIS: layout recovered from the constructor and the aiMgr block searches only.
struct tag_AiClJointList {
    char unk0[8];
    grCollisionHandle m_collision; // 0x8: the joint's collision
    char unk10[0xC];
    u32 m_id;                      // 0x1C: compared with the first word of a block search key

    tag_AiClJointList();
};
static_assert(sizeof(tag_AiClJointList) == 0x20, "Class is wrong size!");

// One floor segment of the stage as the CPU AI sees it (the AI keeps an array of these in aiMgr).
// HYPOTHESIS: layout recovered from the aiMgr block searches only.
struct AiClBlock {
    char unk0[0x18];
    Vec2f m_edgeL;      // 0x18: left end of the walkable surface
    Vec2f m_edgeR;      // 0x20: right end
    float m_top;        // 0x28: highest point of the block
    float m_bottom;     // 0x2C: lowest point of the block
    char unk30[0xE];
    u8 m_teamMask;      // 0x3E: teams (bit per team) the danger flag applies to
    u8 unk3F;
    u32 m_flags;        // 0x40: 2 = usable, 0x20 = pass-through, 0x800 = hazard for m_teamMask, 0x1000 / 0x10000 = worse hazards
    tag_AiClJointList* m_joint; // 0x44: collision joint the block belongs to (NULL = static floor)
    int m_unk48;        // 0x48

    AiClBlock();
    bool chk_fl_danger(int team);
};
static_assert(sizeof(AiClBlock) == 0x4C, "Class is wrong size!");
