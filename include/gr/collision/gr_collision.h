#pragma once

// Local shadow: adds the verified grCollision::setEnable / setDisable members.

#include <StaticAssert.h>
#include <gr/collision/gr_collision_handle.h>
#include <gr/collision/gr_collision_linework.h>
#include <nw4r/ut/ut_LinkList.h>
#include <types.h>

class Stage;
struct grCollSegmentEx;

class grCollision {
public:
    // 0
    u16 m_index;
    // 2
    short m_vtxLen;
    // 4
    u16 m_lineLen;
    // 6
    u16 m_jointLen;
    // 8
    grCollData* m_collData;
    // 12
    grCollData::VtxData* m_vtxDatas;
    // 16
    Vec2f* m_vtxPositions;
    // 20
    grCollisionLine* m_lines;
    // 24
    grCollisionJoint* m_joints;
    // 28
    clAABBox2D m_aabBox;
    // 44
    bool m_isEnabled : 1; // verified: bit 0x80 of byte 0x2C, set by setEnable and cleared by setDisable
    bool m_enablePending : 1; // HYPOTHESIS: bit 0x40 of byte 0x2C; update() turns it into m_isEnabled
    bool m_disablePending : 1; // HYPOTHESIS: bit 0x20 of byte 0x2C; set by setDisable during a task pass, applied by grCollisionManager::processFixPosition
    bool m_hasActiveJoint : 1; // HYPOTHESIS: bit 0x10 of byte 0x2C; update() sets it while at least one joint is active, updateLineStatus() runs only then
    bool m_unk2C_3 : 1; // role: bit 0x08 of byte 0x2C; update() copies it to m_unk2D_5, updateLineStatus() links the lines to other collisions only while it is set
    bool m_noUpdate : 1; // HYPOTHESIS: bit 0x04 of byte 0x2C; while set the module does not refresh the collision
                         // role: update() sets it after the first pass (and passes its inverse to the joints as "first update"), setDisableForce() clears it
    bool m_isDynamic : 1; // HYPOTHESIS: bit 0x02 of byte 0x2C; the collision moves (checked by isOnDynamicCollision)
    bool m_lineStatusDone : 1; // HYPOTHESIS: bit 0x01 of byte 0x2C; set at the end of updateLineStatus()
    // 45
    union {
        char _spacer[3]; // kept for the older callers that test bits of _spacer[0]
        struct {
            bool m_unk2D_7 : 1;
            bool m_isEnabledLatch : 1; // HYPOTHESIS: bit 0x40 of byte 0x2D; setEnable sets it, setDisable clears it, update() copies m_isEnabled into it
            bool m_unk2D_5 : 1; // role: bit 0x20 of byte 0x2D; copy of m_unk2C_3 (grCollisionManager::collisionUpdate tests it on both sides of a virtual link)
            bool m_hasHangJoint : 1; // HYPOTHESIS: bit 0x10 of byte 0x2D; updateLineStatus() sets it when a joint has ledge-hang lines
            bool m_isDestroying : 1; // HYPOTHESIS: bit 0x08 of byte 0x2D; the destructor sets it before unregistering
            bool m_unk2D_2 : 1;
            bool m_unk2D_1 : 1;
            bool m_unk2D_0 : 1;
        };
    };
    // 48
    void* m_next;
    // 52
    void* m_prev;
    // TOTAL_SIZE == 56
public:
    static grCollision* create(Stage* stage, void* colData, Ground* ground, int unk2);
    grCollision(void* colData, int unk1, int unk2);
    ~grCollision();

    void setEnable();
    void setDisable();
    // Local shadow additions (names from the symbol map; HYPOTHESIS signatures).
    grCollisionJoint* createJoint(u16 count, grCollData::JointData* jointDatas);
    grCollisionLine* createLine(u16 count, grCollData::LineData* lineDatas, bool unk);
    void setDisableForce();
    void setJointStatus(bool enable, Ground* ground);
    void setJointAILowPriority(bool lowPriority);
    grCollisionJoint* searchJoint(Ground* ground, u16 modelIndex, u16 nodeIndex);
    const char* getJointNodeName(u32 nodeIndex);
    void initWorkVtxPos(u16 index, Vec2f* pos);
    void setWorkVtxPos(u16 index, Vec2f* pos);
    void copyVtxToPrev();
    void update();
    void updateClippingRange(clAABBox2D* range);
    void updateLineStatus();
    void updateLinkVirtual();
    void removeLink();
    void removeLinkTarget(grCollision* target);
    void updateLinkVirtual1(grCollision* target);
    int updateLinkVirtual2(grCollisionJoint* joint, grCollisionJoint* otherJoint);
    void setVLinkClearStatus(bool status);
    void setVLinkUpdateStatus(bool status);
    void setLineFlags(grCollisionLine* line);
    static bool getLineWork(u32* lineId, grCollisionLineWork* out);
    static void calcLineVector(const u32& lineId, const Vec2f& pos, Vec2f* out);
    static void calcLineVector1(grCollisionLineWork* work, const Vec2f& pos, Vec2f* outPos, Vec2f* outVec);
    static Ground* getReferenceGround(grCollisionLineWork* work);

    grCollisionJoint* getJoint(u16 nodeIndex);
    grCollisionLine* getLine(u16 index);
    grCollisionLine* getLine1(u16 index);
    grCollisionLine* getRightLine(grCollisionLine* line);
    grCollisionLine* getLeftLine(grCollisionLine* line);
    grCollisionJoint* getJointByLineId(u32 lineIndex);
    int getLineIndex(grCollisionLine* line);
    u32 getLineId(grCollisionLine* line);
    grCollData::VtxData* getSrcVtx(grCollisionJoint* joint, int index);
    Vec2f* getDstVtx(grCollisionJoint* joint, int index);
    void getSegment(clSegment2D* outSegment, grCollisionLine* line);
    void getSegment(grCollSegmentEx* outSegment, grCollisionLine* line, int unk);
    void getPreSegment(clSegment2D* outSegment, grCollisionLine* line);
    void getPreSegment1(grCollSegmentEx* outSegment, grCollisionLine* line, int unk);
    void getSegmentTrace(clSegment2D* outSegment, clSegment2D* outTrace, grCollisionLine* line);
    void getSegmentTrace1(grCollSegmentEx* outSegment, clSegment2D* outTrace, grCollisionLine* line, int unk);
};
static_assert(sizeof(grCollision) == 56, "Class is wrong size!");

// TODO: magic number 52 could be the size of a class
struct grCollisionList : public nw4r::ut::LinkList<grCollisionHandle, 52> {

};
// TODO size assertion

extern grCollisionList g_grCollisionList;
