#include <gr/gr_calc_world_callback.h>
#include <mt/mt_trig.h>
#include <st_dxbigblue/gr_dxbigblue_bg.h>

inline grDxBigBlueBg::grDxBigBlueBg(const char* taskName) : grDxBigBlue(taskName) {
    m_posWork = NULL;
    m_posLimitWork = NULL;
    m_rotWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grDxBigBlueBg* grDxBigBlueBg::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grDxBigBlueBg* ground = new (Heaps::StageInstance) grDxBigBlueBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grDxBigBlueBg::~grDxBigBlueBg() { }

void grDxBigBlueBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateCallBack(deltaFrame);
}

// The background keeps to the position of the stage, and moves in a circle around it as the course turns (the angle in
// degrees; it goes from 0 to 180 and back).
void grDxBigBlueBg::updateCallBack(float deltaFrame) {
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
            if (m_posWork != NULL && m_rotWork != NULL) {
                float angle = m_rotWork[2];
                float rate;
                if (angle < 180.0f) {
                    rate = angle / 180.0f;
                    if (rate < 0.0f) {
                        rate = 0.0f;
                    }
                    if (rate > 1.0f) {
                        rate = 1.0f;
                    }
                } else {
                    rate = 1.0f - (angle - 180.0f) / 180.0f;
                    if (rate < 0.0f) {
                        rate = 0.0f;
                    }
                    if (rate > 1.0f) {
                        rate = 1.0f;
                    }
                }
                Vec3f pos;
                Vec3f rot;
                rot.m_x = m_rotWork[0];
                rot.m_y = m_rotWork[1];
                rot.m_z = -m_rotWork[2];
                float radius = rate * 3500.0f;
                float sine;
                float cosine;
                float theta = rot.m_z - 90.0f;
                mtSinCosf(theta * 0.017453292f, &sine, &cosine);
                pos.m_x = m_posWork[0] + radius * cosine;
                pos.m_y = m_posWork[1] + radius * sine;
                pos.m_z = m_posWork[2];
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = pos;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot = rot;
            }
        }
    }
}
