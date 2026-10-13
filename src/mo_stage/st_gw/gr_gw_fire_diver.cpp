#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <types.h>

#include <st_gw/gr_gw_item.h>
#include <st_gw/gr_gw_fire_diver.h>

grGWFireDiver::grGWFireDiver(const char* taskName) : grGW(taskName) {
    m_posWork = NULL;
    m_typeWork = NULL;
    m_eventWork = NULL;
    m_type = 0x10;
    m_nodeHand[0] = 0;
    m_nodeHand[1] = 0;
    m_nodeHand[2] = 0;
    m_handIndex = 0;
    m_handDir = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grGWFireDiver* grGWFireDiver::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWFireDiver* ground = new (Heaps::StageInstance) grGWFireDiver(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWFireDiver::~grGWFireDiver() {
}

void grGWFireDiver::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase(deltaFrame);
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The diver of the kind that the stage names is shown (the tall ones, 12 and 15, wave their three hands one after the
// other, going up and down).
void grGWFireDiver::updateActive(float deltaFrame) {
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
                switch (m_type) {
                case 12:
                    m_nodeHand[0] = getNodeIndex(0, "Diver_Left_Hand_02");
                    m_nodeHand[1] = getNodeIndex(0, "Diver_Left_Hand_01");
                    m_nodeHand[2] = getNodeIndex(0, "Diver_Left_Hand_03");
                    break;
                case 15:
                    m_nodeHand[0] = getNodeIndex(0, "Diver_Right_Hand_02");
                    m_nodeHand[1] = getNodeIndex(0, "Diver_Right_Hand_01");
                    m_nodeHand[2] = getNodeIndex(0, "Diver_Right_Hand_03");
                    break;
                }
                m_state = 2;
            }
            break;
        case 2:
            if (*m_sceneIdWork != m_sceneId) {
                m_state = 0;
            } else {
                if (m_type == *m_typeWork) {
                    setMotion(1, false, false, NULL);
                    setVisibility(1);
                    switch (m_type) {
                    case 12:
                    case 15:
                        setNodeVisibility(true, 0, m_nodeHand[0], true, false);
                        setNodeVisibility(false, 0, m_nodeHand[1], true, false);
                        setNodeVisibility(false, 0, m_nodeHand[2], true, false);
                        m_handIndex = 0;
                        m_handDir = 0;
                        m_timer = 10.0f;
                        break;
                    }
                    m_state = 3;
                }
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
                if (m_type == *m_typeWork) {
                    if (m_timer == 0.0f && (m_type == 15 || m_type == 12)) {
                        setNodeVisibility(false, 0, m_nodeHand[m_handIndex], true, false);
                        if (m_handIndex == 0) {
                            m_handDir = 0;
                        }
                        if (m_handIndex == 2) {
                            m_handDir = 1;
                        }
                        if (m_handDir == 1) {
                            m_handIndex--;
                        } else if (m_handDir == 0) {
                            m_handIndex++;
                        }
                        setNodeVisibility(true, 0, m_nodeHand[m_handIndex], true, false);
                        m_timer = 10.0f;
                    }
                } else {
                    setVisibility(0);
                    m_state = 2;
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

void grGWFireDiver::updateCallBack(float deltaFrame) {
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
                data->m_pos.m_x = m_posWork[2].m_x;
                data->m_pos.m_y = m_posWork[2].m_y;
                data->m_pos.m_z = m_posWork[2].m_z;
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
