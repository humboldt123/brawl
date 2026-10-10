#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_norfair/gr_norfair.h>
#include <st_norfair/gr_norfair_anim.h>

grNorfairWave::grNorfairWave(const char* taskName) : grNorfair(taskName), m_sePlayer() {
    m_posWork = NULL;
    m_unk160 = 1;
    m_wait = 0.0f;
    m_unk1 = -1;
    m_nodeIndex = -1;
    m_isUpdate = false;
    m_sePhase = 0;
    m_seHandle = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grNorfairWave* grNorfairWave::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairWave* ground = new (Heaps::StageInstance) grNorfairWave(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grNorfairWave::~grNorfairWave() {
}

void grNorfairWave::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The wave of lava: it waits for the event 4 of the stage, then rises with a sound and an effect and falls again.
void grNorfairWave::updateMotion(float deltaFrame) {
    m_wait = m_wait - deltaFrame;
    if (m_wait < 0.0f) {
        m_wait = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(0, false, true, NULL);
        setVisibility(0);
        m_sePhase = 0;
        m_seHandle = -1;
        m_state = 1;
        break;
    case 1:
        if (*m_eventIDWork == 4) {
            setMotion(0, false, true, &m_wait);
            g_sndSystem->playSE(static_cast<SndID>(0x1BC8), 0, 0, 0, -1);
            g_ecMgr->setDrawPrio(1);
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x3C0002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "magma_wave1", 0);
            g_ecMgr->setEffect(static_cast<EfID>(0x3C0003));
            g_ecMgr->setDrawPrio(-1);
            m_state = 11;
        }
        break;
    case 11:
        setVisibility(1);
        switch (m_sePhase) {
        case 0:
            if (getMotionFrame(0) > 180.0f) {
                m_seHandle = g_sndSystem->playSE(static_cast<SndID>(0x1BC9), 0, 0, 0, -1);
                m_sePhase++;
            }
            break;
        case 1:
            if (getMotionFrame(0) > 460.0f) {
                if (m_seHandle != -1) {
                    g_sndSystem->stopSE(m_seHandle, 0x5A);
                }
                m_seHandle = -1;
                m_sePhase++;
            }
            break;
        }
        if (m_wait == 0.0f) {
            *m_eventIDWork = 0;
            m_state = 0;
        }
        break;
    }
}

void grNorfairWave::updateCallBack(float deltaFrame) {
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
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                Vec3f* src = posWork;
                data->m_pos.m_x = src->m_x;
                data->m_pos.m_y = src->m_y;
                data->m_pos.m_z = src->m_z;
            }
        }
    }
}

// The animations of the wave are bound with all their parts, the character animation first (only the animation 0 exists).
void grNorfairWave::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_unk160 == animId && force == 0) {
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
    m_unk160 = animId;

    if (animId >= 1) {
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
