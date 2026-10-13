#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <types.h>

#include <st_gw/gr_gw_item.h>
#include <st_gw/gr_gw_step.h>

inline grGWStep::grGWStep(const char* taskName) : grGW(taskName) {
    m_posWork = NULL;
    m_ctrlStepWork = NULL;
    m_eventWork = NULL;
    m_tblStep = NULL;
    m_id = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grGWStep* grGWStep::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWStep* ground = new (Heaps::StageInstance) grGWStep(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWStep::~grGWStep() {
}

void grGWStep::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase(deltaFrame);
        updateVisible(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The step plays its animation backwards when the scene ends; while the scene plays it is shown when the table of the
// scene says so for the control step (and the event is on).
void grGWStep::updateVisible(float deltaFrame) {
    if (m_tblStep != NULL) {
        float* data = static_cast<float*>(getStageData());
        if (data != NULL) {
            m_timer -= deltaFrame;
            if (m_timer < 0.0f) {
                m_timer = 0.0f;
            }
            switch (m_state) {
            case 0:
                setMotion(0, false, true, &m_frameCount);
                setMotionFrame(m_frameCount, 0);
                setVisibility(0);
                m_state = 1;
                break;
            case 1:
                if (*m_sceneIdWork == m_sceneId) {
                    setMotion(0, true, true, &m_frameCount);
                    setMotionFrame(m_frameCount, 0);
                    m_frameLast = m_frameCount;
                    if (m_motionRatio > 0.0f) {
                        m_motionRatio *= -1.0f;
                    }
                    m_state = 3;
                }
                break;
            case 3:
                if (*m_sceneIdWork != m_sceneId) {
                    setMotion(0, false, true, &m_frameCount);
                    if (m_motionRatio < 0.0f) {
                        m_motionRatio *= -1.0f;
                    }
                    m_state = 4;
                } else {
                    if (getMotionFrame(0) > m_frameLast) {
                        setMotion(1, false, false, NULL);
                    } else {
                        m_frameLast = getMotionFrame(0);
                    }
                    if (*m_eventWork == 1) {
                        u8* table = reinterpret_cast<u8*>(m_tblStep->getData(*m_ctrlStepWork));
                        if (table != NULL && table[m_id] == 1) {
                            setVisibility(1);
                        }
                    } else if (*m_eventWork == 0) {
                        setVisibility(0);
                    }
                }
                break;
            case 4:
                if (getMotionFrame(0) >= m_frameCount) {
                    m_state = 0;
                }
                break;
            }
        }
    }
}

void grGWStep::updateCallBack(float deltaFrame) {
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
            if (m_posWork != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = m_posWork->m_x;
                data->m_pos.m_y = m_posWork->m_y;
                data->m_pos.m_z = m_posWork->m_z;
            }
            Vec3f scale;
            gwVec3Scale(&scale, &m_scale, 0.9f);
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}
