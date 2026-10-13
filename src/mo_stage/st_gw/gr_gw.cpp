#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <string.h>

#include <st_gw/gr_gw.h>
#include <st_gw/gr_gw_anim.h>
#include <st_gw/gr_gw_item.h>

grGW* grGW::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGW* ground = new (Heaps::StageInstance) grGW(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGW::grGW(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_sceneIdWork = NULL;
    m_sceneId = 4;
    m_motion = 1;
    m_frameLast = 0.0f;
    m_frameCount = 0.0f;
    m_scale.m_x = 0.0f;
    m_scale.m_y = 0.0f;
    m_scale.m_z = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 4;
        setupMelee();
    }
}

grGW::~grGW() {
}

void grGW::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

static inline bool gwNearZero(float v) {
    bool result = false;
    if ((float)fabs(v) < 1e-5f) {
        result = true;
    }
    return result;
}

// Reads the scale of the root node of the model once (the models are authored too big and shrunk by 0.9 in the callback).
void grGW::updateScaleBase(float deltaFrame) {
    bool unset = false;
    if (gwNearZero(m_scale.m_x) && gwNearZero(m_scale.m_y) && gwNearZero(m_scale.m_z)) {
        unset = true;
    }
    if (unset) {
        nw4r::g3d::ResMdl model;
        if (m_sceneModels[0] != NULL) {
            model = m_sceneModels[0]->m_resMdl;
            if (model.IsValid()) {
                nw4r::g3d::ResNode node = model.GetResNode((u32)m_nodeIndex);
                if (node.IsValid()) {
                    m_scale.m_x = node.ptr()->m_scale.m_x;
                    m_scale.m_y = node.ptr()->m_scale.m_y;
                    m_scale.m_z = node.ptr()->m_scale.m_z;
                }
            }
        }
    }
}

void grGW::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f scale;
            gwVec3Scale(&scale, &m_scale, 0.9f);
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// Only the animation 0 exists (any other index just clears the bound ones).
void grGW::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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
    bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grGWSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }
    result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grGWSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }
    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grGWSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }
    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grGWSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }
    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grGWSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }
    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);
    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
