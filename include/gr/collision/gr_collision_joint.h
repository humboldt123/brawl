#pragma once

// Local BrawlHeaders shadow: adds the recovered grCollisionJoint::getCollisionId (same layout).

#include <StaticAssert.h>
#include <gr/collision/gr_collision_data.h>
#include <gr/collision/gr_collision_line.h>
#include <mt/mt_coll2d.h>
#include <mt/mt_matrix.h>
#include <types.h>

class grCollision;
class Ground;
struct grCollSegmentEx;

class grCollisionJoint {
public:
    // 0
    u16 m_lineStart; // role: index of the joint's first line in the collision's line array (grCollisionJoint::getLine adds it)
    // 2
    u16 m_lineLen; // HYPOTHESIS: number of lines in the joint (grCollisionLineWork bounds its link walks with it)
    // 4
    u16 m_unk4; // role: copied from the joint data by the constructor (m_unk4 to m_unkE)
    u16 m_unk6;
    u16 m_unk8;
    u16 m_unkA;
    u16 m_unkC;
    u16 m_unkE;
    // 16
    clAABBox2D m_aabBox;
    // 32
    clAABBox2D m_prevAabBox; // role: the box around the previous-frame vertex positions, built by updatePos
    // 48
    grCollision* m_collision;
    // 52
    grCollisionLine* m_lines;
    // 56
    grCollData::VtxData* m_vtxDatas;
    // 60
    Vec2f* m_vtxPositions; // role: the collision's work vertices, 16 bytes each (current position, then the previous one)
    // 64
    grCollData::VtxData* m_unk40; // HYPOTHESIS: the constructor sets these two from the line start index
    // 68
    Vec2f* m_unk44;
    // 72
    union {
    struct {
    bool m_0x48_31 : 1;
    bool m_0x48_30 : 1;
    bool m_0x48_29 : 1;
    bool m_0x48_28 : 1;
    bool m_0x48_27 : 1;
    bool m_0x48_26 : 1;
    bool m_0x48_25 : 1;
    bool m_0x48_24 : 1;
    bool m_0x48_23 : 1;
    bool m_0x48_22 : 1;
    bool m_0x48_21 : 1;
    bool m_0x48_20 : 1;
    bool m_0x48_19 : 1;
    bool m_0x48_18 : 1;
    bool m_0x48_17 : 1;
    bool m_0x48_16 : 1;
    bool m_0x48_15 : 1;
    bool m_0x48_14 : 1;
    bool m_0x48_13 : 1;
    bool m_0x48_12 : 1;
    bool m_0x48_11 : 1;
    bool m_0x48_10 : 1;
    bool m_0x48_9 : 1;
    bool m_0x48_8 : 1;
    bool m_0x48_7 : 1;
    bool m_0x48_6 : 1;
    bool m_0x48_5 : 1;
    bool m_0x48_4 : 1;
    bool m_0x48_3 : 1;
    bool m_0x48_2 : 1;
    bool m_0x48_1 : 1;
    bool m_0x48_0 : 1;
    };
    struct {
        u32 m_0x48 : 16; // role: the flag halfword the constructor clears
        u16 m_vtxStart; // role: first vertex of the joint in the collision's vertex arrays (grCollision::getSrcVtx adds it)
    };
    };

    u16 m_vtxLen;

    u16 _0x4E; // role: index into the ground's scene models (grCollisionJoint::getWorldMatrix reads it)

    // 80
    u16 m_nodeIndex;
    // 82
    union {
        struct {
            bool m_0x52_15 : 1;
            bool m_rightLedgeDisabled : 1;
            bool m_leftLedgeDisabled : 1;
            bool m_0x52_12 : 1;
            bool m_0x52_11 : 1;
            bool m_0x52_10 : 1;
            bool m_0x52_9 : 1;
            bool m_0x52_8 : 1;
            bool m_0x52_7 : 1;
            bool m_0x52_6 : 1;
            bool m_0x52_5 : 1;
            bool m_0x52_4 : 1;
            bool m_0x52_3 : 1;
            bool m_0x52_2 : 1;
            bool m_0x52_1 : 1;
            bool m_0x52_0 : 1;
        };
        u16 m_0x52;
    };

    // 84
    bool m_0x54_7 : 1;
    bool m_0x54_6 : 1;
    bool m_0x54_5 : 1;
    bool m_0x54_4 : 1;
    bool m_0x54_3 : 1;
    bool m_0x54_2 : 1;
    bool m_0x54_1 : 1;
    bool m_0x54_0 : 1;
    // 85
    bool m_0x55_7 : 1;
    bool m_0x55_6 : 1;
    bool m_0x55_5 : 1;
    bool m_0x55_4 : 1;
    bool m_0x55_3 : 1;
    bool m_0x55_2 : 1;
    bool m_0x55_1 : 1;
    bool m_0x55_0 : 1;
    // 86
    bool m_0x56_7 : 1;
    bool m_0x56_6 : 1;
    bool m_0x56_5 : 1;
    bool m_0x56_4 : 1;
    bool m_0x56_3 : 1;
    bool m_0x56_2 : 1;
    bool m_0x56_1 : 1;
    bool m_0x56_0 : 1;
    // 88
    Ground* m_ground;
    // 92
    Matrix* m_matrix;

    grCollisionJoint(grCollision* collision, grCollData::JointData* data);
    u8 getCollisionId(); // HYPOTHESIS: return type (the low byte of grCollision::m_index)
    grCollisionLine* getLine(u16 index);
    // HYPOTHESIS: signatures of the update members; the map has no parameter types.
    void updatePosNormal(Matrix* matrix) __attribute__((never_inline)); // MATCH-ONLY: the original calls it out of line from updatePos
    void updatePosInit(Matrix* matrix) __attribute__((never_inline)); // MATCH-ONLY: the original calls it out of line from updatePos
    void updateAABB() __attribute__((never_inline)); // MATCH-ONLY: the original calls it out of line from updatePos
    void updatePos(bool init);
    void clearVLink();
    void updateStatus();
    void getWorldMatrix(Matrix* out) __attribute__((never_inline)); // MATCH-ONLY: the original calls it out of line from updatePos
    void getSegment(u16 index, grCollSegmentEx* out);
};
static_assert(sizeof(grCollisionJoint) == 96, "Class is wrong size!");