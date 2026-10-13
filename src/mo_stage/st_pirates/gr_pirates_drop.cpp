#include <gr/gr_calc_world_callback.h>
#include <mt/mt_prng.h>
#include <st_pirates/gr_pirates_drop.h>
#include <st_pirates/gr_pirates_util.h>
#include <st_pirates/st_pirates.h>

inline grPiratesDrop::grPiratesDrop(const char* taskName) : grPirates(taskName) {
    m_mtxWork = NULL;
    unk164.m_x = 0.0f;
    unk164.m_y = 0.0f;
    unk164.m_z = 0.0f;
    m_posLimitWork = NULL;
    m_stateWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grPiratesDrop* grPiratesDrop::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesDrop* ground = new (Heaps::StageInstance) grPiratesDrop(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesDrop::~grPiratesDrop() { }

void grPiratesDrop::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The drop starts left of the screen, behind the ship, and flies to the right over the deck; the stage waits for it (the state
// is set to 7 when it is gone).
void grPiratesDrop::updateActive(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            setVisibility(0);
            m_state = 1;
            break;
        case 1:
            if (*m_stateWork == 0) {
                Matrix* mtx = m_mtxWork;
                float posZ = mtx->m[2][3];
                float posY = mtx->m[1][3];
                float posX = mtx->m[0][3];
                unk164.m_x = posX;
                unk164.m_y = posY;
                unk164.m_z = posZ;
                unk164.m_x = *m_posLimitWork - 500.0f;
                unk164.m_z = randf() * -500.0f + -100.0f;
                setVisibility(1);
                m_state = 4;
            }
            break;
        case 4: {
            float x = unk164.m_x + *data * deltaFrame;
            Matrix* mtx = m_mtxWork;
            float mtxZ = mtx->m[2][3];
            float mtxY = mtx->m[1][3];
            float mtxX = mtx->m[0][3];
            Vec3f mtxPos(mtxX, mtxY, mtxZ);
            unk164.m_y = mtxPos.m_y;
            unk164.m_x = x;
            if (x > m_posLimitWork[3] + 500.0f) {
                *m_stateWork = 7;
                m_state = 0;
            }
            break;
        }
        }
    }
}

// The model sits where the drop is.
void grPiratesDrop::updateCallBack(float deltaFrame) {
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
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = unk164;
        }
    }
}
