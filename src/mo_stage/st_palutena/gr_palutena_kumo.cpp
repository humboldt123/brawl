#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>
#include <snd/snd_system.h>

#include <st_palutena/gr_palutena.h>

grPalutenaAshibaKumo* grPalutenaAshibaKumo::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaAshibaKumo* ground = new (Heaps::StageInstance) grPalutenaAshibaKumo(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaAshibaKumo::grPalutenaAshibaKumo(const char* taskName) : grPalutena(taskName) {
    m_posGimmick = NULL;
}

grPalutenaAshibaKumo::~grPalutenaAshibaKumo() {
}

grPalutenaAshibaKumoC* grPalutenaAshibaKumoC::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaAshibaKumoC* ground = new (Heaps::StageInstance) grPalutenaAshibaKumoC(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaAshibaKumoC::grPalutenaAshibaKumoC(const char* taskName) : grPalutenaAshibaKumo(taskName) {
}

grPalutenaAshibaKumoC::~grPalutenaAshibaKumoC() {
}

// The position of the node of the cloud is published for the chain.
void grPalutenaAshibaKumoC::processAnim() {
    Ground::processAnim();
    if (m_posGimmick != NULL) {
        getNodePosition(m_posGimmick, 0, m_nodeIndex);
    }
}

grPalutenaAshibaKumoD* grPalutenaAshibaKumoD::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaAshibaKumoD* ground = new (Heaps::StageInstance) grPalutenaAshibaKumoD(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaAshibaKumoD::grPalutenaAshibaKumoD(const char* taskName) : grPalutenaAshibaKumo(taskName) {
    m_phase = 0;
    m_landed = 0;
    m_landState = 0;
    m_time = 0.0f;
    m_time2 = 0.0f;
    m_landTime = 0.0f;
    m_landed = 0;
    m_fromLeft = 0;
    unk16E = 0;
    m_flyOut = 0;
    m_unk170 = 1;
    unk174 = 44.0f;
    unk178 = 5.0f;
    unk17C = 56.0f;
    m_flyTime = 0.0f;
    m_flyRate = 1.0f;
    unk188 = 0.0f;
    m_scale = 1.0f;
    m_scaleMul = 0.97f;
    m_speed = 0.0f;
    m_angle = 0.0f;
    m_translate.m_x = 0.0f;
    m_translate.m_y = 0.0f;
    m_translate.m_z = 0.0f;
    m_landing.m_x = 0.0f;
    m_landing.m_y = 0.0f;
    m_landing.m_z = 0.0f;
    unk1BC.m_x = 0.0f;
    unk1BC.m_y = 0.0f;
    unk1BC.m_z = 0.0f;
    m_node = 0;
    m_landTimer = 0.0f;
    m_ashibaLevel = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    }
}

grPalutenaAshibaKumoD::~grPalutenaAshibaKumoD() {
}

bool grPalutenaAshibaKumoD::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node, 0, "AshibaDKumo_kusari");
    return result;
}

void grPalutenaAshibaKumoD::processAnim() {
    Ground::processAnim();
    if (m_posGimmick != NULL) {
        getNodePosition(m_posGimmick + 1, 0, m_node);
    }
}

void grPalutenaAshibaKumoD::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateLanding(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The cloud flies in at an angle (state 1 picks the way), slows down (2 / 3), turns to follow the line to the chain's end
// (4) and shrinks away (5) when it leaves the picture.
void grPalutenaAshibaKumoD::updateMove(float deltaFrame) {
    stPalutenaData* data = static_cast<stPalutenaData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_time = m_time + deltaFrame;
    m_time2 = m_time2 + deltaFrame;
    u8 phase = m_phase;
    if (phase == 3) {
        Vec3f pos = m_translate;
        float rate = m_time2 / (m_flyTime * (1.0f - m_flyRate) + unk188 * m_flyTime * m_flyRate);
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (1.0f < rate) {
            rate = 1.0f;
        }
        float eased = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))));
        float distance = m_scale * (1.0f - eased) * (m_speed * deltaFrame);
        float sine;
        float cosine;
        mtSinCosf(m_angle, &sine, &cosine);
        pos.m_x = pos.m_x + cosine * distance;
        pos.m_y = pos.m_y + sine * distance;
        m_translate = pos;
        if (m_flyTime <= m_time) {
            m_phase = 1;
        }
    } else if (phase < 3) {
        if (phase == 1) {
            m_flyTime = data->unk30 + (data->unk34 - data->unk30) * randf();
            m_flyRate = data->unk38 + (data->unk3C - data->unk38) * randf();
            float angle;
            if (m_fromLeft == 1) {
                angle = data->unk48 + data->unk4C * 0.25f + data->unk4C * randf();
            } else {
                angle = (data->unk48 - data->unk4C) + (data->unk4C * 2.0f) * randf();
            }
            m_fromLeft = 0;
            m_scale = 1.0f;
            m_angle = angle * 0.017453292f;
            unk16E = 0;
            m_flyOut = 0;
            m_unk170 = 0;
            float speedRate = randf();
            m_landed = 0;
            m_time = 0.0f;
            m_time2 = 0.0f;
            m_phase = 2;
            m_speed = data->unk40 + (data->unk44 - data->unk40) * speedRate;
        } else if (phase == 0) {
            if (m_flyTime <= m_time) {
                m_phase = 1;
            }
        } else {
            Vec3f pos = m_translate;
            float rate = m_time2 / (m_flyTime * m_flyRate);
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            if (1.0f < rate) {
                rate = 1.0f;
            }
            float eased = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 32768.0f))));
            float distance = eased * (m_speed * deltaFrame);
            if (m_time2 < m_flyTime * m_flyRate * 0.75f) {
                float sine;
                float cosine;
                mtSinCosf(m_angle, &sine, &cosine);
                pos.m_x = pos.m_x + cosine * distance;
                pos.m_y = pos.m_y + sine * distance;
                m_translate = pos;
                if (m_flyTime <= m_time) {
                    m_phase = 1;
                }
            } else {
                m_time2 = 0.0f;
                m_speed = distance;
                m_phase = 3;
                unk188 = 0.25f;
                m_landed = 1;
            }
        }
    } else if (phase == 5) {
        if (m_landed == 1) {
            m_phase = 3;
        } else {
            float rate = m_time2 / (m_flyTime * m_flyRate);
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            if (1.0f < rate) {
                rate = 1.0f;
            }
            float eased = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 32768.0f))));
            m_phase = 3;
            m_time2 = 0.0f;
            unk188 = 1.0f - eased;
            m_scale = m_scale * eased;
            m_landed = 1;
        }
    } else if (phase < 5) {
        // the cloud turns towards the line that runs from one end of the chain to the other
        Vec3f* ends = m_posGimmick;
        Vec3f line(ends[0].m_x - ends[1].m_x, ends[0].m_y - ends[1].m_y, ends[0].m_z - ends[1].m_z);
        float degree = nw4r::math::Atan2FIdx(line.m_y, line.m_x) * 0.02454369f * 57.29578f;
        if (degree < 0.0f) {
            degree = degree + 360.0f;
        }
        float current = m_angle * 57.29578f;
        if (180.0f < current) {
            current = current - 360.0f;
        }
        float target;
        if (210.0f <= degree) {
            if (240.0f <= degree) {
                degree = degree + current * 0.125f * -1.0f;
                if (degree - 240.0f < 0.0f) {
                    degree = 240.0f;
                }
                target = 270.0f;
                if (degree - 270.0f < 0.0f) {
                    target = degree;
                }
            } else {
                degree = degree + current * 0.35f * -1.0f;
                if (degree - 210.0f < 0.0f) {
                    degree = 210.0f;
                }
                target = 240.0f;
                if (degree - 240.0f < 0.0f) {
                    target = degree;
                }
            }
        } else {
            degree = degree + current * 0.125f * -1.0f;
            if (degree - 180.0f < 0.0f) {
                degree = 180.0f;
            }
            target = 210.0f;
            if (degree - 210.0f < 0.0f) {
                target = degree;
            }
        }
        m_angle = target * 0.017453292f;
        if (m_landed == 1) {
            m_phase = 3;
            m_speed = m_speed * data->unk50;
        } else {
            float rate = m_time2 / (m_flyTime * m_flyRate);
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            if (1.0f < rate) {
                rate = 1.0f;
            }
            float eased = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 32768.0f))));
            m_time2 = 0.0f;
            unk188 = 1.0f - eased;
            m_phase = 3;
            m_landed = 1;
            m_speed = m_speed * eased * data->unk50;
        }
        Vec3f soundPos(m_translate.m_x + m_landing.m_x, m_translate.m_y + m_landing.m_y, m_translate.m_z + m_landing.m_z);
        m_snd.setPos(&soundPos);
        m_snd.playSE(static_cast<SndID>(0x1CE0), 0, 0, -1);
    }
    if (m_unk170 != 1) {
        Vec3f* ends = m_posGimmick;
        Vec3f line(ends[1].m_x - ends[0].m_x, ends[1].m_y - ends[0].m_y, ends[1].m_z - ends[0].m_z);
        float length = 0.0f;
        float lengthSq = line.m_z * line.m_z + (line.m_x * line.m_x + line.m_y * line.m_y);
        if (!((float)fabs(lengthSq) <= 1.17549435e-38f)) {
            length = lengthSq * rsqrtf(lengthSq);
        }
        if (unk174 <= length) {
            m_phase = 4;
            m_unk170 = 1;
        }
    }
    if (m_flyOut == 1) {
        m_scale = m_scale * m_scaleMul;
        if (m_scale < 0.0f) {
            m_scale = 0.0f;
        }
    } else {
        if (m_angle < 0.0f && -3.1415927f < m_angle && m_posGimmick[1].m_y < unk178) {
            m_fromLeft = 1;
            m_flyOut = 1;
        }
        if ((1.5707964f < m_angle || m_angle < -1.5707964f) && m_posGimmick[1].m_x < unk17C) {
            unk16E = 1;
            m_flyOut = 1;
        }
        if (m_flyOut == 1) {
            m_phase = 5;
        }
    }
}

void grPalutenaAshibaKumoD::updateLanding(float deltaFrame) {
    m_landTimer = m_landTimer - deltaFrame;
    if (m_landTimer < 0.0f) {
        m_landTimer = 0.0f;
    }
    switch (m_landState) {
    case 1: {
        m_landTime = m_landTime + deltaFrame;
        float rate = m_landTime * 0.125f;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (1.0f < rate) {
            rate = 1.0f;
        }
        if (rate == 1.0f) {
            m_landState = 2;
            m_landTime = 0.0f;
        }
        m_landing.m_y = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f)))) * -3.0f;
        break;
    }
    case 2: {
        m_landTime = m_landTime + deltaFrame;
        float rate = m_landTime / 12.0f;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (1.0f < rate) {
            rate = 1.0f;
        }
        if (rate == 1.0f) {
            m_landState = 0;
            m_landTime = 0.0f;
        }
        m_landing.m_y = -0.5f - (1.0f - nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))))) * 2.5f;
        break;
    }
    }
    if (m_landTimer == 0.0f) {
        m_landing.m_y = m_landing.m_y + deltaFrame * 0.025f;
        if (0.0f < m_landing.m_y) {
            m_landing.m_y = 0.0f;
        }
    }
}

void grPalutenaAshibaKumoD::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = m_nodeIndex;
            }
            grNodeCallbackData* data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            data->m_offsetPos.m_x = m_translate.m_x + m_landing.m_x;
            data->m_offsetPos.m_y = m_translate.m_y + m_landing.m_y;
            data->m_offsetPos.m_z = m_translate.m_z + m_landing.m_z;
        }
    }
}

void grPalutenaAshibaKumoD::getTranslate_Delta(Vec3f* out) {
    if (out == NULL) {
        return;
    }
    out->m_x = m_translate.m_x;
    out->m_y = m_translate.m_y;
    out->m_z = m_translate.m_z;
}

// A fighter lands on the cloud (only while the platform it carries is broken).
void grPalutenaAshibaKumoD::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    if (m_ashibaLevel != 4) {
        return;
    }
    bool start = false;
    if (m_landTimer == 0.0f) {
        start = true;
    }
    if (isFirstContact == 1) {
        start = true;
    }
    m_landTimer = 5.0f;
    if (start == 1) {
        m_landState = 1;
    }
}
