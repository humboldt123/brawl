#include <memory.h>
#include <string.h>
#include <types.h>

#include <st_homerun/gr_homerun.h>

grHomerunLoopSky* grHomerunLoopSky::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHomerunLoopSky* ground = new (Heaps::StageInstance) grHomerunLoopSky(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

inline grHomerunLoopSky::grHomerunLoopSky(const char* taskName) : grHomerunLoop(taskName) {
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
}

grHomerunLoopSky::~grHomerunLoopSky() { }

void grHomerunLoopSky::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateScroll(deltaFrame);
        if (m_frameSceneWork != NULL) {
            setMotionFrame(*m_frameSceneWork, 0);
        }
    }
}

// The four sky pieces (type 2 to 5) are 1500 units apart; each publishes its position to the stage (m_posSky).
void grHomerunLoopSky::updateScroll(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        updateG3dProcCalcWorld();
        m_hasUpdatedG3dCalcWorld = false;
        getNodePosition(&m_pos, 0, m_nodeIndex);
        switch (m_type) {
        case 2:
            m_pos.m_x -= 1500.0f;
            break;
        case 4:
            m_pos.m_x += 1500.0f;
            break;
        case 5:
            m_pos.m_x += 3000.0f;
            break;
        }
        switch (m_type) {
        case 2:
            m_posSkyWork[0] = m_pos;
            break;
        case 3:
            m_posSkyWork[1] = m_pos;
            break;
        case 4:
            m_posSkyWork[2] = m_pos;
            break;
        case 5:
            m_posSkyWork[3] = m_pos;
            break;
        }
        m_state = 1;
        // FALL-THROUGH
    case 1:
        m_state = 2;
        break;
    }
}

void grHomerunLoopSky::fixpos() {
    grHomerunLoop::fixpos();
    if (m_isUpdate) {
        fixposScroll();
        fixposCallBack();
        if (*m_scrollRateWork > 0.0f) {
            updateG3dProcCalcWorld();
            m_hasUpdatedG3dCalcWorld = false;
        }
    }
}

// The sky scrolls with the stage; a piece that left the screen is put behind the piece that is the furthest right.
void grHomerunLoopSky::fixposScroll() {
    switch (m_state) {
    case 2:
        if (*m_scrollRateWork > 0.0f) {
            m_pos.m_x -= *m_scrollWork;
        }
        if (m_pos.m_x < *m_posLimitWork - 6000.0f) {
            switch (m_type) {
            case 2:
                m_pos.m_x = 3000.0f + m_posSkyWork[3].m_x;
                if (*m_scrollRateWork > 0.0f) {
                    m_pos.m_x -= *m_scrollWork;
                }
                break;
            case 3:
                m_pos.m_x = 3000.0f + m_posSkyWork[0].m_x;
                break;
            case 4:
                m_pos.m_x = 3000.0f + m_posSkyWork[1].m_x;
                break;
            case 5:
                m_pos.m_x = 3000.0f + m_posSkyWork[2].m_x;
                break;
            }
        }
        switch (m_type) {
        case 2:
            m_posSkyWork[0] = m_pos;
            break;
        case 3:
            m_posSkyWork[1] = m_pos;
            break;
        case 4:
            m_posSkyWork[2] = m_pos;
            break;
        case 5:
            m_posSkyWork[3] = m_pos;
            break;
        }
    }
}

void grHomerunLoopSky::fixposCallBack() {
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
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data[0].m_pos.m_x = m_pos.m_x;
            data[0].m_pos.m_y = m_pos.m_y;
            data[0].m_pos.m_z = m_pos.m_z;
            Vec3f offset(0.0f, -250.0f, 250.0f);
            calcWorldCallBack->m_nodeCallbackDatas[0].m_offsetPos = offset;
        }
    }
}

