#pragma once

#include <StaticAssert.h>
#include <gf/gf_camera_controller.h>
#include <mt/mt_vector.h>
#include <types.h>

// HYPOTHESIS: layout from the constructor's stores; field meanings are unknown.
struct cmFighterTargetControllerWork {
    Vec3f unk0;
    Vec2f unkC;
    Vec2f unk14;
    float unk1C;
};

class cmFighterTargetController : public gfCameraController {
public:
    u32 unk8;
    u32 m_targetIndex; // HYPOTHESIS: index of the followed fighter subject (getFighterPos resets it to 0)
    cmFighterTargetControllerWork unk10[4];
    float unk90;

    cmFighterTargetController();
    Vec3f getFighterPos();
    void resetTarget();
    virtual void update(float);
};
static_assert(sizeof(cmFighterTargetController) == 0x94, "Class is wrong size!");
