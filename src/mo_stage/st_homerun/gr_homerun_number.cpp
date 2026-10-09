#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

#include <st_homerun/gr_homerun.h>

extern "C" char* itoa(int value, char* buf, int radix);

grHomerunNumber* grHomerunNumber::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHomerunNumber* ground = new (Heaps::StageInstance) grHomerunNumber(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

inline grHomerunNumber::grHomerunNumber(const char* taskName) : grHomerun(taskName) {
    m_posWork = NULL;
    m_scoreWork = NULL;
    m_lastScore = 0;
    m_type = 0;
    memset(m_node, 0, sizeof(m_node));
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grHomerunNumber::~grHomerunNumber() { }

void grHomerunNumber::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateNumber(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// One digit of a score board (the type says which one): the digit model that matches the number is shown.
void grHomerunNumber::updateNumber(float deltaFrame) {
    switch (m_state) {
    case 0:
        hideAll();
        show(0);
        m_state = 1;
        // FALL-THROUGH
    case 1:
        m_state = 2;
        // FALL-THROUGH
    case 2: {
        if (*m_scoreWork == 0) {
            setVisibility(0);
            m_state = 8;
            break;
        }
        if (*m_scoreWork == m_lastScore) {
            break;
        }
        if (!m_isVisible) {
            setVisibility(1);
        }
        m_lastScore = *m_scoreWork;
        hideAll();
        char digit[8];
        char text[16];
        strcpy(digit, "");
        itoa(*m_scoreWork, text, 10);
        u32 length = strlen(text);
        switch (m_type) {
        case 0x12:
            if (length < 1) {
                show(0);
            } else {
                strncpy(digit, text + length - 1, 1);
                show(atoi(digit));
            }
            break;
        case 0x13:
            if (length < 2) {
                show(0);
            } else {
                strncpy(digit, text + length - 2, 1);
                show(atoi(digit));
            }
            break;
        case 0x14:
            if (length < 3) {
                show(0);
            } else {
                strncpy(digit, text + length - 3, 1);
                show(atoi(digit));
            }
            break;
        case 0x15:
            if (length < 4) {
                show(0);
            } else {
                strncpy(digit, text + length - 4, 1);
                show(atoi(digit));
            }
            break;
        }
        break;
    }
    case 8:
        if (*m_scoreWork != 0) {
            m_state = 2;
        }
        break;
    }
}

void grHomerunNumber::updateCallBack(float deltaFrame) {
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
            Vec3f* pos = m_posWork;
            if (pos != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data[0].m_pos.m_x = pos->m_x;
                data[0].m_pos.m_y = pos->m_y;
                data[0].m_pos.m_z = pos->m_z;
            }
        }
    }
}

bool grHomerunNumber::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "number_0");
    getNodeIndex(&m_node[1], 0, "number_1");
    getNodeIndex(&m_node[2], 0, "number_2");
    getNodeIndex(&m_node[3], 0, "number_3");
    getNodeIndex(&m_node[4], 0, "number_4");
    getNodeIndex(&m_node[5], 0, "number_5");
    getNodeIndex(&m_node[6], 0, "number_6");
    getNodeIndex(&m_node[7], 0, "number_7");
    getNodeIndex(&m_node[8], 0, "number_8");
    getNodeIndex(&m_node[9], 0, "number_9");
    return result;
}

void grHomerunNumber::show(u8 index) {
    setNodeVisibility(true, 0, m_node[index], true, false);
}

void grHomerunNumber::hide(u8 index) {
    setNodeVisibility(false, 0, m_node[index], true, false);
}

void grHomerunNumber::hideAll() {
    for (u8 i = 0; i < 10; i++) {
        setNodeVisibility(false, 0, m_node[i], true, false);
    }
}

