#include <types.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_task_scheduler.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_manager.h>
#include <gr/collision/gr_collision_status.h>
#include <gr/collision/gr_collision_test_types.h>
#include <gr/ground.h>

// HYPOTHESIS: each vertex owns a 16-byte pair, current position first and previous position second
struct grCollVtxPair {
    Vec2f pos;
    Vec2f prev;
};

// HYPOTHESIS: the line id handed around as one word is the collision index followed by the line index
struct grCollLineIdParts {
    u16 m_collisionIndex;
    u16 m_lineIndex;
};

// HYPOTHESIS: raw view of one line record of the collision data (grCollData::LineData hides these words in its padding)
struct grCollLineDataRaw {
    u16 m_point0Index;
    u16 m_point1Index;
    short m_unk4;
    short m_unk6;
    char _8[4];
    u16 m_unkC; // becomes grCollisionLine::m_unkE (the attribute bits)
    u16 m_unkE; // becomes the halfword at +0x10 (the flag bits and the material)
};

// Unnamed helpers of grCollisionJoint and grCollStatus that this unit calls (owned by their units).
extern "C" {
void fn_80112EBC(grCollisionJoint* joint, grCollision* collision, grCollData::JointData* jointData); // grCollisionJoint::grCollisionJoint
void fn_801139E4(grCollisionJoint* joint); // grCollisionJoint::updateStatus
void fn_80113724(grCollisionJoint* joint, bool firstUpdate); // grCollisionJoint::updatePos
void fn_80113824(grCollisionJoint* joint); // grCollisionJoint::clearVLink
void fn_80113BA8(grCollisionJoint* joint, u16 lineIndex, clSegment2D* out); // grCollisionJoint::getSegment
void fn_801361F0(grCollStatus* status, grCollision* collision); // HYPOTHESIS: the status lets go of a collision that is being disabled
void fn_80112D64(grCollisionManager* manager, grCollision* collision); // grCollisionManager::release
// nw4r::ut::detail::LinkListImpl::Erase(LinkListNode*) (private in the header)
void* Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(void*, void*);
}

// The lines of a collision are 18 bytes apart (grCollisionLine pads itself to 20).
static inline grCollisionLine* lineAt(grCollisionLine* lines, int index) {
    return (grCollisionLine*)((char*)lines + index * 18);
}

typedef nw4r::ut::LinkList<grCollStatus, 0> grCollStatusNodeList;
extern grCollStatusNodeList lbl_8049E594;
extern grCollisionManager* lbl_805A0360;

static inline bool hasTargetCategory(grCollisionLine* line) {
    bool result = false;
    if (line->m_isTargetFighter || line->m_isTargetItem || line->m_isTargetOther) {
        result = true;
    }
    return result;
}

grCollisionJoint* grCollision::createJoint(u16 count, grCollData::JointData* jointDatas) {
    grCollisionJoint* joints = (grCollisionJoint*)gfHeapManager::alloc(Heaps::StageInstance, count * sizeof(grCollisionJoint));
    grCollisionJoint* joint = joints;
    for (u32 i = 0; i < count; i++) {
        if (joint != NULL) {
            fn_80112EBC(joint, this, jointDatas);
        }
        joint++;
        jointDatas++;
    }
    return joints;
}

grCollisionLine* grCollision::createLine(u16 count, grCollData::LineData* lineDatas, bool unk) {
    grCollisionLine* lines = (grCollisionLine*)gfHeapManager::alloc(Heaps::StageInstance, count * 18);
    grCollisionLine* line = lines;
    grCollLineDataRaw* data = (grCollLineDataRaw*)lineDatas;
    for (u32 i = 0; i < count; i++) {
        if (line != NULL) {
            // MATCH-ONLY: the vertex indices are copied as unsigned halfwords
            *(u16*)&line->m_point0Index = data->m_point0Index;
            *(u16*)&line->m_point1Index = data->m_point1Index;
            line->m_unk4 = data->m_unk4;
            line->m_unk6 = data->m_unk6;
            line->m_unk8 = 0;
            line->m_unk9 = 0;
            line->m_unkA = -1;
            line->m_unkC = -1;
            line->m_unkE = data->m_unkC;
            *(u16*)((u8*)line + 0x10) = data->m_unkE;
            if (!unk) {
                line->m_isTargetFighter = true;
            }
            // lines that no category claims are fighter lines
            if (!hasTargetCategory(line)) {
                line->m_isTargetFighter = true;
            }
        }
        line = lineAt(line, 1);
        data++;
    }
    return lines;
}

grCollision::~grCollision() {
    m_isDestroying = true;
    setDisableForce();
    fn_80112D64(lbl_805A0360, this);
    Erase__Q44nw4r2ut6detail12LinkListImplFPQ34nw4r2ut12LinkListNode(&g_grCollisionList, &m_next);
    gfHeapManager::free(m_vtxPositions);
    m_vtxPositions = NULL;
    gfHeapManager::free(m_lines);
    m_lines = NULL;
    gfHeapManager::free(m_joints);
    m_joints = NULL;
}

void grCollision::setEnable() {
    m_isEnabledLatch = true;
    m_isEnabled = m_isEnabledLatch;
    if (m_disablePending) {
        m_disablePending = false;
    }
}

void grCollision::setDisable() {
    if (m_isEnabled) {
        m_isEnabledLatch = false;
        bool inTaskPass = false;
        s32 phase = *(s32*)gfTaskScheduler::getInstance();
        if ((phase >> 24) == 1 && ((s32)((phase << 8) & 0xFF800000) >> 24) < 6) {
            inTaskPass = true;
        }
        if (inTaskPass) {
            m_disablePending = true;
        } else {
            setDisableForce();
        }
    }
}

void grCollision::setDisableForce() {
    bool wasEnabled = m_isEnabled;
    m_isEnabled = false;
    m_lineStatusDone = false;
    if (wasEnabled == true) {
        m_noUpdate = false;
        removeLink();
        // every collision status that still points at this collision lets go of it
        grCollStatusNodeList& list = lbl_8049E594;
        u32 size = list.GetSize();
        int index = 0;
        for (grCollStatusNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter();) {
            fn_801361F0(&*it, this);
            index++;
            u32 currentSize = list.GetSize();
            if (size > currentSize) {
                // the status removed itself from the list, so continue at the same position
                it = list.GetBeginIter();
                int target = index - 1;
                if (target < currentSize) {
                    index = 0;
                    for (int k = 0; k < target; k++) {
                        ++it;
                        index++;
                    }
                }
                size = currentSize;
            } else {
                it++;
            }
        }
    }
}

void grCollision::setJointStatus(bool enable, Ground* ground) {
    if (enable && !m_isEnabled) {
        m_enablePending = true;
        m_disablePending = false;
    }
    for (u32 i = 0; i < m_jointLen; i++) {
        grCollisionJoint* joint = &m_joints[i];
        if (joint->m_0x56_7 != true && joint->m_ground == ground && enable != joint->m_0x54_6) {
            if (enable) {
                joint->m_0x54_5 = true;
            } else {
                joint->m_0x54_6 = joint->m_0x54_4 = joint->m_0x54_7 = false;
            }
        }
    }
}

void grCollision::setJointAILowPriority(bool lowPriority) {
    for (u32 i = 0; i < m_jointLen; i++) {
        m_joints[i].m_0x55_3 = lowPriority;
    }
}

grCollisionJoint* grCollision::searchJoint(Ground* ground, u16 modelIndex, u16 nodeIndex) {
    grCollisionJoint* joint;
    for (u32 i = 0; i < m_jointLen; i++) {
        joint = &m_joints[i];
        if (joint->m_ground == ground && joint->_0x4E == modelIndex && joint->m_nodeIndex == nodeIndex) {
            return joint;
        }
    }
    return NULL;
}

const char* grCollision::getJointNodeName(u32 nodeIndex) {
    if (m_collData == NULL) {
        return NULL;
    }
    return m_collData->getJointNodeName(nodeIndex);
}

// MATCH-ONLY: the original takes the minimum / maximum of two floats with a paired-single subtract and select.
static inline float psMax(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, a, b
    }
    // clang-format on
    return result;
}

static inline float psMin(register float a, register float b) {
    register float diff, result;
    // clang-format off
    asm {
        ps_sub diff, a, b
        ps_sel result, diff, b, a
    }
    // clang-format on
    return result;
}

void grCollision::update() {
    grCollisionJoint* joint;
    if (m_enablePending) {
        m_isEnabled = true;
        m_enablePending = false;
    }
    bool emptyBox = true;
    int activeCount = 0;
    m_isEnabledLatch = m_isEnabled;
    m_unk2D_5 = m_unk2C_3;
    if (m_isEnabled) {
        bool firstUpdate = !m_noUpdate;
        bool first = true;
        clAABBox2D* box = &m_aabBox;
        for (u32 i = 0; i < m_jointLen; i++) {
            joint = &m_joints[i];
            fn_801139E4(joint);
            if (joint->m_0x54_6) {
                fn_80113724(joint, firstUpdate);
                if (joint->m_0x55_6) {
                    fn_80113824(joint);
                }
                activeCount++;
                if (first) {
                    first = false;
                    __memcpy(box, &joint->m_aabBox, sizeof(clAABBox2D));
                } else {
                    clAABBox2D jointBox;
                    __memcpy(&jointBox, &joint->m_aabBox, sizeof(clAABBox2D));
                    box->logicalOR(jointBox, *box);
                }
            }
        }
        m_noUpdate = true;
        m_hasActiveJoint = activeCount != 0;
        if (activeCount != 0) {
            emptyBox = false;
        }
    }
    if (emptyBox) {
        // HYPOTHESIS: a collision without an active joint keeps a tiny box so that box tests stay valid
        Vec2f a(0.01f, 0.01f);
        Vec2f b(-0.01f, 0.01f);
        m_aabBox.m_unk1 = psMin(b.m_x, a.m_x);
        m_aabBox.m_unk2 = psMin(a.m_y, b.m_y);
        m_aabBox.m_unk3 = psMax(b.m_x, a.m_x);
        m_aabBox.m_unk4 = psMax(a.m_y, b.m_y);
    }
}

void grCollision::updateClippingRange(clAABBox2D* range) {
    if (m_aabBox.checkIntersect1(*range)) {
        for (u32 i = 0; i < m_jointLen; i++) {
            grCollisionJoint* joint = &m_joints[i];
            if (joint->m_0x54_6) {
                if (!range->checkIntersect1(joint->m_aabBox)) {
                    joint->m_0x55_6 = false;
                }
            }
        }
    } else {
        m_unk2D_5 = false;
        for (u32 i = 0; i < m_jointLen; i++) {
            m_joints[i].m_0x55_6 = false;
        }
    }
}

void grCollision::updateLineStatus() {
    if (m_isEnabled) {
        if (m_hasActiveJoint) {
            bool first = true;
            bool hasHangJoint = false;
            clAABBox2D box;
            for (u32 i = 0; i < m_jointLen; i++) {
                grCollisionJoint* joint = &m_joints[i];
                joint->m_0x55_1 = false;
                if (joint->m_0x54_6) {
                    u16 lineLen = joint->m_lineLen;
                    for (u16 j = 0; j < lineLen; j++) {
                        grCollisionLine* line = joint->getLine(j);
                        if (line->m_isRotating) {
                            setLineFlags(line);
                        }
                        if (line->m_isHangL | line->m_isHangR) {
                            joint->m_0x55_1 = true;
                        }
                    }
                    if (first) {
                        first = false;
                        __memcpy(&box, &joint->m_aabBox, sizeof(clAABBox2D));
                    } else {
                        clAABBox2D jointBox;
                        __memcpy(&jointBox, &joint->m_aabBox, sizeof(clAABBox2D));
                        box.logicalOR(jointBox, box);
                    }
                    if (joint->m_0x55_1) {
                        hasHangJoint = true;
                    }
                }
            }
            __memcpy(&m_aabBox, &box, sizeof(clAABBox2D));
            m_hasHangJoint = hasHangJoint;
            if (m_unk2C_3 == true) {
                updateLinkVirtual();
            }
            m_lineStatusDone = true;
        }
    }
}

void grCollision::updateLinkVirtual() {
    Vec2f margin(1.0f, 1.0f); // MATCH-ONLY: the original keeps an unused tolerance vector here
    for (u32 i = 0; i < m_jointLen; i++) {
        grCollisionJoint* joint = &m_joints[i];
        if (joint->m_0x54_6 && joint->m_0x54_0) {
            clAABBox2D box;
            __memcpy(&box, &joint->m_aabBox, sizeof(clAABBox2D));
            for (u32 j = i + 1; j < m_jointLen; j++) {
                grCollisionJoint* other = &m_joints[j];
                if (other->m_0x54_6 && other->m_0x54_0 && box.checkIntersect1(other->m_aabBox)) {
                    updateLinkVirtual2(joint, other);
                }
            }
        }
    }
}

void grCollision::removeLink() {
    for (u32 i = 0; i < m_lineLen; i++) {
        grCollisionLine* line = lineAt(m_lines, i);
        if (line->m_unkA != -1) {
            line->m_unkA = -1;
            line->m_unk8 = 0;
        }
        if (line->m_unkC != -1) {
            line->m_unkC = -1;
            line->m_unk9 = 0;
        }
    }
    u16 index = m_index;
    typedef nw4r::ut::LinkList<grCollision, 0x30> grCollisionNodeList;
    grCollisionNodeList& list = (grCollisionNodeList&)g_grCollisionList;
    for (grCollisionNodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
        grCollision* other = &*it;
        for (u32 i = 0; i < other->m_lineLen; i++) {
            grCollisionLine* line = lineAt(other->m_lines, i);
            if (line->m_unk8 == index) {
                line->m_unkA = -1;
                line->m_unk8 = 0;
            }
            if (line->m_unk9 == index) {
                line->m_unkC = -1;
                line->m_unk9 = 0;
            }
        }
    }
}

void grCollision::removeLinkTarget(grCollision* target) {
    u16 index = target->m_index;
    for (u32 i = 0; i < m_lineLen; i++) {
        grCollisionLine* line = lineAt(m_lines, i);
        if (line->m_unk8 == index) {
            line->m_unkA = -1;
            line->m_unk8 = 0;
        }
        if (line->m_unk9 == index) {
            line->m_unkC = -1;
            line->m_unk9 = 0;
        }
    }
}

void grCollision::updateLinkVirtual1(grCollision* target) {
    if (m_aabBox.checkIntersect1(target->m_aabBox)) {
        for (u32 i = 0; i < target->m_jointLen; i++) {
            grCollisionJoint* targetJoint = &target->m_joints[i];
            if (targetJoint->m_0x54_6 && targetJoint->m_0x54_0) {
                if (m_aabBox.checkIntersect1(targetJoint->m_aabBox)) {
                    for (u32 j = 0; j < m_jointLen; j++) {
                        grCollisionJoint* joint = &m_joints[j];
                        if (joint->m_0x54_6 && joint->m_0x54_0 && joint->m_aabBox.checkIntersect1(targetJoint->m_aabBox)) {
                            updateLinkVirtual2(joint, targetJoint);
                        }
                    }
                }
            }
        }
    }
}

int grCollision::updateLinkVirtual2(grCollisionJoint* jointA, grCollisionJoint* jointB) {
    Vec2f margin(1.0f, 1.0f);
    int linkCount = 0;
    short lineIdA = jointA->m_lineStart;
    u16 lenA = jointA->m_lineLen;
    short startB = jointB->m_lineStart;
    u16 lenB = jointB->m_lineLen;
    u32 collisionA = jointA->getCollisionId();
    u32 collisionB = jointB->getCollisionId();
    for (u16 i = 0; i < lenA; i++, lineIdA++) {
        grCollisionLine* lineA = jointA->getLine(i);
        clSegment2D segmentA;
        fn_80113BA8(jointA, i, &segmentA);
        // the box around segment A, grown by the link tolerance
        clAABBox2D boxA;
        boxA.m_unk1 = psMin(segmentA.m_p0Pos.m_x, segmentA.m_p1Pos.m_x) - margin.m_x;
        boxA.m_unk2 = psMin(segmentA.m_p0Pos.m_y, segmentA.m_p1Pos.m_y) - margin.m_y;
        boxA.m_unk3 = psMax(segmentA.m_p0Pos.m_x, segmentA.m_p1Pos.m_x) + margin.m_x;
        boxA.m_unk4 = psMax(segmentA.m_p0Pos.m_y, segmentA.m_p1Pos.m_y) + margin.m_y;
        if (boxA.checkIntersect1(jointB->m_aabBox)) {
            // bit 0: the p0 end of the line has no virtual link yet, bit 1: the p1 end has none
            u32 freeA = (lineA->m_unkA == -1) | ((lineA->m_unkC == -1) << 1);
            u16 attrA = lineA->m_unkE & 0x7F;
            short lineIdB = startB;
            for (u16 j = 0; j < lenB; j++, lineIdB++) {
                grCollisionLine* lineB = jointB->getLine(j);
                u16 shared = attrA & lineB->m_unkE;
                u32 freeB = ((lineB->m_unkA == -1) << 1) | (lineB->m_unkC == -1);
                if (shared & 0x70) {
                    u32 sameDir = (shared & 0xF) != 0 ? 3 : 0;
                    u32 maskA = sameDir | freeA;
                    u32 mask = maskA | freeB;
                    if (mask != 0) {
                        u32 maskB = freeB | sameDir;
                        clSegment2D segmentB;
                        fn_80113BA8(jointB, j, &segmentB);
                        if (boxA.checkIntersect(segmentB)) {
                            if (mask & 1) {
                                Vec2f diff;
                                diff.m_x = segmentA.m_p0Pos.m_x - segmentB.m_p1Pos.m_x;
                                diff.m_y = segmentA.m_p0Pos.m_y - segmentB.m_p1Pos.m_y;
                                if (diff.m_x * diff.m_x + diff.m_y * diff.m_y < 1.0f) {
                                    if (maskA & 1) {
                                        lineA->m_unk8 = collisionB;
                                        lineA->m_unkA = lineIdB;
                                    }
                                    if (maskB & 1) {
                                        lineB->m_unk9 = collisionA;
                                        lineB->m_unkC = lineIdA;
                                    }
                                    linkCount++;
                                }
                            }
                            if (mask & 2) {
                                Vec2f diff;
                                diff.m_x = segmentA.m_p1Pos.m_x - segmentB.m_p0Pos.m_x;
                                diff.m_y = segmentA.m_p1Pos.m_y - segmentB.m_p0Pos.m_y;
                                if (diff.m_x * diff.m_x + diff.m_y * diff.m_y < 1.0f) {
                                    if (maskA & 2) {
                                        lineA->m_unk9 = collisionB;
                                        lineA->m_unkC = lineIdB;
                                    }
                                    if (maskB & 2) {
                                        lineB->m_unk8 = collisionA;
                                        lineB->m_unkA = lineIdA;
                                    }
                                    linkCount++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (linkCount != 0) {
        jointA->m_0x55_0 = true;
        jointB->m_0x55_0 = true;
    }
    return linkCount;
}

void grCollision::setVLinkClearStatus(bool status) {
    for (u32 i = 0; i < m_jointLen; i++) {
        m_joints[i].m_0x55_6 = status;
    }
}

void grCollision::setVLinkUpdateStatus(bool status) {
    for (u32 i = 0; i < m_jointLen; i++) {
        m_joints[i].m_0x54_0 = status;
    }
}

void grCollision::setLineFlags(grCollisionLine* line) {
    grCollSegmentEx segment;
    getSegment(&segment, line, 0);
    // MATCH-ONLY: the direction flags are the low nibble of the byte at +0xF; they are rebuilt from the way the line points
    u8* flagsByte = (u8*)line + 0xF;
    u8 flags = *flagsByte & 0xF0;
    float dx = segment.m_segment.m_p1Pos.m_x - segment.m_segment.m_p0Pos.m_x;
    float dy = segment.m_segment.m_p1Pos.m_y - segment.m_segment.m_p0Pos.m_y;
    *flagsByte = flags;
    if ((float)__fabs(dy) < 0.00001f) {
        if ((float)__fabs(dx) < 0.00001f) {
            // a line without length
            *((u8*)line + 0xE) |= 0x80;
        } else if (dx > 0.0f) {
            *flagsByte = flags | 0x1; // m_isUnder
        } else {
            *flagsByte = flags | 0x2; // m_isUpper
        }
    } else if ((float)__fabs(dx) < 0.00001f) {
        if (dy > 0.0f) {
            *flagsByte = flags | 0x8; // m_isRight
        } else {
            *flagsByte = flags | 0x4; // m_isLeft
        }
    } else if ((float)__fabs(dx / dy) > 1.0f) {
        if (dx > 0.0f) {
            *flagsByte = flags | 0x1;
        } else {
            *flagsByte = flags | 0x2;
        }
    } else {
        if (dy > 0.0f) {
            *flagsByte = flags | 0x8;
        } else {
            *flagsByte = flags | 0x4;
        }
    }
}

grCollisionJoint* grCollision::getJoint(u16 nodeIndex) {
    return &m_joints[nodeIndex];
}

void grCollision::getSegment(grCollSegmentEx* outSegment, grCollisionLine* line, int unk) {
    grCollVtxPair* pairs = (grCollVtxPair*)m_vtxPositions;
    Vec2f p0 = pairs[(u16)line->m_point0Index].pos;
    Vec2f p1 = pairs[(u16)line->m_point1Index].pos;
    outSegment->m_segment.m_p0Pos = p0;
    outSegment->m_segment.m_p1Pos = p1;
    outSegment->m_unk10 = unk;
}

void grCollision::getSegment(clSegment2D* outSegment, grCollisionLine* line) {
    // HYPOTHESIS: vertex positions are 16 bytes apart, so indices are doubled over Vec2f
    u16 index0 = line->m_point0Index;
    Vec2f* p0 = &m_vtxPositions[index0 * 2];
    u16 index1 = line->m_point1Index;
    Vec2f* p1 = &m_vtxPositions[index1 * 2];
    outSegment->m_p0Pos.m_x = p0->m_x;
    outSegment->m_p0Pos.m_y = p0->m_y;
    outSegment->m_p1Pos.m_x = p1->m_x;
    outSegment->m_p1Pos.m_y = p1->m_y;
}

void grCollision::getPreSegment(clSegment2D* outSegment, grCollisionLine* line) {
    grCollVtxPair* pairs = (grCollVtxPair*)m_vtxPositions;
    grCollVtxPair* pair0 = &pairs[(u16)line->m_point0Index];
    grCollVtxPair* pair1 = &pairs[(u16)line->m_point1Index];
    outSegment->m_p0Pos.m_x = pair0->prev.m_x;
    outSegment->m_p0Pos.m_y = pair0->prev.m_y;
    outSegment->m_p1Pos.m_x = pair1->prev.m_x;
    outSegment->m_p1Pos.m_y = pair1->prev.m_y;
}

void grCollision::getSegmentTrace(clSegment2D* outSegment, clSegment2D* outTrace, grCollisionLine* line) {
    u16 index0 = line->m_point0Index;
    Vec2f* p0 = &m_vtxPositions[index0 * 2];
    u16 index1 = line->m_point1Index;
    Vec2f* p1 = &m_vtxPositions[index1 * 2];
    outSegment->m_p0Pos.m_x = p0[0].m_x;
    outSegment->m_p0Pos.m_y = p0[0].m_y;
    outSegment->m_p1Pos.m_x = p1[0].m_x;
    outSegment->m_p1Pos.m_y = p1[0].m_y;
    outTrace->m_p0Pos.m_x = p0[1].m_x;
    outTrace->m_p0Pos.m_y = p0[1].m_y;
    outTrace->m_p1Pos.m_x = p1[1].m_x;
    outTrace->m_p1Pos.m_y = p1[1].m_y;
}

void grCollision::getSegmentTrace1(grCollSegmentEx* outSegment, clSegment2D* outTrace, grCollisionLine* line, int unk) {
    u16 index0 = line->m_point0Index;
    Vec2f* p0 = &m_vtxPositions[index0 * 2];
    u16 index1 = line->m_point1Index;
    Vec2f* p1 = &m_vtxPositions[index1 * 2];
    outSegment->m_segment.m_p0Pos.m_x = p0[0].m_x;
    outSegment->m_segment.m_p0Pos.m_y = p0[0].m_y;
    outSegment->m_segment.m_p1Pos.m_x = p1[0].m_x;
    outSegment->m_segment.m_p1Pos.m_y = p1[0].m_y;
    outTrace->m_p0Pos.m_x = p0[1].m_x;
    outTrace->m_p0Pos.m_y = p0[1].m_y;
    outTrace->m_p1Pos.m_x = p1[1].m_x;
    outTrace->m_p1Pos.m_y = p1[1].m_y;
    outSegment->m_unk10 = unk;
}

void grCollision::getPreSegment1(grCollSegmentEx* outSegment, grCollisionLine* line, int unk) {
    grCollVtxPair* pairs = (grCollVtxPair*)m_vtxPositions;
    Vec2f p0 = pairs[(u16)line->m_point0Index].prev;
    Vec2f p1 = pairs[(u16)line->m_point1Index].prev;
    outSegment->m_segment.m_p0Pos = p0;
    outSegment->m_segment.m_p1Pos = p1;
    outSegment->m_unk10 = unk;
}

grCollisionLine* grCollision::getLine(u16 index) {
    return lineAt(m_lines, index);
}

grCollisionLine* grCollision::getLine1(u16 index) {
    return lineAt(m_lines, index);
}

grCollisionLine* grCollision::getRightLine(grCollisionLine* line) {
    short index = line->m_unk6;
    grCollisionLine* result = NULL;
    if (index >= 0) {
        result = lineAt(m_lines, index);
    }
    return result;
}

grCollisionLine* grCollision::getLeftLine(grCollisionLine* line) {
    short index = line->m_unk4;
    grCollisionLine* result = NULL;
    if (index >= 0) {
        result = lineAt(m_lines, index);
    }
    return result;
}

grCollData::VtxData* grCollision::getSrcVtx(grCollisionJoint* joint, int index) {
    return &m_vtxDatas[joint->m_vtxStart] + index;
}

Vec2f* grCollision::getDstVtx(grCollisionJoint* joint, int index) {
    return &m_vtxPositions[joint->m_vtxStart * 2] + index * 2;
}

grCollisionJoint* grCollision::getJointByLineId(u32 lineIndex) {
    grCollisionJoint* found = NULL;
    for (u32 i = 0; i < m_jointLen; i++) {
        grCollisionJoint* joint = &m_joints[i];
        bool inside = false;
        if (joint->m_lineStart <= lineIndex && lineIndex < joint->m_lineStart + joint->m_lineLen) {
            inside = true;
        }
        if (inside) {
            found = joint;
            break;
        }
    }
    return found;
}

int grCollision::getLineIndex(grCollisionLine* line) {
    return (u32)((char*)line - (char*)m_lines) / 18;
}

u32 grCollision::getLineId(grCollisionLine* line) {
    union {
        u32 m_whole;
        grCollLineIdParts m_parts;
    } id;
    id.m_whole = 0xFFFF;
    id.m_parts.m_collisionIndex = m_index;
    id.m_parts.m_lineIndex = getLineIndex(line);
    return id.m_whole;
}

bool grCollision::getLineWork(u32* lineId, grCollisionLineWork* out) {
    bool found = false;
    grCollLineIdParts* id = (grCollLineIdParts*)lineId;
    int collisionIndex = id->m_collisionIndex;
    grCollision* collision = lbl_805A0360->getCollision(collisionIndex);
    if (collision != NULL && id->m_lineIndex < collision->m_lineLen) {
        grCollisionLine* line = collision->getLine(id->m_lineIndex);
        if (line != NULL) {
            grCollisionJoint* joint = collision->getJointByLineId((short)id->m_lineIndex);
            if (joint != NULL) {
                grCollisionLineWork work(line, joint, collision);
                *out = work;
                found = true;
            }
        }
    }
    if (!found) {
        out->m_collisionLine = NULL;
        out->m_collisionJoint = NULL;
        out->m_collision = NULL;
        out->m_unkC = 0;
    }
    return found;
}

void grCollision::calcLineVector(const u32& lineId, const Vec2f& pos, Vec2f* out) {
    grCollisionLineWork work;
    Vec2f tmp;
    u32 lineIdCopy = lineId;
    if (getLineWork(&lineIdCopy, &work)) {
        calcLineVector1(&work, pos, &tmp, out);
        return;
    }
    out->m_x = 0.0f;
    out->m_y = 0.0f;
}

Ground* grCollision::getReferenceGround(grCollisionLineWork* work) {
    return work->m_collisionJoint->m_ground;
}

void grCollision::calcLineVector1(grCollisionLineWork* work, const Vec2f& pos, Vec2f* outPos, Vec2f* outVec) {
    if (work->m_collisionJoint->m_0x54_4) {
        grCollSegmentEx cur;
        grCollSegmentEx prev;
        work->m_collision->getSegment(&cur, work->m_collisionLine, 0);
        work->m_collision->getPreSegment1(&prev, work->m_collisionLine, 0);
        Vec2f edge = prev.m_segment.m_p1Pos - prev.m_segment.m_p0Pos;
        Vec2f toPos = pos - prev.m_segment.m_p0Pos;
        float lengthSq = edge.m_x * edge.m_x + edge.m_y * edge.m_y;
        if (lengthSq > 0.0001f) {
            // how far along the line the point is, limited to the line itself
            float t = (edge.m_y * toPos.m_y + edge.m_x * toPos.m_x) / lengthSq;
            if (t < 0.0f) {
                t = 0.0f;
            }
            if (t > 1.0f) {
                t = 1.0f;
            }
            // the movement of the line at that point since the previous frame
            Vec2f move1 = cur.m_segment.m_p1Pos - prev.m_segment.m_p1Pos;
            Vec2f move0 = cur.m_segment.m_p0Pos - prev.m_segment.m_p0Pos;
            *outVec = move0 * (1.0f - t) + move1 * t;
        } else {
            Vec2f sum = (cur.m_segment.m_p0Pos - prev.m_segment.m_p0Pos) + (cur.m_segment.m_p1Pos - prev.m_segment.m_p1Pos);
            *outVec = sum * 0.5f;
        }
        *outPos = pos + *outVec;
    } else {
        *outPos = pos;
        outVec->m_x = 0.0f;
        outVec->m_y = 0.0f;
    }
}

void grCollision::initWorkVtxPos(u16 index, Vec2f* pos) {
    grCollVtxPair* pair = &((grCollVtxPair*)m_vtxPositions)[index];
    pair->prev = *pos;
    pair = &((grCollVtxPair*)m_vtxPositions)[index];
    pair->pos = *pos;
}

void grCollision::setWorkVtxPos(u16 index, Vec2f* pos) {
    grCollVtxPair* pair = &((grCollVtxPair*)m_vtxPositions)[index];
    pair->pos = *pos;
}

void grCollision::copyVtxToPrev() {
    for (u16 i = 0; i < (u16)m_vtxLen; i++) {
        grCollVtxPair* pair = &((grCollVtxPair*)m_vtxPositions)[i];
        pair->prev.m_x = pair->pos.m_x;
        pair->prev.m_y = pair->pos.m_y;
    }
}

grCollisionList g_grCollisionList;
