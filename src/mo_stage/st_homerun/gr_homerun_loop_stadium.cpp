#include <memory.h>
#include <string.h>
#include <types.h>

#include <st_homerun/gr_homerun.h>

grHomerunLoopStadium* grHomerunLoopStadium::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHomerunLoopStadium* ground = new (Heaps::StageInstance) grHomerunLoopStadium(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHomerunLoopStadium::grHomerunLoopStadium(const char* taskName) : grHomerunLoop(taskName) {
    m_index = 0;
    memset(m_pos, 0, sizeof(m_pos));
    memset(m_node, 0, sizeof(m_node));
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 6;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[1].m_flags |= 1;
    callback->m_nodeCallbackDatas[2].m_flags |= 1;
    callback->m_nodeCallbackDatas[3].m_flags |= 1;
    callback->m_nodeCallbackDatas[4].m_flags |= 1;
    callback->m_nodeCallbackDatas[5].m_flags |= 1;
}

grHomerunLoopStadium::~grHomerunLoopStadium() { }

void grHomerunLoopStadium::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateScroll(deltaFrame);
    }
}

void grHomerunLoopStadium::updateScroll(float deltaFrame) {
    if (m_scrollWork != NULL && m_scrollRateWork != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0:
            updateG3dProcCalcWorld();
            m_hasUpdatedG3dCalcWorld = false;
            getNodePosition(&m_pos[0], 0, m_node[0]);
            getNodePosition(&m_pos[1], 0, m_node[1]);
            getNodePosition(&m_pos[2], 0, m_node[2]);
            getNodePosition(&m_pos[3], 0, m_node[3]);
            getNodePosition(&m_pos[4], 0, m_node[4]);
            getNodePosition(&m_pos[5], 0, m_node[5]);
            m_state = 1;
            // FALL-THROUGH
        case 1:
            m_state = 2;
            break;
        }
    }
}

void grHomerunLoopStadium::fixpos() {
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

// The stands move with the scroll; a piece that left the screen is put behind the last one (they are 420 units wide).
void grHomerunLoopStadium::fixposScroll() {
    switch (m_state) {
    case 2:
        if (*m_scrollRateWork > 0.0f) {
            m_pos[0].m_x -= *m_scrollWork;
            m_pos[1].m_x -= *m_scrollWork;
            m_pos[2].m_x -= *m_scrollWork;
            m_pos[3].m_x -= *m_scrollWork;
            m_pos[4].m_x -= *m_scrollWork;
            m_pos[5].m_x -= *m_scrollWork;
        }
        u8 index = m_index;
        if (m_pos[index].m_x < *m_posLimitWork - 840.0f) {
            int prev = index + 5;
            if (prev >= 6) {
                prev -= 6;
            }
            m_pos[index].m_x = 420.0f + m_pos[prev].m_x;
            m_index++;
            if (m_index >= 6) {
                m_index -= 6;
            }
        }
    }
}

// The six stand pieces are bound to the world callback grouped by their kind (A, A, A, B, B, C).
void grHomerunLoopStadium::fixposCallBack() {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_node[0];
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_node[2];
                calcWorldCallBack->m_nodeCallbackDatas[2].m_nodeIndex = m_node[4];
                calcWorldCallBack->m_nodeCallbackDatas[3].m_nodeIndex = m_node[1];
                calcWorldCallBack->m_nodeCallbackDatas[4].m_nodeIndex = m_node[5];
                calcWorldCallBack->m_nodeCallbackDatas[5].m_nodeIndex = m_node[3];
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = m_pos[0];
            calcWorldCallBack->m_nodeCallbackDatas[1].m_pos = m_pos[2];
            calcWorldCallBack->m_nodeCallbackDatas[2].m_pos = m_pos[4];
            calcWorldCallBack->m_nodeCallbackDatas[3].m_pos = m_pos[1];
            calcWorldCallBack->m_nodeCallbackDatas[4].m_pos = m_pos[5];
            calcWorldCallBack->m_nodeCallbackDatas[5].m_pos = m_pos[3];
        }
    }
}

bool grHomerunLoopStadium::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "SubStadium_A_01");
    getNodeIndex(&m_node[1], 0, "SubStadium_B_01");
    getNodeIndex(&m_node[2], 0, "SubStadium_A_02");
    getNodeIndex(&m_node[3], 0, "SubStadium_C_01");
    getNodeIndex(&m_node[4], 0, "SubStadium_A_03");
    getNodeIndex(&m_node[5], 0, "SubStadium_B_02");
    return result;
}

