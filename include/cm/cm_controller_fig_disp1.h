#pragma once

#include <StaticAssert.h>
#include <gf/gf_camera_controller.h>
#include <types.h>

class cmFigDisp1Controller : public gfCameraController {
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
    float unk38;
    u32 unk3C;
public:
    cmFigDisp1Controller();
    // HYPOTHESIS: the parameter is the displayed object's width (the constructor passes 26.0f).
    void setCamParamByObjWidth(float width);
    virtual void update(float);
    virtual ~cmFigDisp1Controller();
};
static_assert(sizeof(cmFigDisp1Controller) == 0x40, "Class is wrong size!");
