#pragma once

#include <StaticAssert.h>
#include <gf/gf_camera.h>
#include <gf/gf_camera_controller.h>
#include <types.h>

class cmMenuPadController : public gfCameraController {
    u32 unk8;
    float unkC;
    bool unk10 : 1;
    float unk14;
    Vec3f unk18;
    float unk24;
public:
    cmMenuPadController();
    void storeDefault();
    void init();
    virtual void update(float);
};
static_assert(sizeof(cmMenuPadController) == 0x28, "Class is wrong size!");
