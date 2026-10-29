// Local shadow for native methods absent from BrawlHeaders.
#pragma once

#include <cm/cm_controller_ai_param.h>
#include <gf/gf_camera_controller.h>
#include <mt/mt_vector.h>

class cmAIController : public gfCameraController {
public:
    // HYPOTHESIS: layouts of the two blocks the constructor copies from the unit's default tables.
    struct DefaultParam {
        float unk[25];
    };
    struct DefaultParam2 {
        float unk[6];
    };

    // HYPOTHESIS: a signed camera mode in the top byte (resetInternalCameraParam tests it for 1); stages set it.
    s32 unk8 : 8;
    u32 unk8_rest : 24;
    cmCameraParam m_param;
    cmCameraParam m_internalParam;
    u32 unk84;
    u16 unk88;
    u32 unk8C;
    u16 unk90;
    char unk92[2];
    u8 unk94_80 : 1;
    u8 unk94_40 : 1;
    u8 unk94_20 : 1;
    u8 unk94_10 : 1;
    u8 unk94_08 : 1;
    u8 unk94_04 : 1;
    u8 unk94_02 : 1;
    u8 unk94_01 : 1;
    u8 unk95_80 : 1;
    u8 unk95_rest : 7;
    // HYPOTHESIS: the aspect ratio copied from gfCameraManager by the constructor.
    float m_aspect;
    Vec3f m_userRot;
    Vec2f m_userPos;
    Vec2f m_devilOffset;
    float m_userDistMin;
    DefaultParam unkBC;
    DefaultParam2 unk120;
    void* unk138;
    void* unk13C;
    void* unk140;
    char unk144[0x18];
    Rect2D unk15C;
    Rect2D unk16C;
    float unk17C;
    float unk180;
    float unk184;
    Rect2D unk188;

    static cmAIController* create();
    cmAIController();
    virtual void update(float p1);
    virtual void draw();
    virtual ~cmAIController();
    virtual void updateCameraPosTarget();

    void reset();
    void setUserDistMin(float distance);
    void updateMeleeFix(float p1);
    void resetDevilOffset();
    void accDevilOffset(Vec2f*);
    // HYPOTHESIS: sets the camera offset of the scripted camera (a rotation offset and a position offset); only the Pikmin
    // final smash article calls it.
    void setUserOffset(const Vec3f& rot, const Vec2f& pos);

};
static_assert(sizeof(cmAIController) == 0x198, "Class is wrong size!");

extern cmAIController* g_cmAIController;
