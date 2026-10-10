#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>

#include <st_palutena/gr_palutena.h>
#include <st_palutena/gr_palutena_anim.h>

inline grPalutenaAshibaBreak::grPalutenaAshibaBreak(const char* taskName) : grPalutena(taskName) {
    m_translate.m_x = 0.0f;
    m_translate.m_y = 0.0f;
    m_translate.m_z = 0.0f;
    m_motion = 1;
    m_timer = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
}

grPalutenaAshibaBreak* grPalutenaAshibaBreak::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaAshibaBreak* ground = new (Heaps::StageInstance) grPalutenaAshibaBreak(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaAshibaBreak::~grPalutenaAshibaBreak() {
}

void grPalutenaAshibaBreak::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateBreak(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The model hides until the platform breaks (state 0 -> 1), then it plays its animation (2) and hides again (3).
void grPalutenaAshibaBreak::updateBreak(float deltaFrame) {
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(1, false, true, NULL);
        setVisibility(0);
        m_state = 1;
        break;
    case 2:
        if (m_timer == 0.0f) {
            setMotion(1, false, true, NULL);
            setVisibility(0);
            m_state = 3;
        }
        break;
    default:
        break;
    }
}

void grPalutenaAshibaBreak::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = m_nodeIndex;
            }
            grNodeCallbackData* data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            data->m_offsetPos.m_x = m_translate.m_x;
            data->m_offsetPos.m_y = m_translate.m_y;
            data->m_offsetPos.m_z = m_translate.m_z;
        }
    }
}

// The animation of the broken platform (only the first one is bound).
void grPalutenaAshibaBreak::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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
        grPalutenaSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grPalutenaSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grPalutenaSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grPalutenaSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grPalutenaSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

void grPalutenaAshibaBreak::getTranslate_Delta(Vec3f* out) {
    if (out == NULL) {
        return;
    }
    out->m_x = m_translate.m_x;
    out->m_y = m_translate.m_y;
    out->m_z = m_translate.m_z;
}

void grPalutenaAshibaBreak::requestBuild() {
    if (m_state > 3) {
        return;
    }
    if (m_state < 2) {
        return;
    }
    m_state = 0;
}

void grPalutenaAshibaBreak::requestBreak() {
    if (m_state == 1) {
        setMotion(0, false, true, &m_timer);
        setVisibility(1);
        m_state = 2;
    }
}
