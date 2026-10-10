#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_ice/gr_ice.h>

// Matrix::rotZ (main, unnamed)
extern "C" void fn_8003EBF4(Matrix* matrix, float angle);

grIceRot::grIceRot(const char* taskName) : grIce(taskName), m_snd() {
    m_mtxWork = NULL;
    m_unk15C = 0.0f;
    m_unk160 = 0.0f;
    m_angle = 0.0f;
    m_landTimer = 0.0f;
    m_speed = 0.0f;
    m_dir = 1.0f;
    m_accel = 0.0f;
    m_maxSpeed = 0.0f;
    m_scale = 1.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grIceRot* grIceRot::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceRot* ground = new (Heaps::StageInstance) grIceRot(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceRot::~grIceRot() {
}

void grIceRot::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateRot(deltaFrame);
        updateCallBack(deltaFrame);
        m_hasUpdatedG3dCalcWorld = false;
        updateG3dProcCalcWorld();
    }
}

// The platform tips over when a fighter lands on it: it turns until it reaches the limit, swings back and forth with a
// smaller and smaller swing and finally comes back to its place.
void grIceRot::updateRot(float deltaFrame) {
    if (m_mtxWork == NULL) {
        return;
    }
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data == NULL) {
        return;
    }
    Vec3f rot;
    m_mtxWork->getRotate(&rot);
    float timer = m_timer;
    m_timer = timer - deltaFrame;
    if (timer - deltaFrame <= 0.0f) {
        m_timer = 0.0f;
    }
    timer = m_landTimer;
    m_landTimer = timer - deltaFrame;
    if (timer - deltaFrame < 0.0f) {
        m_landTimer = 0.0f;
    }
    switch (m_state) {
    case 0:
        if (m_isEnableCollisionStatus == 0) {
            setEnableCollisionStatus(1);
        }
        m_speed = 0.0f;
        m_dir = 1.0f;
        m_accel = data->unk40;
        m_maxSpeed = data->unk44;
        m_scale = 1.0f;
        m_state = 1;
        break;
    case 1:
        if (m_timer == 0.0f && m_landTimer != 0.0f) {
            m_landTimer = 0.0f;
            setEnableCollisionStatus(0);
            m_snd.playSE(static_cast<SndID>(0x1C82), 0, 0, -1);
            m_state = 0xC;
        }
        break;
    case 0xC:
        m_speed = m_speed + m_accel * m_dir;
        if (m_maxSpeed < m_speed) {
            m_speed = m_maxSpeed;
        }
        m_angle = m_angle + m_speed;
        if ((data->unk48 - 90.0f) + 90.0f < rot.m_z + m_angle) {
            m_speed = m_speed * data->unk4C;
            m_dir = m_dir * -1.0f;
            m_state = 0xD;
            m_scale = m_scale * data->unk4C;
        }
        break;
    case 0xE:
        if (m_timer == 0.0f) {
            m_state = 0xF;
        }
        // fall through
    case 0xD: {
        float limit = m_maxSpeed * m_scale;
        float speed = m_speed + m_dir * m_accel * m_scale;
        m_speed = speed;
        if (limit < __fabs(speed)) {
            m_speed = m_dir * limit;
        }
        float angle = m_angle;
        m_angle = angle + m_speed;
        if ((0.0f < m_dir && (data->unk48 - 90.0f) * m_scale + 90.0f < rot.m_z + angle + m_speed) ||
            (m_dir < 0.0f && rot.m_z + m_angle < 90.0f - (data->unk48 - 90.0f) * m_scale)) {
            m_speed = m_speed * data->unk4C;
            m_dir = m_dir * -1.0f;
            m_scale = m_scale * data->unk4C;
            if (m_scale < 0.1f && m_state == 0xD) {
                m_timer = data->unk50;
                m_state = 0xE;
            }
        }
        break;
    }
    case 0xF:
        m_angle = m_angle - data->unk54;
        if (m_angle < 0.0f) {
            m_angle = 0.0f;
            m_snd.playSE(static_cast<SndID>(0x1C83), 0, 0, -1);
            m_state = 0;
        }
        break;
    }
}

void grIceRot::updateCallBack(float deltaFrame) {
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
            if (m_mtxWork != NULL) {
                Matrix matrix = *m_mtxWork;
                fn_8003EBF4(&matrix, m_angle * 0.017453292f);
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = matrix;
                Vec3f pos;
                pos.m_x = matrix.m[0][3];
                pos.m_y = matrix.m[1][3];
                pos.m_z = matrix.m[2][3];
                m_snd.setPos(&pos);
            }
        }
    }
}

// The platform only tips when a fighter lands on the top of it (the 1) and the stage is not busy.
void grIceRot::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data != NULL && m_state == 1) {
        m_landTimer = 5.0f;
        if (isFirstContact == true) {
            m_timer = data->unk3C;
        }
    }
}
