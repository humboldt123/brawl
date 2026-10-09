#include <gr/gr_calc_world_callback.h>
#include <memory.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

inline grHalberdEnkei::grHalberdEnkei(const char* taskName) : grHalberd(taskName) {
    m_mtxWork = NULL;
    m_hasSrt = 0;
    m_motion = 2;
    m_motionTimer = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grHalberdEnkei* grHalberdEnkei::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdEnkei* ground = new (Heaps::StageInstance) grHalberdEnkei(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdEnkei::~grHalberdEnkei() {
}

void grHalberdEnkei::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grHalberdEnkei::updateMotion(float deltaFrame) {
    if (m_frameWork == NULL) {
        return;
    }
    if (m_stateWork == NULL) {
        return;
    }
    switch (m_state) {
    case 0:
        setMotion(2, 0, 1, NULL);
        m_state = 2;
        // fall through
    case 2:
        if (*m_stateWork == 2) {
            setMotion(0, 0, 1, &m_motionTimer);
            setMotionFrame(*m_frameWork, 0);
            m_state = 3;
        }
        break;
    }
    if (m_motion == 1) {
        if (*m_stateWork == 2) {
            setMotion(0, 0, 1, &m_motionTimer);
            setMotionFrame(*m_frameWork, 0);
        }
    } else if (m_motion == 0 && getMotionFrame(0) >= m_motionTimer) {
        setMotion(1, 1, 1, NULL);
        m_hasSrt = 1;
    }
}

void grHalberdEnkei::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_mtxWork != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
            }
        }
    }
}

// The animation of the backdrop (2 animations; the texture animation is played only until the first one ends).
void grHalberdEnkei::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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
    modelAnim->unbindMatColAnim(sceneMdl);
    if (m_hasSrt == 0) {
        modelAnim->unbindTexSrtAnim(sceneMdl);
    }
    m_motion = animId;

    if (animId >= 2) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grHalberdSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grHalberdSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    if (m_hasSrt == 0) {
        result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
        if (result) {
            grHalberdSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
        }
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->m_anmObjChrRes->SetFrame(0.0f);
    modelAnim->m_anmObjMatClrRes->SetFrame(0.0f);
    if (m_hasSrt == 0) {
        modelAnim->m_anmObjTexSrtRes->SetFrame(0.0f);
    }
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoopNode(shouldLoop);
    modelAnim->setLoopMatCol(shouldLoop);
    if (m_hasSrt == 0) {
        modelAnim->setLoopTexSrt(shouldLoop);
    }

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

void grHalberdEnkei::setMotionFrame(float frame, u32 animIndex) {
    gfModelAnimation* modelAnim = m_modelAnims[animIndex];
    if (modelAnim != NULL) {
        if (m_hasSrt == 0) {
            modelAnim->setFrame(frame);
        } else {
            modelAnim->m_anmObjChrRes->SetFrame(frame);
            modelAnim->m_anmObjMatClrRes->SetFrame(frame);
        }
    }
}

float grHalberdEnkei::getMotionFrame(u32 animIndex) {
    gfModelAnimation* modelAnim = m_modelAnims[animIndex];
    float frame = 0.0f;
    if (modelAnim != NULL) {
        frame = modelAnim->m_anmObjChrRes->GetFrame();
    }
    return frame;
}
