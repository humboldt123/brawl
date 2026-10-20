#pragma once

#include <StaticAssert.h>
#include <gr/collision/gr_collision_status.h>
#include <mt/mt_vector.h>
#include <types.h>

// Local shadow of BrawlHeaders/so/ground/so_ground_shape_impl.h: declares the out-of-line soGroundShapeImpl members.
// HYPOTHESIS: parameter types are recovered from the callers in soGroundModuleImpl.

class soModuleAccesser;
class grCollStatusEventReceiver;

// Static description of one ground shape (HYPOTHESIS layout; first byte is the ShapeKind).
struct soGroundShapeData {
    u8 m_kind;
    char unk1[3];
    union {
        float m_f4;
        int m_followNode;
    };
    float m_f8;
    float m_fC;
    float m_f10;
};

class soGroundShapeImpl {
public:
    enum ShapeKind {
        Shape_Rhombus_Modify = 0x0,
        Shape_Rhombus = 0x1,
        Shape_Rhombus_Follow_Node = 0x2,
    };

    enum ShapeFlagId {
        Shape_Flag_Ground = 0x0,
        Shape_Flag_Ignore_Scale = 0x1,

        Shape_Rhombus_Flag_Min_L = 0x3,
        Shape_Rhombus_Flag_Min_R = 0x4,
        Shape_Rhombus_Flag_Min_U = 0x5,
        Shape_Rhombus_Flag_Min_D = 0x6,

        Shape_Rhombus_Modify_Flag_Fix = 0x3,
        Shape_Rhombus_Modify_Flag_No_OFS = 0x4,
        Shape_Rhombus_Modify_Flag_Min_U = 0x5,
        Shape_Rhombus_Modify_Flag_Min_LR = 0x6,
        Shape_Rhombus_Modify_Flag_No_Modify = 0x7,
        Shape_Rhombus_Modify_Flag_Free_Y = 0x8,
        Shape_Rhombus_Modify_Flag_Front_Fix = 0x9,
        Shape_Rhombus_Modify_Flag_Up_Fix = 0xa,
    };

    enum CorrectKind {
        Correct_None = 0x0,
        Correct_Ground = 0x1,
        Correct_Ground_Cliff_Stop = 0x2,
        Correct_Ground_Ottotto = 0x3,
        Correct_Cliff = 0x4,
        Correct_Air = 0x5,
        Correct_Air_Passable = 0x6,
        Correct_Keep = 0x7,
        Correct_Ground_Cliff_Stop_Front = 0x8,
    };

    enum CliffCheckKind {
        Cliff_Check_None = 0x0,
        Cliff_Check_Always = 0x1,
        Cliff_Check_Always_Both_Sides = 0x2,
        Cliff_Check_On_Drop = 0x3,
        Cliff_Check_On_Drop_Both_Sides = 0x4,
    };

    soGroundShapeData* m_data;
    grCollStatus* m_collStatus;
    Vec2f m_savePos;
    Vec2f m_offset;
    float m_24;
    float m_28;
    float m_rhombusUp;
    float m_rhombusDown;
    float m_rhombusWidth;
    int m_rhombusFollowNode;
    float m_downFriction;
    u8 m_collMaterialType;
    char _53;
    union {
        struct {
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool : 1;
            bool m_ignoreScale : 1;
            bool m_ground : 1;
        };
        union {
            struct {
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool m_upFix : 1;
                bool m_frontFix : 1;
                bool m_freeY : 1;
                bool m_noModify : 1;
                bool m_minLR : 1;
                bool m_minU : 1;
                bool m_noOFS : 1;
                bool m_fix : 1;
                bool : 1;
                bool : 1;
                bool : 1;
            } m_rhombus;
            struct {
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool : 1;
                bool m_minD : 1;
                bool m_minU : 1;
                bool m_minR : 1;
                bool m_minL : 1;
                bool : 1;
                bool : 1;
                bool : 1;
            } m_rhombusModify;
        };
    } m_flag;

    u8 m_correctKind;
    u8 m_cliffCheck;
    u32 m_shapeKind : 4;
    u32 : 8;
    u32 m_posReset : 1;
    u32 m_testPreMap : 1;
    u32 m_testDynamicCollision : 1;
    u32 m_testCollStop : 1;
    u32 m_eventToGround : 1;
    u32 : 31;

    soGroundShapeImpl();
    virtual ~soGroundShapeImpl();

    void clean();
    void setup(int taskId, soGroundShapeData* data, int heap, u8 flag, grCollStatusEventReceiver* receiver);
    void activate(int taskId, Vec2f* pos);
    void deactivate();
    void initShape();
    void reset(Vec2f* pos);
    void resetStatus();
    void clearSetting();
    void updateDownMaterialInfo();
    void setEventToGround(bool eventToGround, u32 store);
    void setTestCollStop(bool testCollStop, u32 store);
    void setTestPreMap(bool testPreMap, u32 store);
    void setTestDynamicCollision(bool testDynamicCollision, u32 store);
    void setCalcLineMove(bool calcLineMove);
    void setTargetLineCategory(u8 category);
    void setExclusiveLineAttr(short attr);
    void setPassableCheck(bool passableCheck);
    bool isPassableCheck();
    void setCloudThrough(bool cloudThrough);
    bool isCloudThrough();
    bool isCloudThroughOut(Vec2f* out);
    void setCorrectIgnoreSlope(bool ignore);
    void enable();
    void unable();
    void setFlag(int flag, bool set);
    void setSafePos(Vec2f* pos);
    void setSpeed(Vec2f* speed);
    void setTracerType(bool type);
    void attach(u8 kind);
    bool isAttach();
    float getCrushRateUD();
    float getCrushRateLR();
    void detach();
    void getAttackPointBreak(bool* a, bool* b, bool* c, bool* d);
    void modify(float up, float down, float width);
    void modify(float up, float down, float width, int followNode);
    void getRhombus(soModuleAccesser* moduleAccesser, float* up, float* down, float* width);
    void getRhombus(soModuleAccesser* moduleAccesser, float* up, float* down, float* width, int* followNode);
    void setAttackPoint(bool a, bool b, bool c, bool d, bool flag);
    void getCorrectPos(Vec2f* out, float scale);
    void update(soModuleAccesser* moduleAccesser, int state);
    void calcRhombusModify(soModuleAccesser* moduleAccesser, float* out1, float* out2, float* out3, float* out4);
    void updateRhombus(soModuleAccesser* moduleAccesser);
    void updateRhombusFollowNode(soModuleAccesser* moduleAccesser);
    void updateRhombusModify(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soGroundShapeImpl) == 68, "Class is wrong size!");