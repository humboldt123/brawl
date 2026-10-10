#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_ice/gr_ice.h>

grIceKumo* grIceKumo::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceKumo* ground = new (Heaps::StageInstance) grIceKumo(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceKumo::~grIceKumo() {
}

void grIceKumo::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The cloud floats in from the side the stage says (the side of the first part of its model), stays on the screen and
// floats out on the other side.
void grIceKumo::updateMove(float deltaFrame) {
    if (m_mtxWork == NULL) {
        return;
    }
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data == NULL) {
        return;
    }
    float timer = m_timer;
    m_timer = timer - deltaFrame;
    if (timer - deltaFrame < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 1:
        if (*m_stateWork == 5) {
            Matrix* mtx = m_mtxWork;
            if (mtx[1].m[0][3] <= mtx[0].m[0][3]) {
                float x = m_limitWork[1].m_x;
                m_posY = (mtx[0].m[1][3] + mtx[1].m[1][3]) * 0.5f;
                m_posX = x + 20.0f;
                m_posZ = 0.0f;
            } else {
                float x = m_limitWork[0].m_x;
                m_posY = (mtx[0].m[1][3] + mtx[1].m[1][3]) * 0.5f;
                m_posX = x - 20.0f;
                m_posZ = 0.0f;
            }
            m_state = 0xB;
        }
        break;
    case 0:
        setVisibility(0);
        setEnableCollisionStatus(0);
        m_state = 1;
        break;
    case 0xB: {
        if (!m_isVisible) {
            setVisibility(1);
            setEnableCollisionStatus(1);
        }
        Matrix* mtx = m_mtxWork;
        if (mtx[1].m[0][3] <= mtx[0].m[0][3]) {
            float x = m_posX - data->unk7C * deltaFrame;
            m_posY = (mtx[0].m[1][3] + mtx[1].m[1][3]) * 0.5f;
            m_posX = x;
            if (x < m_limitWork[0].m_x - 20.0f) {
                *m_stateWork = 0xE;
                m_state = 0;
            }
        } else {
            float x = m_posX + data->unk7C * deltaFrame;
            m_posY = (mtx[0].m[1][3] + mtx[1].m[1][3]) * 0.5f;
            m_posX = x;
            if (m_limitWork[1].m_x + 20.0f < x) {
                *m_stateWork = 0xE;
                m_state = 0;
            }
        }
        break;
    }
    }
}

void grIceKumo::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = m_posX;
            data->m_pos.m_y = m_posY;
            data->m_pos.m_z = m_posZ;
        }
    }
}
