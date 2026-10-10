#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_id.h>

#include <st_kart/gr_kart.h>
#include <st_kart/gr_kart_anim.h>

grKartWarning* grKartWarning::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grKartWarning* ground = new (Heaps::StageInstance) grKartWarning(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grKartWarning::grKartWarning(const char* taskName) : grKart(taskName) {
    m_pos.m_x = -50.0f;
    m_pos.m_y = 60.0f;
    m_pos.m_z = 0.0f;
    m_stateWork = NULL;
    m_motion = 0;
    m_motionTimer = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    m_seId[0] = snd_se_stage_Kart_info;
    m_seData[0].id = snd_se_stage_Kart_info;
    m_seData[0].unk4 = 0.0f;
    m_seData[0].unk8 = 0.0f;
    m_seData[0].unkC = 0.0f;
    m_seSeq.registId(m_seId, 1);
    m_seSeq.registSeq(0, m_seData, 1, Heaps::StageInstance);
}

grKartWarning::~grKartWarning() {
}

void grKartWarning::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The warning shows up when the stage asks for it (0): the siren sounds and the light blinks until the timer runs out.
void grKartWarning::updateActive(float deltaFrame) {
    stKartData* param = static_cast<stKartData*>(getStageData());
    if (param != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 1:
            if (*m_stateWork == 0) {
                setMotion(0, true, false, &m_motionTimer);
                setVisibility(1);
                m_seSeq.playFrame(0, getMotionFrame(0), 0.0f);
                m_timer = param->unk18;
                m_state = 6;
            }
            break;
        case 0:
            setMotion(1, true, false, NULL);
            setVisibility(0);
            m_state = 1;
            break;
        case 6:
            m_seSeq.playFrame(0, getMotionFrame(0));
            if (m_timer == 0.0f) {
                *m_stateWork = 10;
                m_state = 0;
            }
            break;
        }
    }
}

void grKartWarning::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_x = m_pos.m_x;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_y = m_pos.m_y;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_z = m_pos.m_z;
        }
    }
}

// The animation of the warning (only the first one is bound; the others have no animation of their own).
void grKartWarning::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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

    if (animId == 0) {
        bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > 0);
        if (result) {
            grKartSetChrAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > 0);
        if (result) {
            grKartSetVisibilityAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > 0);
        if (result) {
            grKartSetTexPatAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > 0);
        if (result) {
            grKartSetTexSrtAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > 0);
        if (result) {
            grKartSetColorAnim2(0, model, modelAnim, Heaps::StageInstance);
        }

        gfModelAnimation::bind(sceneMdl, modelAnim);
        modelAnim->setFrame(0.0);
        modelAnim->setUpdateRate(1.0);
        modelAnim->setLoop(loop);

        if (frameCount != NULL) {
            *frameCount = modelAnim->getFrameCount();
        }
    }
}
