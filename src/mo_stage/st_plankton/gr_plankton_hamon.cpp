#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>

#include <st_plankton/gr_plankton.h>
#include <st_plankton/gr_plankton_anim.h>

// HYPOTHESIS: the same helpers as nw4r's ScnObj uses to switch the callback timing and ops on (main, unnamed)
extern "C" void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* object, u32 timing);
extern "C" void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* object, u32 op);

static const char sHamonMaterial[] = "hamon";

void grPlanktonHamonScnObjCallBack::SetCallBackCondition(nw4r::g3d::ScnObj* object) {
    ScnObj_EnableCallbackTiming(object, 2);
    ScnObj_EnableCallbackExecOp(object, 4);
}

// Sets the two tev colors of the material of the ripple.
void grPlanktonHamonScnObjCallBack::ExecCallback_CALC_MAT(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* object, u32 param, void* info) {
    nw4r::g3d::ScnMdl* scnMdl = static_cast<nw4r::g3d::ScnMdl*>(object);
    if (scnMdl != NULL && timing != 3 && timing > 2 && timing < 5) {
        nw4r::g3d::ResMatTevColor tevColor;
        nw4r::g3d::ResMat mat;
        nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
        if (model.IsValid()) {
            mat = model.GetResMat(sHamonMaterial);
            if (mat.IsValid()) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
                tevColor = access.GetResMatTevColor(0);
                if (tevColor.IsValid()) {
                    GXColor color;
                    GXColor* source = m_color[0];
                    color.r = source->r;
                    color.g = source->g;
                    color.b = source->b;
                    color.a = source->a;
                    tevColor.GXSetTevColor(GX_TEVREG1, color);
                    source = m_color[1];
                    color.r = source->r;
                    color.g = source->g;
                    color.b = source->b;
                    color.a = source->a;
                    tevColor.GXSetTevColor(GX_TEVREG2, color);
                    tevColor.DCStore(false);
                    mat.DCStore(false);
                }
            }
        }
    }
}

void grPlanktonHamonScnObjCallBack::setColor(u8 index, GXColor* color) {
    if (index > 1) {
        return;
    }
    m_color[index] = color;
}

grPlanktonHamon* grPlanktonHamon::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPlanktonHamon* ground = new (Heaps::StageInstance) grPlanktonHamon(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPlanktonHamon::grPlanktonHamon(const char* taskName) : grPlankton(taskName) {
    *reinterpret_cast<u32*>(&m_color1) = 0xFFFFFFFF;
    *reinterpret_cast<u32*>(&m_color2) = 0xFFFFFFFF;
    m_state = 0;
    m_timer = 0.0f;
    m_hamon = NULL;
    m_color1.r = 0;
    m_color1.g = 0;
    m_color1.b = 0;
    m_color1.a = 0;
    m_color2.r = 0;
    m_color2.g = 0;
    m_color2.b = 0;
    m_color2.a = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
}

grPlanktonHamon::~grPlanktonHamon() {
    if (m_sceneModels[0] != NULL) {
        // HYPOTHESIS: the callback of the scene model is taken off (offset 0xd4 of the scene model)
        *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_sceneModels[0]) + 0xD4) = NULL;
    }
}

void grPlanktonHamon::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// A ripple waits (0), starts its animation when the stage makes it (1), takes the colors of its kind (8) and goes back to
// waiting after its animation.
void grPlanktonHamon::updateActive(float deltaFrame) {
    switch (m_state) {
    case 0:
        setMotion(1, false, true, NULL);
        setVisibility(0);
        m_state = 1;
        // fall through
    case 1:
        if (m_hamon->m_state == 0) {
            setMotion(0, false, true, &m_wait);
            stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
            if (data != NULL) {
                GXColor* first = &data->unk7C[m_hamon->m_kind];
                m_color1.r = first->r;
                m_color1.g = first->g;
                m_color1.b = first->b;
                m_color1.a = first->a;
                GXColor* second = &data->unkB8[m_hamon->m_kind];
                m_color2.r = second->r;
                m_color2.g = second->g;
                m_color2.b = second->b;
                m_color2.a = second->a;
                m_state = 8;
            }
        }
        break;
    case 8:
        if (!m_isVisible) {
            setVisibility(1);
        }
        if (getMotionFrame(0) >= m_wait) {
            m_hamon->m_x = 0.0f;
            m_hamon->m_y = 0.0f;
            m_hamon->m_state = 3;
            m_hamon->m_kind = 0;
            setVisibility(0);
            m_state = 0;
        }
        break;
    }
}

void grPlanktonHamon::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
                m_scnObjCallback.SetCallBackCondition(scnMdl);
                *reinterpret_cast<void**>(reinterpret_cast<u8*>(scnMdl) + 0xD4) = &m_scnObjCallback;
            }
            if (m_hamon != NULL) {
                grNodeCallbackData* data = &calcWorldCallBack->m_nodeCallbackDatas[0];
                data->m_pos.m_x = m_hamon->m_x;
                data->m_pos.m_y = m_hamon->m_y;
                data->m_pos.m_z = 0.0f;
                m_scnObjCallback.setColor(0, &m_color1);
                m_scnObjCallback.setColor(1, &m_color2);
            }
        }
    }
}

// The animation of the ripple (two: it starts hidden and plays once).
void grPlanktonHamon::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_motion = animId;

    if (animId >= 1) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grPlanktonSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grPlanktonSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grPlanktonSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grPlanktonSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grPlanktonSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
