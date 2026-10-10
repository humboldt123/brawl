#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_norfair/gr_norfair.h>
#include <st_norfair/gr_norfair_anim.h>

grNorfairShutter* grNorfairShutter::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairShutter* ground = new (Heaps::StageInstance) grNorfairShutter(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grNorfairShutter::~grNorfairShutter() {
}

void grNorfairShutter::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The shutter comes down when the wave comes (the event 4) and goes up again after it.
void grNorfairShutter::updateActive(float deltaFrame) {
    stNorfairData* data = static_cast<stNorfairData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        m_frameCount = m_frameCount - deltaFrame;
        if (m_frameCount < 0.0f) {
            m_frameCount = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(2, false, true, NULL);
            setVisibility(0);
            m_state = 1;
            break;
        case 1:
            if (*m_eventIDWork == 4) {
                m_timer = data->unk50;
                m_state = 6;
            }
            break;
        case 6:
            if (m_timer == 0.0f) {
                setMotion(0, false, true, &m_frameCount);
                m_state = 7;
            }
            break;
        case 7:
            if (!m_isVisible) {
                setVisibility(1);
            }
            if (*m_eventIDWork != 4) {
                m_timer = data->unk54;
                m_state = 8;
            }
            break;
        case 8:
            if (m_timer == 0.0f) {
                setMotion(1, false, true, &m_frameCount);
                m_state = 9;
            }
            break;
        case 9:
            if (m_frameCount == 0.0f) {
                m_state = 0;
            }
            break;
        }
    }
}

void grNorfairShutter::updateCallBack(float deltaFrame) {
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
            Vec3f* posWork = m_posWork;
            if (posWork != NULL) {
                Vec3f* pos = &posWork[*m_posIndex];
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = pos->m_x;
                data->m_pos.m_y = pos->m_y;
                data->m_pos.m_z = pos->m_z;
            }
        }
    }
}

// The animations of the shutter are bound with all their parts, the character animation first.
void grNorfairShutter::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    if (animId >= 2) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grNorfairSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grNorfairSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grNorfairSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grNorfairSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grNorfairSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
