#pragma once

#include <StaticAssert.h>
#include <gf/gf_model.h>
#include <gr/gr_gimmick.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

class grVisibleProductionMaterialMotion : public grVisibleProduction {
public:
    nw4r::g3d::ScnMdl* m_sceneModel; // 0x10
    gfModelAnimation* m_modelAnim;   // 0x14
    u32 m_animIndex;                 // 0x18

    grVisibleProductionMaterialMotion(nw4r::g3d::ScnMdl* sceneModel, gfModelAnimation* modelAnim, u32* animIndex) : m_sceneModel(sceneModel), m_modelAnim(modelAnim), m_animIndex(*animIndex) {
        modelAnim->m_resFile.GetResAnmClr(m_animIndex);
        initialize();
    }
    // HYPOTHESIS: the parameter types (gr_adventure_motion_ground.cpp still calls this by address).
    static grVisibleProductionMaterialMotion* create(nw4r::g3d::ScnMdl* sceneModel, gfModelAnimation* modelAnim, u32* animIndex);
    virtual void initialize();
    virtual void remove();
    virtual void productFlowStandby();
    virtual void productFlowStart();
    virtual void productFlowProducing(float deltaFrame);
    virtual void productFlowEnd();
    virtual void setForcedEnding();
};
static_assert(sizeof(grVisibleProductionMaterialMotion) == 0x1C, "Class is wrong size!");
