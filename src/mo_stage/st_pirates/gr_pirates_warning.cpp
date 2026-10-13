#include <gr/gr_calc_world_callback.h>
#include <snd/snd_system.h>
#include <st_pirates/gr_pirates_util.h>
#include <st_pirates/gr_pirates_warning.h>
#include <st_pirates/st_pirates.h>

grPiratesWarning* grPiratesWarning::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesWarning* ground = new (Heaps::StageInstance) grPiratesWarning(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesWarning::grPiratesWarning(const char* taskName) : grPirates(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    unk160.m_x = -60.0f;
    unk160.m_y = 35.0f;
    unk160.m_z = 0.0f;
    m_stateWork = NULL;
    unk170 = 0;
    unk174 = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grPiratesWarning::~grPiratesWarning() { }

void grPiratesWarning::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The warning of the tornado: it is shown for the time of the stage data, then the stage is told it is over (state 2).
void grPiratesWarning::updateActive(float deltaFrame) {
    stPiratesData* data = static_cast<stPiratesData*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setMotion(1, true, false, NULL);
            setVisibility(0);
            m_state = 1;
            break;
        case 1:
            if (*m_stateWork == 0) {
                setMotion(0, true, false, &unk174);
                setVisibility(1);
                g_sndSystem->playSE(snd_se_stage_Pirates_warning, 0, 0, 0, -1);
                m_timer = data->unk84;
                m_state = 4;
            }
            break;
        case 4:
            if (m_timer == 0.0f) {
                *m_stateWork = 2;
                m_state = 0;
            }
            break;
        }
    }
}

// The model sits where the warning is shown.
void grPiratesWarning::updateCallBack(float deltaFrame) {
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
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = unk160;
        }
    }
}

void grPiratesWarning::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION(unk170, 1, animId, loop, force, frameCount)
}
