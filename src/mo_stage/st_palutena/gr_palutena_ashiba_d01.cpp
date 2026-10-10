#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>

#include <st_palutena/gr_palutena.h>

inline grPalutenaAshibaD01::grPalutenaAshibaD01(const char* taskName) : grPalutenaAshiba(taskName) {
    m_landing.m_x = 0.0f;
    m_landing.m_y = 0.0f;
    m_landing.m_z = 0.0f;
    m_translate.m_x = 0.0f;
    m_translate.m_y = 0.0f;
    m_translate.m_z = 0.0f;
    m_unk1 = -1;
    m_nodeIndex = -1;
    unk1D1 = 0;
    m_landState = 0;
    m_landTimer = 0.0f;
    m_landTime = 0.0f;
}

grPalutenaAshibaD01* grPalutenaAshibaD01::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaAshibaD01* ground = new (Heaps::StageInstance) grPalutenaAshibaD01(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaAshibaD01::~grPalutenaAshibaD01() {
}

void grPalutenaAshibaD01::update(float deltaFrame) {
    grPalutenaAshiba::update(deltaFrame);
    if (m_isUpdate) {
        updateLanding(deltaFrame);
    }
}

// The dip when a fighter lands on the platform: it is pushed down (sine, 8 frames) and springs back (12 frames).
void grPalutenaAshibaD01::updateLanding(float deltaFrame) {
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

// The model follows the platform (shake, landing dip and the translation of the cloud under it).
void grPalutenaAshibaD01::updateCallBack(float deltaFrame) {
    Vec3f pos;
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            calcWorldCallBack->m_index = 0;
            scnMdl->m_calcWorldCallBack = calcWorldCallBack;
            scnMdl->EnableScnMdlCallbackTiming(1);
            scnMdl->m_nodeIndex = m_nodeIndex;
            grNodeCallbackData* data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            Vec3f shaken = m_offset + m_shakePos;
            Vec3f landed = shaken + m_landing;
            Vec3f offset = landed + m_translate;
            data->m_offsetPos.m_x = offset.m_x;
            data->m_offsetPos.m_y = offset.m_y;
            data->m_offsetPos.m_z = offset.m_z;
            data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            data->m_scale.m_x = m_scale.m_x;
            data->m_scale.m_y = m_scale.m_y;
            data->m_scale.m_z = m_scale.m_z;
            getNodePosition(&pos, 0, m_nodeIndex);
            m_snd.setPos(&pos);
        }
    }
}

void grPalutenaAshibaD01::getTranslate_Delta(Vec3f* out) {
    if (out == NULL) {
        return;
    }
    out->m_x = m_translate.m_x;
    out->m_y = m_translate.m_y;
    out->m_z = m_translate.m_z;
}

void grPalutenaAshibaD01::setTranslate_Delta(float x, float y, float z) {
    m_translate.m_x = x;
    m_translate.m_y = y;
    m_translate.m_z = z;
}

void grPalutenaAshibaD01::getLanding_Delta(Vec3f* out) {
    if (out == NULL) {
        return;
    }
    out->m_x = m_landing.m_x;
    out->m_y = m_landing.m_y;
    out->m_z = m_landing.m_z;
}

// A fighter lands on the platform: it starts to dip when the platform is whole.
void grPalutenaAshibaD01::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    bool start = false;
    if (m_landTimer == 0.0f) {
        start = true;
    }
    if (isFirstContact == 1) {
        start = true;
    }
    m_landTimer = 5.0f;
    if (m_scale.m_y != 1.0f) {
        return;
    }
    if (start == 1) {
        m_landState = 1;
    }
}
