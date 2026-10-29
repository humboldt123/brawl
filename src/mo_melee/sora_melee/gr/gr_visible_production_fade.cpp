#include <gr/gr_visible_production_fade.h>
#include <mu/mu_mat_color.h>
#include <types.h>

extern "C" nw4r::g3d::ResMatChan GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool sync);

// MATCH-ONLY: the real CopiedMatAccess is 0x34 bytes; the BrawlHeaders copy is 4 bytes short (as in gr_battlefield.cpp).
struct grVisibleProductionFadeMatAccess : public nw4r::g3d::ScnMdl::CopiedMatAccess {
    u32 m_pad;
    grVisibleProductionFadeMatAccess(nw4r::g3d::ScnMdl* sceneModel, u32 id) : CopiedMatAccess(sceneModel, id) { }
};

grVisibleProductionFade* grVisibleProductionFade::create(nw4r::g3d::ScnMdl* sceneModel, int kind, float length) {
    return new (Heaps::StageInstance) grVisibleProductionFade(sceneModel, kind, length);
}

void grVisibleProductionFade::initialize() { }

void grVisibleProductionFade::remove() { }

void grVisibleProductionFade::productFlowStandby() { }

void grVisibleProductionFade::setStart() {
    nw4r::g3d::ResMdl resMdl = m_sceneModel->m_resMdl;
    for (u32 i = 0; i < resMdl.GetResMatNumEntries(); i++) {
        nw4r::g3d::ResMat resMat = resMdl.GetResMat(i);
        if (resMat.IsValid()) {
            grVisibleProductionFadeMatAccess access(m_sceneModel, resMat.ref().m_id);
            nw4r::g3d::ResMatChan resMatChan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access, false);
            if (resMatChan.IsValid()) {
                GXColor color;
                if (m_kind == Kind_FadeOut) {
                    m_alpha = 255.0f;
                    color.a = m_alpha;
                }
                if (m_kind == Kind_FadeIn) {
                    m_alpha = 0.0f;
                    color.a = m_alpha;
                }
                resMatChan.GXSetChanMatColor(GX_ALPHA0, color);
            }
            resMat.DCStore(false);
        }
    }
    m_frame = 0.0f;
    setState(VisibleProduction_Executing);
}

void grVisibleProductionFade::productFlowStart() { }

void grVisibleProductionFade::productFlowProducing(float deltaFrame) {
    changeMaterialAlpha();
    m_frame += deltaFrame;
    if (m_kind == Kind_FadeOut) {
        m_alpha -= m_alphaStep;
    } else if (m_kind == Kind_FadeIn) {
        m_alpha += m_alphaStep;
    }
    if (m_length <= m_frame) {
        setState(VisibleProduction_Finished);
    }
}

void grVisibleProductionFade::productFlowEnd() {
    setState(VisibleProduction_Standby);
}

void grVisibleProductionFade::changeMaterialAlpha() {
    nw4r::g3d::ResMdl resMdl = m_sceneModel->m_resMdl;
    for (u32 i = 0; i < resMdl.GetResMatNumEntries(); i++) {
        nw4r::g3d::ResMat resMat = resMdl.GetResMat(i);
        if (resMat.IsValid()) {
            grVisibleProductionFadeMatAccess access(m_sceneModel, resMat.ref().m_id);
            nw4r::g3d::ResMatChan resMatChan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access, false);
            if (resMatChan.IsValid()) {
                GXColor color;
                resMatChan.GXGetChanMatColor(GX_ALPHA0, &color);
                if (m_kind == Kind_FadeOut) {
                    color.a = m_alpha < 0.0f ? 0 : (u8)m_alpha;
                }
                if (m_kind == Kind_FadeIn) {
                    color.a = m_alpha > 255.0f ? 255 : (u8)m_alpha;
                }
                resMatChan.GXSetChanMatColor(GX_ALPHA0, color);
            }
            resMat.DCStore(false);
        }
    }
}

void grVisibleProductionFade::setForcedEnding() {
    setState(VisibleProduction_Finished);
}
