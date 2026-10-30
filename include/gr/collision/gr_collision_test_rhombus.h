#pragma once

// grCollisionTestRhombus: the collision test used for fighters (main.dol 0x801256E4-0x80132004, gr_collision_body_rhombus*.cpp).
// The shape is a rhombus (up/down/left/right corner around a centre, grCollShapeRhombus). grCollStatus::test (0x80134CA0)
// allocates 0x480 bytes (__nw(0x480, 0xc)) and calls the constructor (0x801258BC) when the current shape type is 1 or 2.
//
// Layout recovered from the constructor, resetWork, updateCurrent/updateTarget and every access of the 49 functions.
// Naming: m_unkNNN = hex byte offset when no role is evident; "role:" comments give the evidence, HYPOTHESIS marks guesses.
// The base class (grCollisionTest, 0x330 bytes with the vptr at 0x32C) is documented in gr_collision_test.h.
//
// Work shapes (all grCollShapeRhombus, 0x3C bytes: vptr, then clRhombus2D = centre, up, down, left, right, aabb):
//   m_shape0 (0x330) previous shape        <- grCollStatus::m_prevCollShape   (base m_unk2b8 points here)
//   m_shape1 (0x36C) current shape         <- grCollStatus::m_prevCollShape   (base m_unk2bc; updateCurrent moves it toward m_shape2)
//   m_shape2 (0x3A8) intermediate target   <- grCollStatus::m_currentCollShape (base m_unk2c0; updateTarget = lerp(m_shape1, m_shape3))
//   m_shape3 (0x3E4) full target           <- copy of m_shape2 (base m_unk2c4)
// Because the shapes are rhombi, the rhombus of a shape sits at shape + 4 (m_shape1.m_rhombus = this + 0x370, m_shape2 = 0x3AC).
//
// Order of the virtual declarations: the first virtual of the class is resetWork, as in the vtable (0x8045DA18), because MWCC
// emits the vtable in the translation unit that defines the first virtual function (gr_collision_body_rhombus.cpp).

#include <StaticAssert.h>
#include <gr/collision/gr_collision_shape_rhombus.h>
#include <gr/collision/gr_collision_test.h>
#include <types.h>

class grCollision;
class grCollisionLine;

class grCollisionTestRhombus : public grCollisionTest {
public:
    // Corner bits of m_unmovedMask (testCollisionSetup): the corner of m_shape1 equals the same corner of m_shape2.
    static const u16 VTX_UP = 1;
    static const u16 VTX_DOWN = 2;
    static const u16 VTX_RIGHT = 4;
    static const u16 VTX_LEFT = 8;
    // m_hitDirs uses grCollisionTest::DIR_LEFT (1), DIR_RIGHT (2), DIR_UP (4), DIR_DOWN (8).

    // 816 (0x330)
    grCollShapeRhombus m_shape0;
    // 876 (0x36C)
    grCollShapeRhombus m_shape1;
    // 936 (0x3A8)
    grCollShapeRhombus m_shape2;
    // 996 (0x3E4)
    grCollShapeRhombus m_shape3;
    // 1056 (0x420)
    clRhombus2D m_savedRhombus; // role: testStaticCorrectionSetup copies m_shape1's rhombus (14 floats, 0x370..0x3A7) here before
                                // the static correction; testStaticDown / testStaticChangeDownLineCheck read its down corner
                                // (0x430/0x434, the position before the correction) to look up the line below it.
    // 1112 (0x458)
    u16 m_hitDirs; // role: bits of grCollisionTest::DIR_*; testLeft ORs 1 (and 4 for the upper corner), testRight 2, testUp 4, testDown 8
                   // when the moving rhombus hit a line from that side; cleared by testCollisionNotMove / testCollisionCompound.
                   // The MoveSlatingLine dispatchers skip a corner variant whose direction was already hit (mask 10, 6, 9, 5, 8).
    // 1114 (0x45A)
    u16 m_unmovedMask; // role: testCollisionSetup: VTX_UP/DOWN/RIGHT/LEFT set when the corner of m_shape1 is equal to the one of
                       // m_shape2 (that corner does not move this pass); testUp/Down/Left/Right and the MoveSlatingLine variants
                       // only test corners whose bit is clear.
    // 1116 (0x45C) offsets of the corners of m_shape3 from its centre, filled by resetWork
    Vec2f m_cornerOffsetLeft;  // role: ajustShape/calcShapeMin: width = m_cornerOffsetRight.x - m_cornerOffsetLeft.x
    // 1124 (0x464)
    Vec2f m_cornerOffsetRight;
    // 1132 (0x46C)
    Vec2f m_cornerOffsetUp;    // role: ajustShape: height = m_cornerOffsetUp.y - m_cornerOffsetDown.y
    // 1140 (0x474)
    Vec2f m_cornerOffsetDown;
    // 1148 (0x47C)
    u8 m_unk47C; // HYPOTHESIS: "the move is mostly horizontal". resetWork sets it to 1 when |dy| < 0.05 && |dx| > 0.4, or
                 // |dx| > 0.5 && |dx/dy| > 1 (or, with grCollStatus + 0x73 bit 4, |dx/dy| > 1), after a ledge special case;
                 // testCollisionNotMoveSlatingLine takes the MoveX* variants when it is set (and the status is not on the ground).
    // 1149-1151: padding. sizeof == 0x480.

    grCollisionTestRhombus(grCollStatus* status);

    // ---- virtual overrides, in vtable order (0x8045DA18, slot 0 at +8). The base declares the slots.
    virtual void resetWork();                                                           // +0x08 0x80125930
    virtual void updateCurrent(float rate);                                             // +0x0C 0x80125E1C
    virtual void updateTarget(float rate);                                              // +0x10 0x80125E30
    virtual void ajustShape();                                                          // +0x14 0x80126B48
    virtual int testStaticCorrection(grCollisionLineWork* lineWork);                    // +0x18 0x80126F40
    virtual void testCollisionSetup();                                                  // +0x1C 0x8012C32C
    virtual int testCollisionNotMove(grCollisionLineWork* lineWork);                    // +0x20 0x8012C6A8
    virtual int testCollisionCompound(grCollisionLineWork* lineWork);                   // +0x24 0x80132000 (a tail jump to testCollisionNotMove)
    virtual int testStaticCorrectionLast(grCollisionLineWork* lineWork);                // +0x28 0x8012A8B8
    virtual void testStaticCorrectionSetup();                                           // +0x2C 0x80126EA4
    virtual void updateIgnoreLine();                                                    // +0x30 0x80125E44
    // The base declares `void* unk`; the parameter is a float* (the move rate, in/out: testCollisionNotMove passes its local rate).
    // It must keep the base signature to override the slot; cast to float* in the body. Returns 1 on every path (r3 = 1 at the epilogue).
    virtual int testCollisionNotMoveSlatingLine(grCollisionLineWork* lineWork, float* rate); // +0x34 0x8012CADC

    // ---- non-virtual members. All of them are called out of line (bl), hence never_inline.
    // No `this` (r3 = rhombus, r4 = segment): returns 0 when the segment misses the rhombus, else the getIntersectInfo bits
    // (a nonzero bit pattern of the edges crossed), or 0x10 when the segment lies inside it. 0x801256E4.
    static u8 checkIntersect(clRhombus2D* rhombus, clSegment2D* segment) __attribute__((never_inline));
    // 0x801260DC. pos = the centre (getCenterPos); outSpace = (free width, free height) around pos between the collected lines.
    // Also fills m_unk2EC/2F4/2FC/304 (the free box edges) and m_unk31C (ground line). Called by ajustShape.
    void calcSpace(const Vec2f* pos, Vec2f* outSpace) __attribute__((never_inline)); // HYPOTHESIS: parameter types
    // 0x80126A80. Shrinks the rhombus to the minimum width around m_unk2FC..m_unk304 (called by ajustShape); the result is the
    // inlined isFinite() of the aabb (the caller ignores it).
    int calcShapeMin(clRhombus2D* rhombus) __attribute__((never_inline)); // HYPOTHESIS: parameter/return type
    // The static correction of one line, by direction. flags = checkIntersect's result. Each returns 1 when the rhombus was moved.
    int testStaticRight(grCollisionLineWork* lineWork, u8 flags) __attribute__((never_inline));  // 0x80127524 (parameter 3 is read: bit 0)
    int testStaticLeft(grCollisionLineWork* lineWork, u8 flags) __attribute__((never_inline));   // 0x80128330
    int testStaticUp(grCollisionLineWork* lineWork, u8 flags) __attribute__((never_inline));     // 0x80129190 (flags not read)
    int testStaticDown(grCollisionLineWork* lineWork, u8 flags) __attribute__((never_inline));   // 0x801295D0
    int testStaticChangeDownLineCheck(grCollisionLineWork* lineWork) __attribute__((never_inline)); // 0x8012A2C8, returns 1 when moved
    // The dynamic test of one corner against one line. rate = in/out move rate (lowest hit so far). Returns 1 on a hit (0 for none);
    // on a hit each sets its m_hitDirs bit and calls updateTouchLine; testUp also ORs SIDE_UP into m_unk2CA and copies the line
    // into m_unk30C.
    int testUp(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));    // 0x8012ACA0
    int testDown(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));  // 0x8012B120
    int testLeft(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));  // 0x8012B5A8
    int testRight(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline)); // 0x8012BC68

    // The corner-sweep tests of testCollisionNotMoveSlatingLine, one per (axis, direction), 0x8012CFA0-0x80132000.
    // Dispatchers (X/Y x Right/Left/Up/Down) look at the neighbour lines and call the two corner variants; they return 1.
    // Corner variants sweep the quad between a corner pair of m_shape1 and m_shape2 against the line; they return void and, on a
    // hit, move m_shape2 (clRhombus2D::move at 0x3AC), set *rate = 1.0 and call updateTouchLine(id 4-7, lineWork).
    int testCollisionMoveSlatingLineMoveXRight(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));  // 0x8012CFA0
    int testCollisionMoveSlatingLineMoveXLeft(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));   // 0x8012D1A8
    void testCollisionMoveSlatingLineMoveXRightDown(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline)); // 0x8012D39C
    void testCollisionMoveSlatingLineMoveXRightUp(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));   // 0x8012D7D8
    void testCollisionMoveSlatingLineMoveXLeftDown(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));  // 0x8012DC08
    void testCollisionMoveSlatingLineMoveXLeftUp(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));    // 0x8012E044
    int testCollisionMoveSlatingLineMoveXUp(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));     // 0x8012E474
    void testCollisionMoveSlatingLineMoveXUpRight(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));    // 0x8012E67C
    void testCollisionMoveSlatingLineMoveXUpLeft(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));     // 0x8012E9FC
    int testCollisionMoveSlatingLineMoveXDown(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));   // 0x8012ED7C
    void testCollisionMoveSlatingLineMoveXDownRight(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));  // 0x8012EF84
    void testCollisionMoveSlatingLineMoveXDownLeft(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));   // 0x8012F32C
    int testCollisionMoveSlatingLineMoveYRight(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));  // 0x8012F6AC
    int testCollisionMoveSlatingLineMoveYLeft(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));   // 0x8012F8B4
    void testCollisionMoveSlatingLineMoveYRightDown(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline)); // 0x8012FABC
    void testCollisionMoveSlatingLineMoveYRightUp(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));   // 0x8012FE3C
    void testCollisionMoveSlatingLineMoveYLeftDown(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));  // 0x8013022C
    void testCollisionMoveSlatingLineMoveYLeftUp(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));    // 0x801305AC
    int testCollisionMoveSlatingLineMoveYUp(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));     // 0x8013099C
    void testCollisionMoveSlatingLineMoveYUpRight(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));    // 0x80130B90
    void testCollisionMoveSlatingLineMoveYUpLeft(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));     // 0x80131000
    int testCollisionMoveSlatingLineMoveYDown(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));   // 0x80131498
    void testCollisionMoveSlatingLineMoveYDownRight(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));  // 0x801316A0
    void testCollisionMoveSlatingLineMoveYDownLeft(grCollisionLineWork* lineWork, float* rate) __attribute__((never_inline));   // 0x80131B50
};
static_assert(sizeof(grCollisionTestRhombus) == 0x480, "Class is wrong size!");
static_assert(offsetof(grCollisionTestRhombus, m_shape0) == 0x330, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_shape1) == 0x36C, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_shape2) == 0x3A8, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_shape3) == 0x3E4, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_savedRhombus) == 0x420, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_hitDirs) == 0x458, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_unmovedMask) == 0x45A, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_cornerOffsetLeft) == 0x45C, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_cornerOffsetDown) == 0x474, "Class is wrong layout!");
static_assert(offsetof(grCollisionTestRhombus, m_unk47C) == 0x47C, "Class is wrong layout!");

// ---------------------------------------------------------------------------------------------------------------------------
// External callees shared by the three rhombus units (stand-ins until they are named in symbols.txt; HYPOTHESIS signatures,
// checked against the call sites' register use). Where the symbol already has a real name the member is declared in the
// headers instead (grCollisionLineWork::getNext/getPrev, clRhombus2D::move/getIntersectInfo, clSegment2D::projectionX/Y,
// grCollisionTest::updateTouchLine/testMovePtToSeg/isIgnoreLine/pushbackPreIgnoreLine/createCheckMaskStatic).
// ---------------------------------------------------------------------------------------------------------------------------
class grCollStatus;
extern "C" {
// grCollision::getSegment, 4-argument overload (5 words out: the segment and `unk`, every caller passes 0). 0x80111968
void fn_80111968(grCollision* collision, grCollSegmentEx* out, grCollisionLine* line, int unk);
// grCollision::getPreSegment1 (same shape, the line's segment one frame ago). 0x80111AF4
void fn_80111AF4(grCollision* collision, grCollSegmentEx* out, grCollisionLine* line, int unk);
// grCollision::getPreSegment (3 arguments). 0x801119F8
void fn_801119F8(grCollision* collision, clSegment2D* out, grCollisionLine* line);
// grCollision::getSegmentTrace: out = the segment now, trace = the segment from the previous vertex positions. 0x80111A38
// (Ghidra drops the fourth argument `line`.)
void fn_80111A38(grCollision* collision, clSegment2D* out, clSegment2D* trace, grCollisionLine* line);
// grCollisionLineWork::getPrevNoVLink / getNextNoVLink. 0x80113F88 / 0x80114024
// grCollisionLineWork::getPosXDownLine / getPosXDownLinePre: float x is in f1 (Ghidra drops it). 0x801150D0 / 0x8011557C
// grCollisionLineWork::getPosXUpLine / getPosYLeftLine / getPosYRightLine (no float argument). 0x80115A28 / 0x80115E44 / 0x801162F4
// grCollisionLineWork::getLeftDownLine / getRightDownLine. 0x801167A4 / 0x80116960
// grCollStatus::addIgnoreLine / isIgnoreLine / removeIgnoreLine / removeIgnoreLineWithNeighbor. 0x80137074 / 0x80137114 / 0x801371D8 / 0x80137298
void fn_80137074(grCollStatus* status, grCollisionLineWork* lineWork);
int fn_80137114(grCollStatus* status, grCollisionLineWork* lineWork);
void fn_801371D8(grCollStatus* status, grCollisionLineWork* lineWork);
void fn_80137298(grCollStatus* status, grCollisionLineWork* lineWork);
}
