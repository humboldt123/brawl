#include <memory.h>
#include <string.h>
#include <types.h>

#include <st_homerun/gr_homerun.h>

grHomerunScore* grHomerunScore::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHomerunScore* ground = new (Heaps::StageInstance) grHomerunScore(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHomerunScore::grHomerunScore(const char* taskName) : grHomerun(taskName) {
    m_posWork = NULL;
    m_posNumberWork = NULL;
    m_posZeroWork = NULL;
    m_posLimitWork = NULL;
    m_scoreWork = NULL;
    m_scoreSandBagWork = NULL;
    m_scrollWork = NULL;
    m_scrollRateWork = NULL;
    memset(m_node, 0, sizeof(m_node));
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grHomerunScore::~grHomerunScore() { }

void grHomerunScore::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateScore(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// A score board shows the distance of the next 300 feet: it sits at that distance from the launch point (it is the
// sandbag's distance when the stage scrolls) and is raised by 300 once the sandbag passed it. The digit nodes of the
// board hand their positions to the number grounds.
void grHomerunScore::updateScore(float deltaFrame) {
    if (m_posNumberWork == NULL) {
        return;
    }
    switch (m_state) {
    case 0:
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
        if (!m_isVisible) {
            setVisibility(1);
        }
        if (*m_scrollRateWork > 0.0f) {
            m_posWork->m_x = 2000.0f + (500.0f * ((float)*m_scoreWork - *m_scoreSandBagWork)) / 50.0f / 3.28084f;
        } else {
            m_posWork->m_x = m_posZeroWork->m_x + (500.0f * (float)*m_scoreWork) / 50.0f / 3.28084f;
        }
        m_posWork->m_y = m_posZeroWork->m_y;
        m_posWork->m_z = -60.0f;
        if (m_posWork->m_x < *m_posLimitWork - 187.5f) {
            *m_scoreWork = (u32)((float)*m_scoreWork + 300.0f);
            m_posWork->m_x = m_posZeroWork->m_x + (500.0f * (float)*m_scoreWork) / 50.0f / 3.28084f;
        }
        Vec3f nodePos;
        getNodePosition(&nodePos, 0, m_nodeIndex);
        Vec3f offset;
        offset = *m_posWork - nodePos;
        getNodePosition(&m_posNumberWork[0], 0, m_node[0]);
        getNodePosition(&m_posNumberWork[1], 0, m_node[1]);
        getNodePosition(&m_posNumberWork[2], 0, m_node[2]);
        getNodePosition(&m_posNumberWork[3], 0, m_node[3]);
        m_posNumberWork[0].m_x += offset.m_x;
        m_posNumberWork[1].m_x += offset.m_x;
        m_posNumberWork[2].m_x += offset.m_x;
        m_posNumberWork[3].m_x += offset.m_x;
        break;
    }
    case 8:
        if (*m_scoreWork != 0) {
            m_state = 2;
        }
        break;
    }
}

void grHomerunScore::updateCallBack(float deltaFrame) {
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

bool grHomerunScore::setNode() {
    bool result = grGimmick::setNode();
    if (m_sceneModels[0] == NULL) {
        return false;
    }
    if (m_sceneModels[0]->m_resMdl.IsValid() == false) {
        return false;
    }
    getNodeIndex(&m_node[0], 0, "Nunber_Position_A_01");
    getNodeIndex(&m_node[1], 0, "Nunber_Position_A_02");
    getNodeIndex(&m_node[2], 0, "Nunber_Position_A_03");
    getNodeIndex(&m_node[3], 0, "Nunber_Position_A_04");
    return result;
}

