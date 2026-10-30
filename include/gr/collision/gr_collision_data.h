#pragma once

// Local BrawlHeaders shadow: names the fields of grCollData::JointData that the joint constructor reads.

#include <StaticAssert.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_vector.h>
#include <types.h>

class grCollData {
public:
    struct VtxData {
        Vec2f m_pos;
    };

    // Layout read by the grCollisionJoint constructor (same record as grCollConvJointData).
    struct JointData {
        u16 m_lineStart; // +0x00
        u16 m_lineLen; // +0x02
        u16 m_unk4;
        u16 m_unk6;
        u16 m_unk8;
        u16 m_unkA;
        u16 m_unkC;
        u16 m_unkE;
        u16 m_flags; // +0x10, HYPOTHESIS: bit 0 clear enables the joint; bits 1 and 2 are copied into the joint flags
        char _spacer12[2];
        clAABBox2D m_aabBox; // +0x14
        u16 m_vtxStart; // +0x24
        u16 m_vtxLen; // +0x26
        u16 m_unk28;
        u16 m_nodeIndex; // +0x2A
        char _spacer2C[0x20];
        const char m_name[0x20]; // +0x4C
    };
    static_assert(sizeof(JointData) == 0x6C, "Class is wrong size!");

    struct LineData {
        short m_pointX;
        short m_pointY;
        char _spacer[12];
    };

    u16 m_vtxLen;
    short m_lineLen;
    short m_jointLen;
    short m_unk1;
    VtxData* m_vtxDatas;
    LineData* m_lineDatas;
    JointData* m_jointDatas;
    u8 unk14[0x14];

    const char* getJointNodeName(u32 nodeIndex);
};
static_assert(sizeof(grCollData) == 0x28, "Class is wrong size!");
