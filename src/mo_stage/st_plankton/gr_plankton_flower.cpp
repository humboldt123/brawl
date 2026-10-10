#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <snd/snd_id.h>
#include <snd/snd_system.h>

#include <st_plankton/gr_plankton.h>

grPlanktonFlower* grPlanktonFlower::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPlanktonFlower* ground = new (Heaps::StageInstance) grPlanktonFlower(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPlanktonFlower::grPlanktonFlower(const char* taskName) : grPlankton(taskName), m_subject(0, 1) {
    m_soundState = 0;
    m_soundTimer = 0.0f;
    m_effectTimer = 0.0f;
    m_scale.m_x = 1.0f;
    m_scale.m_y = 1.0f;
    m_scale.m_z = 1.0f;
    m_leaf = NULL;
    m_subject.clear();
    m_subject.m_state = 1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 4;
    }
}

grPlanktonFlower::~grPlanktonFlower() {
}

void grPlanktonFlower::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateScale(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The flower is closed (state 0 / 1) until its leaves have been hit often enough, opens (4) for a while and closes (6) again
// when the time (the parameter of the stage) is over. While it is open the camera looks at it, and the sound of the stage
// fades in and out with it.
void grPlanktonFlower::updateScale(float deltaFrame) {
    if (m_leaf == NULL) {
        return;
    }
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    m_soundTimer = m_soundTimer - deltaFrame;
    if (m_soundTimer < 0.0f) {
        m_soundTimer = 0.0f;
    }
    m_effectTimer = m_effectTimer - deltaFrame;
    if (m_effectTimer < 0.0f) {
        m_effectTimer = 0.0f;
    }
    u8 state = m_state;
    if (state == 4) {
        goto open;
    }
    if (state < 4) {
        if (state != 1) {
            if (state != 0) {
                goto sound;
            }
            setVisibility(0);
            m_scale.m_x = 0.001f;
            m_scale.m_y = 0.001f;
            m_scale.m_z = 0.001f;
            m_subject.m_state = 1;
            m_state = 1;
        }
        if (isOpen() != 1) {
            goto sound;
        }
        m_subject.m_state = 0;
        m_subject.m_stateB = 0;
        {
            Vec3f pos;
            getNodePosition(&pos, 0, m_nodeIndex);
            Rect2D range;
            range.m_up = 25.0f;
            range.m_down = -25.0f;
            range.m_left = -25.0f;
            range.m_right = 25.0f;
            m_subject.setPos(&pos);
            m_subject.m_60 = range;
        }
        m_state = 4;
        goto open;
    }
    if (state != 6) {
        goto sound;
    }
    if (isOpen() == 1) {
        m_state = 4;
    } else {
        float rate = m_timer / data->unk28;
        rate = nw4r::math::FSelect(rate - 0.001f, rate, 0.001f);
        rate = nw4r::math::FSelect(rate - 1.0f, 1.0f, rate);
        m_scale.m_x = rate;
        m_scale.m_y = rate;
        m_scale.m_z = rate;
        if (rate == 0.001f) {
            m_state = 0;
        }
        if (rate <= 0.5f && m_soundState == 1) {
            float half = data->unk28;
            m_soundState = 6;
            m_soundTimer = half * 0.5f;
        }
    }
    goto sound;
open:
    setVisibility(1);
    m_scale.m_x = 1.0f;
    m_scale.m_y = 1.0f;
    m_scale.m_z = 1.0f;
    if (m_effectTimer == 0.0f) {
        playSEOpen();
        m_effectTimer = 75.0f;
    }
    {
        u8 sound = m_soundState;
        m_timer = data->unk28;
        if (sound != 4) {
            if (sound < 4) {
                if (sound != 1 && sound == 0) {
                    m_soundTimer = 15.0f;
                }
            } else if (sound == 6) {
                m_soundTimer = 0.0f;
            }
        }
    }
    m_soundState = 4;
    m_state = 6;
sound:
    u8 sound = m_soundState;
    if (sound == 4) {
        float rate = 1.0f - m_soundTimer / 15.0f;
        rate = nw4r::math::FSelect(rate - 0.0f, rate, 0.0f);
        rate = nw4r::math::FSelect(rate - 1.0f, 1.0f, rate);
        g_sndSystem->setEffectVol(0, 0, rate);
        if (rate == 1.0f) {
            m_soundState = 1;
        }
    } else if (sound > 3 && sound == 6) {
        float rate = m_soundTimer / (data->unk28 * 0.5f);
        rate = nw4r::math::FSelect(rate - 0.0f, rate, 0.0f);
        rate = nw4r::math::FSelect(rate - 1.0f, 1.0f, rate);
        g_sndSystem->setEffectVol(0, 0, rate);
        if (rate == 0.0f) {
            m_soundState = 0;
        }
    }
}

void grPlanktonFlower::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            data->m_scale.m_x = m_scale.m_x;
            data->m_scale.m_y = m_scale.m_y;
            data->m_scale.m_z = m_scale.m_z;
        }
    }
}

void grPlanktonFlower::playSEOpen() {
    Vec3f pos;
    getNodePosition(&pos, 0, m_nodeIndex);
    m_snd.setPos(&pos);
    m_snd.playSE(static_cast<SndID>(0x1D2D), 0, 0, -1);
}

// The flower is open while its six leaves have been hit (it is not whole while it still grows).
bool grPlanktonFlower::isOpen() {
    if (m_scale.m_x == 1.0f && m_scale.m_y == 1.0f && m_scale.m_z == 1.0f) {
        return false;
    }
    for (u8 i = 0; i < 6; i++) {
        if (m_leaf[i].m_hitCount != 14) {
            return false;
        }
    }
    return true;
}
