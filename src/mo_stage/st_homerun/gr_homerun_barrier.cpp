#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <memory.h>
#include <snd/snd_id.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_homerun/gr_homerun_anim.h>
#include <st_homerun/gr_homerun.h>

grHomerunBarrier* grHomerunBarrier::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHomerunBarrier* ground = new (Heaps::StageInstance) grHomerunBarrier(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHomerunBarrier::grHomerunBarrier(const char* taskName) : grHomerun(taskName) {
    m_stateWork = NULL;
    m_hpWork = NULL;
    m_barrierState = 0;
    m_nextState = 0;
    m_pos[0] = 0.0f;
    m_pos[1] = 0.0f;
    m_pos[2] = 0.0f;
    m_motion = 3;
    m_motionTimer = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grHomerunBarrier::~grHomerunBarrier() { }

void grHomerunBarrier::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateState(deltaFrame);
        updateScroll(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The barrier state the stage keeps (m_stateWork): 1 hit, 2 standing, 3 about to vanish, 4 broken, 5 gone. This plays
// the animation (0 hit, 1 vanish, 2 break; 3 is the idle pose), the sounds and the break effect.
void grHomerunBarrier::updateState(float deltaFrame) {
    if (getStageData() == NULL) {
        return;
    }
    switch (m_state) {
    case 0:
        setMotion(3, false, true, NULL);
        setVisibility(0);
        m_state = 1;
        break;
    case 1:
        switch (*m_stateWork) {
        case 1:
            setMotion(0, false, true, &m_motionTimer);
            setVisibility(1);
            g_sndSystem->playSE(snd_se_Homerun_barrier, 0, 0, 0, -1);
            *m_stateWork = 2;
            m_barrierState = *m_stateWork;
            m_nextState = 0;
            m_state = 3;
            break;
        case 3:
            setMotion(1, false, true, &m_motionTimer);
            setVisibility(1);
            g_sndSystem->playSE(snd_se_Homerun_barrier_off, 0, 0, 0, -1);
            m_barrierState = *m_stateWork;
            m_nextState = 5;
            m_state = 6;
            break;
        }
        break;
    case 3:
        switch (*m_stateWork) {
        case 1:
            setMotion(0, false, true, &m_motionTimer);
            g_sndSystem->playSE(snd_se_Homerun_barrier, 0, 0, 0, -1);
            *m_stateWork = 2;
            m_barrierState = *m_stateWork;
            m_nextState = 0;
            break;
        case 3:
            setMotion(1, false, true, &m_motionTimer);
            setVisibility(1);
            g_sndSystem->playSE(snd_se_Homerun_barrier_off, 0, 0, 0, -1);
            m_barrierState = *m_stateWork;
            m_nextState = 5;
            m_state = 6;
            break;
        }
        // FALL-THROUGH
    case 6:
    case 7:
        m_motionTimer -= deltaFrame;
        if (m_motionTimer < 0.0f) {
            m_motionTimer = 0.0f;
        }
        if (0.0f == m_motionTimer) {
            *m_stateWork = m_nextState;
            m_state = 0;
        }
        break;
    }
    if (*m_stateWork == 4) {
        setMotion(2, false, true, &m_motionTimer);
        setVisibility(1);
        g_sndSystem->playSE(snd_se_Homerun_barrier_crash, 0, 0, 0, -1);
        u32 effect = g_ecMgr->setEffect(ef_ptc_stg_homerun_crash);
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        g_ecMgr->setParent(effect, scnMdl, getTgtNode(), false);
        m_barrierState = *m_stateWork;
        m_nextState = 5;
        *m_stateWork = 5;
        m_state = 7;
    }
}

void grHomerunBarrier::updateScroll(float deltaFrame) { }

void grHomerunBarrier::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data[0].m_pos.m_x = m_pos[0];
            data[0].m_pos.m_y = m_pos[1];
            data[0].m_pos.m_z = m_pos[2];
        }
    }
}

// Binds the animation of the barrier (three of them: hit, vanish, break; the idle pose is 3).
void grHomerunBarrier::setMotion(u32 index, bool loop, bool force, float* frameCount) {
    if (m_motion == index && force == 0) {
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
    m_motion = index;

    if (index >= 3) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > index);
    if (result) {
        grHomerunSetVisibilityAnim2(index, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > index);
    if (result) {
        grHomerunSetChrAnim2(index, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > index);
    if (result) {
        grHomerunSetTexPatAnim2(index, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > index);
    if (result) {
        grHomerunSetTexSrtAnim2(index, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > index);
    if (result) {
        grHomerunSetColorAnim2(index, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
