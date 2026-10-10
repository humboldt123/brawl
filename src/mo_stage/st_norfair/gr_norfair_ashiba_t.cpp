#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <types.h>

#include <st_norfair/gr_norfair.h>

// HYPOTHESIS: the clamp helper that the other stages use as well.
static inline float norfairClamp(float lo, float hi, float value) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

grNorfairAshibaT::grNorfairAshibaT(const char* taskName) : grNorfair(taskName) {
    m_posWork = NULL;
    m_posMagmaWork = NULL;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    m_posYFrom = 0.0f;
    m_posYTo = 0.0f;
    m_moveTime = 0.0f;
    m_index = 0;
    m_first = 1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
}

grNorfairAshibaT* grNorfairAshibaT::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairAshibaT* ground = new (Heaps::StageInstance) grNorfairAshibaT(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grNorfairAshibaT::~grNorfairAshibaT() {
}

void grNorfairAshibaT::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The platforms of the trainer rise and sink between three heights. They go down when the wave comes (the event 4) and
// follow the lava otherwise: they move up when it is far under them and sink when it comes close.
void grNorfairAshibaT::updateActive(float deltaFrame) {
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0: {
        bool near = false;
        Vec3f* pos = &m_posWork[m_index];
        if (norfairIsNearZero(pos->m_x) && norfairIsNearZero(pos->m_y) && norfairIsNearZero(pos->m_z)) {
            near = true;
        }
        if (near == true) {
            return;
        }
        m_state = 1;
    }
    case 1:
        m_state = 7;
    case 7:
        if (m_timer != 0.0f) {
            float t = 1.0f - m_timer / m_moveTime;
            t = norfairClamp(0.0f, 1.0f, t);
            float sine = nw4r::math::SinIdx((u16)(16384.0f * t));
            m_offset.m_y = m_posYFrom + sine * (m_posYTo - m_posYFrom);
        } else {
            Vec3f nodePos;
            getNodePosition(&nodePos, 0, m_nodeIndex);
            if (*m_eventIDWork == 4) {
                if (m_index != 3) {
                    m_index = 3;
                    m_posYFrom = m_offset.m_y;
                    m_posYTo = m_posWork[m_index].m_y;
                    m_timer = 90.0f * randf() + 210.0f;
                    m_moveTime = m_timer;
                }
            } else {
                float magmaY = m_posMagmaWork->m_y;
                if (nodePos.m_y <= magmaY + 30.0f) {
                    if (m_first == 1) {
                        m_first = 0;
                    } else {
                        m_index = m_index + 1;
                        if (m_index > 2) {
                            m_index = 2;
                        }
                    }
                    m_posYFrom = m_offset.m_y;
                    m_posYTo = m_posWork[m_index].m_y;
                    m_timer = 30.0f * randf() + 60.0f;
                    m_moveTime = m_timer;
                } else {
                    if (m_index != 0 && m_offset.m_y >= magmaY + 50.0f) {
                        m_index = m_index - 1;
                        m_posYFrom = m_offset.m_y;
                        m_posYTo = m_posWork[m_index].m_y;
                        m_timer = 30.0f * randf() + 60.0f;
                        m_moveTime = m_timer;
                    }
                }
            }
        }
        break;
    }
}

void grNorfairAshibaT::updateCallBack(float deltaFrame) {
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
                data->m_offsetPos.m_x = m_offset.m_x;
                data->m_offsetPos.m_y = m_offset.m_y;
                data->m_offsetPos.m_z = m_offset.m_z;
            }
        }
    }
}
