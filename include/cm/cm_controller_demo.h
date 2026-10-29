#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <gf/gf_camera_controller.h>
#include <mt/mt_vector.h>
#include <types.h>

class cmDemoController : public gfCameraController {
public:
    s32 m_targetType; // HYPOTHESIS: the value setTargetType stores
    float unkC;
    u8 unk10;
    float unk14;
    float unk18;
    char _1C[4];
    Rect2D unk20;
    char _30[0x18];
    float unk48;
    s32 unk4C;
    s32 unk50;
    float unk54;
    float unk58;

    cmDemoController();
    ~cmDemoController();
    void setTargetType(int type);
    // HYPOTHESIS: the parameter is the player number (negative numbers find nothing).
    static cmSubject* getFighterSubject(int playerNo);
    bool isForceCamControl3D();
    virtual void update(float);
};
static_assert(sizeof(cmDemoController) == 0x5C, "Class is wrong size!");
