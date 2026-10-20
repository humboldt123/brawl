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
    u32 m_unk9 : 16;
    int m_unkB : 8;

    u32 m_unkC : 8;
    u32 m_cliffSide : 8; // HYPOTHESIS: 0 = left, 1 = right, 2 = both (cliff check side picked by the shape operators)
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
    char _spacer2a[32];
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
    char _spacer4[6];
    // 110
    char m_targetLineCategory;
    // 111
    char _spacer5[1];
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

    bool m_touchDown : 1;
    bool m_touchUp : 1;
    bool m_touchLeft : 1;
    bool m_touchRight : 1;
    bool m_touchUpRight : 1;
    bool m_touchUpLeft : 1;
    bool m_touchDownRight : 1;
    bool m_touchDownLeft : 1;

    // 118
    char _spacer6[10];
    // 128
    grCollision* m_collision;
    // 132
    Vec2f m_unk;
    // 140
    Vec2f m_touchLineSpeed;
    // 148
    char _spacer7[32];
    // 180
    Vec2f m_180[3];
    // 204
    char _spacer8[8];
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
    u8 getTouchCount();
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
    void drawDebug();
    void drawDebugCliff();
    void setIgnoreLine(u32* lineId);
};
static_assert(sizeof(grCollStatus) == 636, "Class is wrong size!");

class grCollStatusEventReceiver {
public:
    virtual void receive(int, bool);
};