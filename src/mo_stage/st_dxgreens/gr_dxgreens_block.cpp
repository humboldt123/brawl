#include <ec/ec_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <string.h>

#include <st_dxgreens/gr_dxgreens.h>
#include <st_dxgreens/gr_dxgreens_anim.h>

grDxGreensBlock::grDxGreensBlock(const char* taskName) : grDxGreens(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_speed = 0.0f;
    m_blockData = NULL;
    m_landed = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
}

grDxGreensBlock* grDxGreensBlock::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGreensBlock* ground = new (Heaps::StageInstance) grDxGreensBlock(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGreensBlock::~grDxGreensBlock() {
}

void grDxGreensBlock::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The life of a block: it waits hidden (0) until the stage releases it, then it is placed at the top (1) and falls down
// to its row (2); when it was broken (the stage sets 4) it goes back to hidden.
void grDxGreensBlock::updateActive(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        setVisibility(0);
        setEnableCollisionStatus(false);
        m_state = 1;
        m_speed = 0.0f;
        break;
    case 1: {
        u8 blockState = m_blockData->m_state;
        if (blockState != 1) {
            if (blockState != 0) {
                return;
            }
            m_blockData->m_pos.m_y = m_posLimit->m_y;
        }
        m_state = 2;
        break;
    }
    case 2:
        if (!m_isVisible) {
            setVisibility(1);
            setEnableCollisionStatus(true);
        }
        if (m_blockData->m_state == 4) {
            m_blockData->m_state = 5;
            m_state = 0;
        } else {
            updatePos(deltaFrame);
        }
        break;
    }
}

// The block falls with growing speed (the stage data holds the largest speed and the pull) and stops at its row.
void grDxGreensBlock::updatePos(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_speed += data[11] * deltaFrame;
        if (data[10] < m_speed) {
            m_speed = data[10];
        }
        m_blockData->m_pos.m_y -= m_speed * deltaFrame;
        if (m_blockData->m_pos.m_y <= m_blockData->m_posTgt.m_y) {
            m_blockData->m_pos.m_y = m_blockData->m_posTgt.m_y;
            if (m_blockData->m_state != 2) {
                m_landed = 1;
            }
            m_blockData->m_state = 2;
            m_speed = 0.0f;
        }
    }
}

// Hard blocks show the cracked texture: their animation runs to the end and stays on the last frame, the others stay on
// the first.
void grDxGreensBlock::updateMotion(float deltaFrame) {
    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim != NULL) {
        if (m_blockData->m_hard == 1) {
            float frameCount = modelAnim->getFrameCount();
            if (frameCount <= modelAnim->getFrame()) {
                modelAnim->setFrame(2.0f);
            }
        } else {
            modelAnim->setFrame(0.0f);
        }
    }
}

void grDxGreensBlock::updateCallBack(float deltaFrame) {
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
            stDxGreensBlockData* blockData = m_blockData;
            if (blockData != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = blockData->m_pos.m_x;
                data->m_pos.m_y = blockData->m_pos.m_y;
                data->m_pos.m_z = blockData->m_pos.m_z;
            }
        }
    }
}

// A fighter hits the block from below: a block that is falling (0) or at rest after it has landed (2) breaks (3).
void grDxGreensBlock::receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* joint, bool isStartCollision) {
    bool hit = false;
    switch (m_blockData->m_state) {
    case 0:
        hit = true;
        break;
    case 1:
        break;
    case 2:
        if (m_landed == 1) {
            hit = true;
        }
        break;
    }
    if (!hit) {
        return;
    }
    m_blockData->m_state = 3;
    if (m_blockData->m_hard == 1) {
        g_ecMgr->setEffect(static_cast<EfID>(0x5E0003), &m_blockData->m_pos);
    } else {
        g_ecMgr->setEffect(static_cast<EfID>(0x5E0004), &m_blockData->m_pos);
    }
}
