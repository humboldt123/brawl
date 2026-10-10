#include <ec/ec_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_ice/gr_ice.h>

grIceIce::grIceIce(const char* taskName) : grIce(taskName), m_snd() {
    m_mtxWork = NULL;
    m_mtxGimmickWork = NULL;
    m_posX = 0.0f;
    m_posY = 0.0f;
    m_posZ = 0.0f;
    m_unk16C = 0.0f;
    m_bobY = 0.0f;
    m_unk174 = 0.0f;
    m_stateWork = NULL;
    m_landTimer = 0.0f;
    m_limitWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grIceIce* grIceIce::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceIce* ground = new (Heaps::StageInstance) grIceIce(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceIce::~grIceIce() {
}

void grIceIce::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateUpDown(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The ice block comes in from the left, moves to the right and sinks when it reaches the second part of the model (the
// water at the right side of the stage), where it sends up a splash.
void grIceIce::updateMove(float deltaFrame) {
    if (m_mtxWork == NULL || m_mtxGimmickWork == NULL) {
        return;
    }
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data == NULL) {
        return;
    }
    switch (m_state) {
    case 1:
        if (*m_stateWork == 5) {
            float y = m_mtxWork->m[1][3];
            m_posX = m_limitWork[0].m_x - 20.0f;
            m_posY = y;
            m_posZ = 0.0f;
            m_state = 0xB;
        }
        break;
    case 0:
        setVisibility(0);
        setEnableCollisionStatus(0);
        m_sinking = 0;
        m_state = 1;
        break;
    case 0xB:
        if (m_sinking != 1) {
            if (!m_isVisible) {
                setVisibility(1);
                setEnableCollisionStatus(1);
            }
            float x = m_posX + data->unkC0 * deltaFrame;
            m_posY = m_mtxWork->m[1][3];
            m_posX = x;
            if (x <= m_limitWork[1].m_x + 20.0f) {
                float limit = m_mtxGimmickWork->m[0][3];
                Matrix mtx(true);
                bool found = getNodeMatrix(&mtx, 0, "P_RSide1");
                u32 side = (found == true) ? 1 : 2;
                if (!found) {
                    found = getNodeMatrix(&mtx, 0, "P_RSide2");
                }
                if (found && limit <= mtx.m[0][3] + data->unkC0 * deltaFrame) {
                    m_sinking = 1;
                    u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x480003));
                    if (side == 1) {
                        g_ecMgr->setParent(effect, m_sceneModels[0], "Ryuuhyou1", 0);
                    }
                    if (side == 2) {
                        g_ecMgr->setParent(effect, m_sceneModels[0], "Ryuuhyou2", 0);
                    }
                    m_snd.playSE(static_cast<SndID>(0x1C86), 0, 0, -1);
                }
            } else {
                *m_stateWork = 0xE;
                m_state = 0;
            }
        }
        break;
    }
}

// The block bobs: it sinks while a fighter stands on it and comes back when the fighter left; when it went down too far it
// sinks for good.
void grIceIce::updateUpDown(float deltaFrame) {
    if (m_mtxWork == NULL || m_limitWork == NULL) {
        return;
    }
    stIceData* data = static_cast<stIceData*>(getStageData());
    if (data == NULL) {
        return;
    }
    float timer = m_landTimer - deltaFrame;
    m_landTimer = timer;
    if (timer < 0.0f) {
        m_landTimer = 0.0f;
    }
    if (m_sinking == 1) {
        float y = m_bobY - data->unkD4 * deltaFrame;
        m_bobY = y;
        m_posY = m_mtxWork->m[1][3];
        if (y < m_limitWork[1].m_y - 20.0f) {
            *m_stateWork = 0xE;
            m_state = 0;
        }
    } else if (m_landTimer == 0.0f) {
        float y = m_bobY + data->unkCC * deltaFrame;
        m_bobY = y;
        if (0.0f < y) {
            m_bobY = 0.0f;
        }
    } else {
        float y = m_bobY - data->unkC4 * deltaFrame;
        m_bobY = y;
        if (data->unkD0 <= __fabs(y)) {
            m_sinking = 1;
        }
    }
}

void grIceIce::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = m_posX;
            data->m_pos.m_y = m_posY;
            data->m_pos.m_z = m_posZ;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_y = data->m_pos.m_y + m_bobY;
            m_snd.setPos(reinterpret_cast<Vec3f*>(&m_posX));
        }
    }
}

// The block starts to sink when a fighter lands on it (the first time with a sound).
void grIceIce::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    if (m_landTimer == 0.0f) {
        m_snd.playSE(static_cast<SndID>(0x1C85), 0, 0, -1);
    }
    m_landTimer = 5.0f;
}
