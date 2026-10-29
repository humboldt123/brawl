#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <yk/yk_no_hit_normal.h>
#include <types.h>

class grVisibleProductionFade : public grVisibleProduction {
public:
    // HYPOTHESIS: kind 0 starts opaque and fades out, kind 1 starts transparent and fades in.
    enum Kind {
        Kind_FadeOut = 0,
        Kind_FadeIn = 1,
    };

    nw4r::g3d::ScnMdl* m_sceneModel; // 0x10
    float m_length;                  // 0x14
    float m_frame;                   // 0x18
    float m_alphaStep;               // 0x1C
    float m_alpha;                   // 0x20
    int m_kind;                      // 0x24

    grVisibleProductionFade(nw4r::g3d::ScnMdl* sceneModel, int kind, float length) : m_length(length), m_frame(0.0f), m_alphaStep(255.0f / length), m_alpha(0.0f), m_kind(kind) {
        m_sceneModel = ykDynamicCastScnMdl(sceneModel);
        initialize();
    }
    static grVisibleProductionFade* create(nw4r::g3d::ScnMdl* sceneModel, int kind, float length);
    virtual void initialize();
    virtual void remove();
    virtual void productFlowStandby();
    virtual void setStart();
    virtual void productFlowStart();
    virtual void productFlowProducing(float deltaFrame);
    virtual void productFlowEnd();
    void changeMaterialAlpha();
    virtual void setForcedEnding();
};
static_assert(sizeof(grVisibleProductionFade) == 0x28, "Class is wrong size!");
