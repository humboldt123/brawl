#pragma once

#include <StaticAssert.h>
#include <gf/gf_camera_controller.h>
#include <mt/mt_vector.h>
#include <types.h>

class cmTechController : public gfCameraController {
public:
    void* m_target; // HYPOTHESIS: the object start() receives; resetTarget clears it
    u16 unkC;
    u16 unkE;
    u8 unk10;
    bool unk11_80 : 1;
    bool unk11_40 : 1;
    float unk14;
    Vec3f unk18;
    Vec3f unk24;
    Vec3f unk30;
    Vec3f unk3C;
    Vec3f unk48;
    float unk54;

    cmTechController();
    ~cmTechController();
    void resetTarget();
    virtual void update(float);
};
static_assert(sizeof(cmTechController) == 0x58, "Class is wrong size!");
