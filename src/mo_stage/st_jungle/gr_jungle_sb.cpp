#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleSB::grJungleSB(const char* taskName) : grJungle(taskName) {
    m_posWork = NULL;
    m_stateWork = NULL;
    m_motion = 1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas->m_flags |= 1;
    }
}

grJungleSB* grJungleSB::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleSB* ground = new (Heaps::StageInstance) grJungleSB(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleSB::~grJungleSB() {
}

void grJungleSB::update(float deltaFrame) {
    grJungle::update(deltaFrame);
    if (m_isUpdate) {
        updateBreak(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grJungleSB::updateBreak(float deltaFrame) {
    if (m_stateWork != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, false, true, NULL);
            setVisibility(0);
            m_state = 1;
            break;
        case 1:
            if (*m_stateWork == 2) {
                requestBreak();
            }
            break;
        case 9:
            if (m_timer == 0.0f) {
                m_state = 0;
            }
            break;
        }
    }
}

void grJungleSB::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = *m_sceneModels;
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_posWork != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = m_posWork->m_x;
                data->m_pos.m_y = m_posWork->m_y;
                data->m_pos.m_z = m_posWork->m_z;
            }
        }
    }
}

void grJungleSB::requestBreak() {
    if (m_stateWork != NULL && m_state == 1) {
        setVisibility(1);
        setMotion(0, false, false, &m_timer);
        *m_stateWork = 3;
        u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x390001));
        g_ecMgr->setParent(effect, m_sceneModels[0], "StoneN", 0);
        m_state = 9;
    }
}

void grJungleSB::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion != animId || force != 0) {
        nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
        if (sceneMdl != NULL) {
            gfModelAnimation* modelAnim = *m_modelAnims;
            if (modelAnim != NULL) {
                nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
                if (model.IsValid()) {
                    modelAnim->unbindNodeAnim(sceneMdl);
                    modelAnim->unbindVisibleAnim(sceneMdl);
                    modelAnim->unbindTexAnim(sceneMdl);
                    modelAnim->unbindTexSrtAnim(sceneMdl);
                    modelAnim->unbindMatColAnim(sceneMdl);
                    m_motion = animId;
                    if (animId == 0) {
                        GR_JUNGLE_BIND_ANIMS(animId, model, modelAnim)
                        gfModelAnimation::bind(sceneMdl, modelAnim);
                        modelAnim->setFrame(0.0);
                        modelAnim->setUpdateRate(1.0);
                        modelAnim->setLoop(loop);
                        if (frameCount != NULL) {
                            *frameCount = modelAnim->getFrameCount();
                        }
                    }
                }
            }
        }
    }
}
