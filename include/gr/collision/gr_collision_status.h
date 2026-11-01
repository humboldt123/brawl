#pragma once

// Local shadow of BrawlHeaders/gr/collision/gr_collision_status.h: declares more of the grCollStatus members.

#include <StaticAssert.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_shape_circle.h>
#include <gr/collision/gr_collision_shape_rhombus.h>
#include <types.h>

class grCollStatusEventReceiver;

class grCollStatus {
public:

    enum TracerType {

    };

    enum Setting {
        Setting_None = 0x0,
        Setting_Ground = 0x1,
        Setting_Air = 0x2,
        Setting_Cliff = 0x3,
    };

    enum TouchId {
        Touch_Up = 0x0,
        Touch_Left = 0x1,
        Touch_Right = 0x2,
        Touch_Down = 0x3,
        Touch_Up_Left = 0x4,
        Touch_Up_Right = 0x5,
        Touch_Down_Left = 0x6,
        Touch_Down_Right = 0x7,
    };
    typedef u8 TouchMask;
    static const TouchMask TOUCH_MASK_NONE = 0x0;
    static const TouchMask TOUCH_MASK_UP = 1 << Touch_Up;
    static const TouchMask TOUCH_MASK_LEFT = 1 << Touch_Left;
    static const TouchMask TOUCH_MASK_RIGHT = 1 << Touch_Right;
    static const TouchMask TOUCH_MASK_DOWN = 1 << Touch_Down;
    static const TouchMask TOUCH_MASK_UP_LEFT = 1 << Touch_Up_Left;
    static const TouchMask TOUCH_MASK_UP_RIGHT = 1 << Touch_Up_Right;
    static const TouchMask TOUCH_MASK_DOWN_LEFT = 1 << Touch_Down_Left;
    static const TouchMask TOUCH_MASK_DOWN_RIGHT = 1 << Touch_Down_Right;
    static const TouchMask TOUCH_MASK_ALL = TOUCH_MASK_UP | TOUCH_MASK_LEFT | TOUCH_MASK_RIGHT | TOUCH_MASK_DOWN | TOUCH_MASK_UP_LEFT | TOUCH_MASK_UP_RIGHT | TOUCH_MASK_DOWN_LEFT | TOUCH_MASK_DOWN_RIGHT;

    // 0
    grCollStatus* m_next;
    // 4
    grCollStatus* m_prev;
    // 8
    Setting m_setting : 8;
    // HYPOTHESIS: a signed contact side (the block stages' heading messages accept only 2); read signed (extsb).
    int m_unk9 : 8;
    u32 m_unkA : 8;
    int m_unkB : 8;

    u32 m_unkC : 8;
    int m_cliffSide : 8; // signed field (grCollisionTest::stopEdgeCorrect compares it with 0 / 1) (cluster B); HYPOTHESIS: 0 = left, 1 = right, 2 = both (cliff check side picked by the shape operators)
    // 14
    union {
        struct {
            u16 : 7;
            u16 m_noPassableCheck : 1;
            u16 : 8;
        };
        u16 m_exclusiveLineAttr;
    };
    // 16
    int m_taskId;
    // 20
    char _spacer2a[8];
    Vec2f m_unk1C; // role: reference point the cliff candidates are measured from (grCollisionTest::updateCliffInfo, testCliffCheck) (cluster C)
    char _spacer2c[16];
    // 52
    Vec2f m_cliffPos;
    char _spacer2b[24];
    // 84
    int m_cliffDir : 8;
    // 85
    char m_cliffTeam;
    // 86
    char _spacer3a[2];
    int m_cliffCheckSides; // HYPOTHESIS: 0 = one side, 2 = both sides

    // 92
    grCollShape* m_prevCollShape;
    // 96
    grCollShape* m_currentCollShape;
    // 100
    grCollStatusEventReceiver* m_receiver;
    // 104
    grCollision* m_unk104; // HYPOTHESIS: collision updated before the status test (grCollisionManager::processPreMapCorrection)
    int m_unk6C : 8; // role: 1 = run the attached/air tests in grCollisionTest::test; testRightWallMain etc. skip when 1 (cluster C)
    u8 m_unk6D : 8; // (a byte bitfield: grCollisionTest::testAttached assigns it with lbz/rlwimi/stb) role: bit n set = touch id n is attached (the wall tests check bits 2, 1, 0) (cluster C)
    // 110
    char m_targetLineCategory;
    // 111
    u8 m_unk6F; // role: break-object event mask, bit 7 up / 6 down / 5 left / 4 right (grCollisionTest::testAir -> notifyEventBreakObject) (cluster B)
    // 112
    bool m_attackPointBreak0 : 1;
    bool m_attackPointBreak1 : 1;
    bool m_attackPointBreak2 : 1;
    bool m_attackPointBreak3 : 1;
    bool : 1;
    bool : 1;
    bool : 1;
    bool : 1;
    // 113
    bool m_enabled : 1;
    bool : 1;
    bool : 1;
    bool m_isPassableGround : 1;
    bool m_cliffCheckOn : 1;
    bool : 1;
    bool : 1;
    bool m_isCalcLineMove : 1;
    // 114
    bool : 1;
    bool : 1;
    bool m_cliffStop : 1;
    bool : 1;
    bool m_testCollStop : 1;
    bool m_missStepR : 1;
    bool m_missStepL : 1;
    bool m_isCloudThrough : 1;
    // 115
    bool m_isCloudThroughOut : 1;
    bool m_attackPointFlag : 1;
    bool m_isCorrectIgnoreSlope : 1;
    bool m_tracerType : 1;
    bool m_isEventToGround : 1;
    bool m_ledgeSomething : 1;
    bool m_testDynamicCollision : 1;
    bool : 1;

    char _116[1];

    // 117

    union {
        struct {
            bool m_touchDown : 1;
            bool m_touchUp : 1;
            bool m_touchLeft : 1;
            bool m_touchRight : 1;
            bool m_touchUpRight : 1;
            bool m_touchUpLeft : 1;
            bool m_touchDownRight : 1;
            bool m_touchDownLeft : 1;
        };
        u8 m_touchBits : 8; // all eight touch flags at once (grCollisionTest::test clears them with one read-modify-write) (cluster A)
    };

    // 118
    char _spacer6[6];
    // 124
    u32 m_unk124 : 8; // HYPOTHESIS: update pass, 0 before and 1 during map correction (grCollisionManager)
    u32 : 24;
    // 128
    grCollision* m_collision;
    // 132
    Vec2f m_unk;
    // 140
    Vec2f m_touchLineSpeed;
    // 148
    Vec2f m_unk94; // role: the up shape corner (getTopPos) stored by grCollisionTest::testTopMain (cluster C)
    Vec2f m_unk9C; // role: the left shape corner (getLeftPos) stored by testLeftWallMain (cluster C)
    Vec2f m_unkA4; // role: the right shape corner (getRightPos) stored by testRightWallMain (cluster C)
    Vec2f m_unkAC; // role: the down shape corner (getDownPos) stored by testGround (cluster B, per layout notes)
    // 180
    Vec2f m_180[4]; // role: the normals of the corner touch edges (test() writes them for upLeft, upRight, downLeft, downRight); 0xCC..0xD3 were _spacer8 (cluster A)
    // 212
    grCollisionLineWork m_collLineWorkArray[19];
    // 516
    grCollShape m_collShape;
    // 520
    char _spacer9[116];
    // TOTAL SIZE == 636
    // Local shadow additions (names from the symbol map; HYPOTHESIS signatures).
    static grCollStatus* create(int heap, Setting setting, int unk);
    ~grCollStatus();
    void regist();
    void unRegist();
    void resetStatus();
    void setStGround();
    void setPassableCheck(bool passable);
    void clearIgnoreLine();
    void setAttackPoint(bool a, bool b, bool c, bool d);
    int getTouchLineMaterialType(TouchId touchId);
    void setTouchLineSpeed(Vec2f* speed);
    void getShape(clRhombus2D* out);
    void getPos(Vec2f*);
    u32 getTouchLineId(TouchId touchId);
    bool isOttotto(float f, int dir);
    // The number of touched sides: a plain word counter (no narrowing before the return).
    u32 getTouchCount();
    bool getDownCorrectPos(Vec3f* a, Vec3f* b);
    void entryCliffHang();
    void leaveCliffHang();
    void setCliffRange(Vec2f* a, Vec2f* b, float f);
    void getShapePrev(clRhombus2D* out);
    void test();
    bool isTouch(TouchId touchId);
    bool getTouchLineNormal(TouchId touchId, Vec2f* out);
    bool checkCliffEntryOverlap();
    void setStAir();
    void setStCliff();
    bool isCliffOver(int side);
    bool isCliffOverPos(Vec2f pos, int side);
    void drawDebug();
    void drawDebugCliff();
    void setIgnoreLine(u32* lineId);
};
static_assert(sizeof(grCollStatus) == 636, "Class is wrong size!");

class grCollStatusEventReceiver {
public:
    virtual void receive(int, bool);
};