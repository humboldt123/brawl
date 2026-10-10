#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_metalgear/gr_metalgear.h>

grMetalgear::grMetalgear(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grMetalgear* grMetalgear::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgear* ground = new (Heaps::StageInstance) grMetalgear(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgear::~grMetalgear() {
}

grMetalgearExclamation::grMetalgearExclamation(const char* taskName) : grMetalgear(taskName) {
    m_posWork = NULL;
    m_stateWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grMetalgearExclamation* grMetalgearExclamation::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgearExclamation* ground = new (Heaps::StageInstance) grMetalgearExclamation(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgearExclamation::~grMetalgearExclamation() {
}

void grMetalgearExclamation::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The mark is shown while the search light has a fighter (the state of the stage is 4).
void grMetalgearExclamation::updateActive(float deltaFrame) {
    switch (m_state) {
    case 0:
        setVisibility(0);
        m_state = 1;
    case 1:
        if (*m_stateWork == 4) {
            setMotionFrame(0.0f, 0);
            m_state = 6;
        }
        break;
    case 6:
        if (!m_isVisible) {
            setVisibility(1);
        }
        if (*m_stateWork == 4) {
            return;
        }
        m_state = 0;
        break;
    }
}

void grMetalgearExclamation::updateCallBack(float deltaFrame) {
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
            Vec3f* pos = m_posWork;
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = pos->m_x;
            data->m_pos.m_y = pos->m_y;
            data->m_pos.m_z = pos->m_z;
        }
    }
}
