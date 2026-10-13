#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <types.h>

#include <st_gw/gr_gw_item.h>
#include <st_gw/gr_gw_fire_manhore.h>

inline grGWFireManhore::grGWFireManhore(const char* taskName) : grGW(taskName), m_snd() {
    m_stepWork = NULL;
    m_ctrlStepWork = NULL;
    m_typeWork = NULL;
    m_eventWork = NULL;
    m_type = 0x10;
    m_stepLast = 0xFF;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grGWFireManhore* grGWFireManhore::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWFireManhore* ground = new (Heaps::StageInstance) grGWFireManhore(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWFireManhore::~grGWFireManhore() {
}

void grGWFireManhore::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase(deltaFrame);
        updateVisible(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The fire is shown while the stage says that this kind of fire burns (and the event is on); it plays its end animation when
// the scene is over.
void grGWFireManhore::updateVisible(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
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
            if (*m_sceneIdWork == m_sceneId) {
                m_state = 3;
            }
            break;
        case 3:
            if (*m_sceneIdWork == m_sceneId) {
                bool burns;
                if (*m_eventWork == 1) {
                    if (m_type == *m_typeWork) {
                        burns = true;
                    } else {
                        burns = false;
                    }
                } else {
                    burns = false;
                }
                if (!m_isVisible && !burns) {
                } else if (m_isVisible && !burns) {
                    setVisibility(0);
                } else if (!m_isVisible && burns) {
                    setVisibility(1);
                }
                if (burns && *m_stepWork != m_stepLast) {
                    m_snd.playSE(static_cast<SndID>(0x1C90), 0, 0, -1);
                    m_snd.setPos(&m_posWork[*m_stepWork]);
                }
                m_stepLast = *m_stepWork;
            } else if (m_type == *m_typeWork) {
                setMotion(0, false, false, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                m_state = 4;
            } else {
                *m_stepWork = 0xFF;
                m_state = 0;
            }
            break;
        case 4:
            if (getMotionFrame(0) >= m_frameCount) {
                *m_stepWork = 0xFF;
                m_state = 0;
            }
            break;
        }
    }
}

void grGWFireManhore::updateCallBack(float deltaFrame) {
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
            if (m_posWork != NULL && *m_stepWork != 0xFF) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                Vec3f* pos = &m_posWork[*m_stepWork];
                data->m_pos.m_x = pos->m_x;
                data->m_pos.m_y = pos->m_y;
                data->m_pos.m_z = pos->m_z;
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
