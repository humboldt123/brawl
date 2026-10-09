#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <string.h>

#include <st_mariopast/gr_mariopast.h>

inline grMarioPastLift::grMarioPastLift(const char* taskName) : grMarioPast(taskName) {
    m_posWork = NULL;
    m_posLimit = NULL;
    m_ctrlId = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_id = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
}

grMarioPastLift* grMarioPastLift::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMarioPastLift* ground = new (Heaps::StageInstance) grMarioPastLift(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMarioPastLift::~grMarioPastLift() {
}

void grMarioPastLift::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// A lift waits (0) until it is its turn (the stage's id byte equals its id), then rides (2) from the start to the end position
// of the stage data and goes back to waiting when it leaves the camera area.
void grMarioPastLift::updateMove(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data == NULL) {
        return;
    }
    switch (m_state) {
    case 1:
        if (*m_ctrlId != m_id) {
            m_pos.m_x = m_posWork[0].m_x;
            m_pos.m_y = m_posWork[0].m_y;
            m_pos.m_z = m_posWork[0].m_z;
            if (m_posWork[0].m_y < m_posWork[1].m_y) {
                m_pos.m_y = m_posLimit[1].m_y;
                return;
            }
            m_pos.m_y = m_posLimit[0].m_y;
            return;
        }
        setVisibility(1);
        setEnableCollisionStatus(true);
        if (m_collision != NULL) {
            m_collision->setEnable();
        }
        m_state = 2;
        break;
    case 0:
        setVisibility(0);
        setEnableCollisionStatus(false);
        m_state = 1;
        return;
    case 2:
        break;
    default:
        return;
    }
    Vec3f dir;
    dir.m_x = m_posWork[1].m_x - m_posWork[0].m_x;
    dir.m_y = m_posWork[1].m_y - m_posWork[0].m_y;
    dir.m_z = m_posWork[1].m_z - m_posWork[0].m_z;
    dir.normalize();
    float y = m_pos.m_y + dir.m_y * (data[4] * deltaFrame);
    m_pos.m_x = m_posWork[0].m_x;
    m_pos.m_y = y;
    m_pos.m_z = m_posWork[0].m_z;
    if (m_posWork[1].m_y <= m_posWork[0].m_y) {
        if (y < m_posLimit[1].m_y - 10.0f) {
            m_state = 0;
        }
    } else if (m_posLimit[0].m_y + 10.0f < y) {
        m_state = 0;
    }
}

void grMarioPastLift::updateCallBack(float deltaFrame) {
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
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = m_pos.m_x;
                data->m_pos.m_y = m_pos.m_y;
                data->m_pos.m_z = m_pos.m_z;
            }
        }
    }
}
