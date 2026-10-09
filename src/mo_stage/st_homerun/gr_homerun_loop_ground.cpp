#include <memory.h>
#include <string.h>
#include <types.h>

#include <st_homerun/gr_homerun.h>

grHomerunLoopGround* grHomerunLoopGround::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHomerunLoopGround* ground = new (Heaps::StageInstance) grHomerunLoopGround(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHomerunLoopGround::grHomerunLoopGround(const char* taskName) : grHomerunLoop(taskName) {
    m_index = 0;
    memset(m_node, 0, sizeof(m_node));
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 12;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[1].m_flags |= 1;
    callback->m_nodeCallbackDatas[2].m_flags |= 1;
    callback->m_nodeCallbackDatas[3].m_flags |= 1;
    callback->m_nodeCallbackDatas[4].m_flags |= 1;
    callback->m_nodeCallbackDatas[5].m_flags |= 1;
    callback->m_nodeCallbackDatas[6].m_flags |= 1;
    callback->m_nodeCallbackDatas[7].m_flags |= 1;
    callback->m_nodeCallbackDatas[8].m_flags |= 1;
    callback->m_nodeCallbackDatas[9].m_flags |= 1;
    callback->m_nodeCallbackDatas[10].m_flags |= 1;
    callback->m_nodeCallbackDatas[11].m_flags |= 1;
}

grHomerunLoopGround::~grHomerunLoopGround() { }

void grHomerunLoopGround::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateScroll(deltaFrame);
    }
}

// The positions of the twelve ground strips are read once; the stage keeps them (m_posGround).
void grHomerunLoopGround::updateScroll(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        for (u8 i = 0; i < 12; i++) {
            getNodePosition(&m_posGroundWork[i], 0, m_node[i]);
        }
        m_state = 1;
        // FALL-THROUGH
    case 1:
        m_state = 2;
        break;
    }
}

void grHomerunLoopGround::fixpos() {
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

// The strips move with the scroll; every strip that left the screen is put behind the last one (250 units further).
void grHomerunLoopGround::fixposScroll() {
    switch (m_state) {
    case 2:
        if (*m_scrollRateWork > 0.0f) {
            for (u8 i = 0; i < 12; i++) {
                m_posGroundWork[i].m_x -= *m_scrollWork;
            }
        }
        for (;;) {
            bool wrapped = m_posGroundWork[m_index].m_x < *m_posLimitWork - 625.0f;
            if (!wrapped) {
                return;
            }
            int prev = m_index + 11;
            if (prev >= 12) {
                prev -= 12;
            }
            m_posGroundWork[m_index].m_x = 250.0f + m_posGroundWork[prev].m_x;
            m_index++;
            if (m_index >= 12) {
                m_index -= 12;
            }
        }
    }
}

void grHomerunLoopGround::fixposCallBack() {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_node[0];
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_node[1];
                calcWorldCallBack->m_nodeCallbackDatas[2].m_nodeIndex = m_node[2];
                calcWorldCallBack->m_nodeCallbackDatas[3].m_nodeIndex = m_node[3];
                calcWorldCallBack->m_nodeCallbackDatas[4].m_nodeIndex = m_node[4];
                calcWorldCallBack->m_nodeCallbackDatas[5].m_nodeIndex = m_node[5];
                calcWorldCallBack->m_nodeCallbackDatas[6].m_nodeIndex = m_node[6];
                calcWorldCallBack->m_nodeCallbackDatas[7].m_nodeIndex = m_node[7];
                calcWorldCallBack->m_nodeCallbackDatas[8].m_nodeIndex = m_node[8];
                calcWorldCallBack->m_nodeCallbackDatas[9].m_nodeIndex = m_node[9];
                calcWorldCallBack->m_nodeCallbackDatas[10].m_nodeIndex = m_node[10];
                calcWorldCallBack->m_nodeCallbackDatas[11].m_nodeIndex = m_node[11];
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            for (u8 i = 0; i < 12; i++) {
                calcWorldCallBack->m_nodeCallbackDatas[i].m_pos = m_posGroundWork[i];
            }
        }
    }
}

bool grHomerunLoopGround::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "Ground_A_01");
    getNodeIndex(&m_node[1], 0, "Ground_A_02");
    getNodeIndex(&m_node[2], 0, "Ground_B_01");
    getNodeIndex(&m_node[3], 0, "Ground_B_02");
    getNodeIndex(&m_node[4], 0, "Ground_C_01");
    getNodeIndex(&m_node[5], 0, "Ground_C_02");
    getNodeIndex(&m_node[6], 0, "Ground_D_01");
    getNodeIndex(&m_node[7], 0, "Ground_D_02");
    getNodeIndex(&m_node[8], 0, "Ground_E_01");
    getNodeIndex(&m_node[9], 0, "Ground_E_02");
    getNodeIndex(&m_node[10], 0, "Ground_F_01");
    getNodeIndex(&m_node[11], 0, "Ground_F_02");
    return result;
}

