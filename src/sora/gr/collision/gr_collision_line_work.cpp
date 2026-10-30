// MATCH-ONLY: this unit copies whole vectors with the aggregate word copy of a plain base (see mt_vector.h).
#define MT_VEC2F_PLAIN_BASE
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_collection.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_test_types.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_memory_util.h>
#include <types.h>

grCollCollection::grCollCollection(int capacity) {
    m_lineWorks.m_capacity = capacity;
    m_unk10 = 0;
    m_unk12 = 7;
    m_unk13 = true;
    m_lineWorks.m_num = 0;
    m_lineWorks.m_works = static_cast<grCollisionLineWork*>(gfHeapManager::alloc(Heaps::Tmp, capacity * sizeof(grCollisionLineWork)));
    gfMemFill(m_lineWorks.m_works, 0, capacity * sizeof(grCollisionLineWork));
}

grCollCollection::~grCollCollection() { }

void grCollCollection::reset() {
    m_unk10 = 0;
    m_unk12 = 7;
    m_unk13 = true;
    m_lineWorks.m_num = 0;
}

// MATCH-ONLY: the original passes the unextended 32-bit loop counters to these two (every caller in the dol does), so the index
// parameters of grCollision::getJoint / getLine1 are probably not u16; the stand-ins keep the symbols of the members.
extern "C" grCollisionJoint* getJoint__11grCollisionFUs(grCollision* collision, u32 index);
extern "C" grCollisionLine* getLine1__11grCollisionFUs(grCollision* collision, u32 index);

// MATCH-ONLY: the original measures a returned vector temporary through a by-value copy.
static inline float getLength(Vec2f vec) {
    return vec.length();
}

// MATCH-ONLY: the original takes the componentwise minimum and maximum of the end points of a segment with paired singles.
static inline void segmentBounds(register const clSegment2D* segment, register Vec2f* lo, register Vec2f* hi) {
    register float p0, p1, diff, minPair, maxPair;
    // clang-format off
    asm {
        psq_l p0, 0(segment), 0, 0
        psq_l p1, 8(segment), 0, 0
        ps_sub diff, p0, p1
        ps_sel minPair, diff, p1, p0
        ps_sel maxPair, diff, p0, p1
        psq_st minPair, 0(lo), 0, 0
        psq_st maxPair, 0(hi), 0, 0
    }
    // clang-format on
}

// MATCH-ONLY: the componentwise minimum and maximum of two pairs, written to out (min first, then max).
static inline void mergeBounds(register Vec2f* outMin, register Vec2f* outMax, register const Vec2f* minB, register const Vec2f* maxB) {
    register float a, b, c, d, diffMin, diffMax, resMin, resMax;
    // clang-format off
    asm {
        psq_l a, 0(outMin), 0, 0
        psq_l b, 0(outMax), 0, 0
        psq_l c, 0(minB), 0, 0
        psq_l d, 0(maxB), 0, 0
        ps_sub diffMin, a, c
        ps_sub diffMax, b, d
        ps_sel resMin, diffMin, c, a
        ps_sel resMax, diffMax, b, d
        psq_st resMin, 0(outMin), 0, 0
        psq_st resMax, 0(outMax), 0, 0
    }
    // clang-format on
}

static inline bool segmentsEqual(const clSegment2D& a, const clSegment2D& b) {
    return a.m_p0Pos.m_x == b.m_p0Pos.m_x && a.m_p0Pos.m_y == b.m_p0Pos.m_y && a.m_p1Pos.m_x == b.m_p1Pos.m_x &&
           a.m_p1Pos.m_y == b.m_p1Pos.m_y;
}

static inline bool segmentsDiffer(const clSegment2D& a, const clSegment2D& b) {
    bool differ = true;
    if (segmentsEqual(a, b)) {
        differ = false;
    }
    return differ;
}

// Adds the line works of the collision's lines that the query box meets: the line must be allowed by the category mask,
// not carry an excluded attribute, be at least a minimum long and the box must touch the area it swept this frame.
bool grCollCollection::addSegment(grCollision* collision) {
    u16 categoryBits = m_unk12 << 4;
    u32 jointLen = collision->m_jointLen;
    u32 jointLineId = 0;
    for (u32 i = 0; i < jointLen; i++, jointLineId += 0x10000) {
        grCollisionJoint* joint = getJoint__11grCollisionFUs(collision, i);
        if (joint->m_0x54_6 && joint->m_0x54_4) {
            u32 lineStart = joint->m_lineStart;
            u32 lineLen = joint->m_lineLen;
            if (joint->m_aabBox.checkIntersect1(m_aabb)) {
                for (u32 index = lineStart; index < lineStart + lineLen; index++) {
                    grCollisionLine* line = getLine1__11grCollisionFUs(collision, index);
                    u16 attr = *(u16*)((char*)line + 0x10);
                    if ((attr & m_unk10) == 0 && (line->m_unkE & categoryBits) != 0) {
                        grCollSegmentEx segment;
                        clSegment2D trace;
                        collision->getSegmentTrace1(&segment, &trace, line, jointLineId | index);
                        if (getLength(segment.m_segment.m_p1Pos - segment.m_segment.m_p0Pos) < 0.01f) {
                            continue;
                        }
                        {
                            bool hit = false;
                            if (!segmentsDiffer(segment.m_segment, trace)) {
                                hit = m_aabb.checkIntersect(segment.m_segment);
                            } else {
                                clAABBox2D sweep;
                                Vec2f traceHi;
                                Vec2f traceLo;
                                segmentBounds(&segment.m_segment, (Vec2f*)&sweep, (Vec2f*)&sweep.m_unk3);
                                segmentBounds(&trace, &traceLo, &traceHi);
                                mergeBounds((Vec2f*)&sweep, (Vec2f*)&sweep.m_unk3, &traceLo, &traceHi);
                                if (m_aabb.checkIntersect1(sweep)) {
                                    hit = true;
                                }
                            }
                            if (hit) {
                                grCollisionLineWork work;
                                work.m_collisionLine = line;
                                work.m_collisionJoint = joint;
                                work.m_collision = collision;
                                work.m_unkC = 0;
                                work.m_unkD = false;
                                if (m_lineWorks.m_num < m_lineWorks.m_capacity) {
                                    m_lineWorks.m_works[m_lineWorks.m_num] = work;
                                    m_lineWorks.m_num++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return m_lineWorks.m_num != 0;
}

// Fills the list with the line works near the box: from one collision when given, else from every collision that
// has finished updating (the dynamic ones too when m_unk13 is set), except the collision in m_unk14.
bool grCollCollection::collect(u16 excludedAttr, grCollision* only) {
    m_unk10 = excludedAttr;
    if (only != NULL) {
        if (only->m_isEnabled && only->m_lineStatusDone) {
            addSegment(only);
        }
    } else if (m_unk13) {
        typedef nw4r::ut::LinkList<grCollision, 0x30> NodeList;
        NodeList& list = (NodeList&)g_grCollisionList;
        for (NodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
            grCollision* collision = &*it;
            if (collision != m_unk14 && collision->m_noUpdate && collision->m_isEnabled && collision->m_lineStatusDone) {
                addSegment(collision);
            }
        }
    } else {
        typedef nw4r::ut::LinkList<grCollision, 0x30> NodeList;
        NodeList& list = (NodeList&)g_grCollisionList;
        for (NodeList::Iterator it = list.GetBeginIter(); it != list.GetEndIter(); it++) {
            grCollision* collision = &*it;
            if (collision != m_unk14 && collision->m_noUpdate && !collision->m_isDynamic && collision->m_isEnabled &&
                collision->m_lineStatusDone) {
                addSegment(collision);
            }
        }
    }
    return m_lineWorks.m_num != 0;
}
