#include <ai/ai_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

inline grHalberdTarget::grHalberdTarget(const char* taskName) : grHalberd(taskName) {
    m_posWork = NULL;
    m_motion = 4;
    m_motionTimer = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    m_seId = -1;
    m_dangerZone = -1;
}

grHalberdTarget* grHalberdTarget::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdTarget* ground = new (Heaps::StageInstance) grHalberdTarget(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdTarget::~grHalberdTarget() {
}

void grHalberdTarget::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grHalberdTarget::updateMotion(float deltaFrame) {
    if (m_stateWork != NULL) {
        m_motionTimer -= deltaFrame;
        if (m_motionTimer < 0.0f) {
            m_motionTimer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(4, 0, 1, NULL);
            setVisibility(0);
            m_state = 2;
            // fall through
        case 2:
            if (*m_stateWork == 8) {
                setMotion(3, 0, 1, &m_motionTimer);
                setVisibility(1);
                m_seId = m_snd.playSE(snd_se_stage_Halberd_Target_01, 0, 0, -1);
                m_state = 6;
            }
            break;
        case 6: {
            Vec2f zoneMin(m_posWork->m_x - 20.0f, m_posWork->m_y + 20.0f);
            Vec2f zoneMax(m_posWork->m_x + 20.0f, m_posWork->m_y - 20.0f);
            m_dangerZone = g_aiMgr->setDangerZone(&zoneMin, &zoneMax, m_dangerZone, 0, 0);
            if (*m_stateWork == 9) {
                setMotion(1, 1, 1, &m_motionTimer);
                if (m_seId != -1) {
                    m_snd.stopSE(m_seId, 0);
                }
                m_seId = m_snd.playSE(snd_se_stage_Halberd_Target_02, 0, 0, -1);
                m_state = 7;
            }
            break;
        }
        case 7:
            if (*m_stateWork == 10) {
                setMotion(0, 0, 1, &m_motionTimer);
                if (m_seId != -1) {
                    m_snd.stopSE(m_seId, 0);
                }
                m_seId = -1;
                m_snd.playSE(snd_se_stage_Halberd_Target_close, 0, 0, -1);
                if (m_dangerZone != -1) {
                    g_aiMgr->delDangerZone(m_dangerZone);
                    m_dangerZone = -1;
                }
                m_state = 8;
            }
            break;
        }
        if (m_motionTimer == 0.0f) {
            u8 motion = m_motion;
            if (motion == 3) {
                setMotion(2, 1, 1, NULL);
            } else if (motion < 3 && motion == 0) {
                m_state = 0;
            }
        }
    }
}

void grHalberdTarget::updateCallBack(float deltaFrame) {
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
            if (m_posWork != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = *m_posWork;
            }
        }
    }
}

// The animation of the target (4 animations).
void grHalberdTarget::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    if (animId >= 4) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grHalberdSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grHalberdSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grHalberdSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grHalberdSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grHalberdSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
