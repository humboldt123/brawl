#include <memory.h>
#include <string.h>
#include <types.h>

#include <st_pictchat/gr_pictchat.h>

// ----- the side bar -----

grPictchatSideBar* grPictchatSideBar::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatSideBar* ground = new (Heaps::StageInstance) grPictchatSideBar(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatSideBar::grPictchatSideBar(const char* taskName) : grPictchat(taskName) {
    m_posGimmickWork = NULL;
    memset(m_nodeLamp, 0, sizeof(m_nodeLamp));
    m_pictCountWork = NULL;
}

grPictchatSideBar::~grPictchatSideBar() {
}

// The places of the lamps (their nodes) are given to the stage.
void grPictchatSideBar::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        getNodePosition(&m_posGimmickWork[0], 0, m_nodeLamp[0]);
        getNodePosition(&m_posGimmickWork[1], 0, m_nodeLamp[1]);
        getNodePosition(&m_posGimmickWork[2], 0, m_nodeLamp[2]);
        getNodePosition(&m_posGimmickWork[3], 0, m_nodeLamp[3]);
        getNodePosition(&m_posGimmickWork[4], 0, m_nodeLamp[4]);
        getNodePosition(&m_posGimmickWork[5], 0, m_nodeLamp[5]);
        getNodePosition(&m_posGimmickWork[6], 0, m_nodeLamp[6]);
        getNodePosition(&m_posGimmickWork[7], 0, m_nodeLamp[7]);
        getNodePosition(&m_posGimmickWork[8], 0, m_nodeLamp[8]);
        getNodePosition(&m_posGimmickWork[9], 0, m_nodeLamp[9]);
        getNodePosition(&m_posGimmickWork[10], 0, m_nodeLamp[10]);
        getNodePosition(&m_posGimmickWork[11], 0, m_nodeLamp[11]);
        getNodePosition(&m_posGimmickWork[12], 0, m_nodeLamp[12]);
        getNodePosition(&m_posGimmickWork[13], 0, m_nodeLamp[13]);
        getNodePosition(&m_posGimmickWork[14], 0, m_nodeLamp[14]);
        getNodePosition(&m_posGimmickWork[15], 0, m_nodeLamp[15]);
        getNodePosition(&m_posGimmickWork[16], 0, m_nodeLamp[16]);
        getNodePosition(&m_posGimmickWork[17], 0, m_nodeLamp[17]);
        getNodePosition(&m_posGimmickWork[18], 0, m_nodeLamp[18]);
        getNodePosition(&m_posGimmickWork[19], 0, m_nodeLamp[19]);
        getNodePosition(&m_posGimmickWork[20], 0, m_nodeLamp[20]);
        getNodePosition(&m_posGimmickWork[21], 0, m_nodeLamp[21]);
        getNodePosition(&m_posGimmickWork[22], 0, m_nodeLamp[22]);
    }
}

// The lamps are all off at first, and the lamp of the number of the pictures that were shown is on.
void grPictchatSideBar::update(float deltaFrame) {
    switch (m_state) {
    case 0:
        setNodeVisibility(false, 0, m_nodeLamp[0], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[1], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[2], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[3], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[4], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[5], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[6], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[7], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[8], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[9], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[10], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[11], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[12], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[13], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[14], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[15], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[16], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[17], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[18], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[19], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[20], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[21], true, false);
        setNodeVisibility(false, 0, m_nodeLamp[22], true, false);
        m_state = 1;
    case 1:
        m_state = 5;
    case 5:
        if (*m_pictCountWork != 0x17) {
            setNodeVisibility(true, 0, m_nodeLamp[*m_pictCountWork], true, false);
        }
        break;
    default:
        return;
    }
}

bool grPictchatSideBar::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeLamp[0], 0, "GLamp_locator_01");
    getNodeIndex(&m_nodeLamp[1], 0, "GLamp_locator_02");
    getNodeIndex(&m_nodeLamp[2], 0, "GLamp_locator_03");
    getNodeIndex(&m_nodeLamp[3], 0, "GLamp_locator_04");
    getNodeIndex(&m_nodeLamp[4], 0, "GLamp_locator_05");
    getNodeIndex(&m_nodeLamp[5], 0, "GLamp_locator_06");
    getNodeIndex(&m_nodeLamp[6], 0, "GLamp_locator_07");
    getNodeIndex(&m_nodeLamp[7], 0, "GLamp_locator_08");
    getNodeIndex(&m_nodeLamp[8], 0, "GLamp_locator_09");
    getNodeIndex(&m_nodeLamp[9], 0, "GLamp_locator_10");
    getNodeIndex(&m_nodeLamp[10], 0, "GLamp_locator_11");
    getNodeIndex(&m_nodeLamp[11], 0, "GLamp_locator_12");
    getNodeIndex(&m_nodeLamp[12], 0, "GLamp_locator_13");
    getNodeIndex(&m_nodeLamp[13], 0, "GLamp_locator_14");
    getNodeIndex(&m_nodeLamp[14], 0, "GLamp_locator_15");
    getNodeIndex(&m_nodeLamp[15], 0, "GLamp_locator_16");
    getNodeIndex(&m_nodeLamp[16], 0, "GLamp_locator_17");
    getNodeIndex(&m_nodeLamp[17], 0, "GLamp_locator_18");
    getNodeIndex(&m_nodeLamp[18], 0, "GLamp_locator_19");
    getNodeIndex(&m_nodeLamp[19], 0, "GLamp_locator_20");
    getNodeIndex(&m_nodeLamp[20], 0, "GLamp_locator_21");
    getNodeIndex(&m_nodeLamp[21], 0, "GLamp_locator_22");
    getNodeIndex(&m_nodeLamp[22], 0, "GLamp_locator_23");
    return result;
}

// ----- one lamp of the side bar -----

inline grPictchatSideBarLamp::grPictchatSideBarLamp(const char* taskName) : grPictchat(taskName) {
    m_posWork = NULL;
    unk15C = 0.0f;
    unk160 = 0.0f;
    unk164 = 0.0f;
    m_type = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
}

grPictchatSideBarLamp* grPictchatSideBarLamp::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatSideBarLamp* ground = new (Heaps::StageInstance) grPictchatSideBarLamp(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatSideBarLamp::~grPictchatSideBarLamp() {
}

void grPictchatSideBarLamp::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateCallBack(deltaFrame);
    }
}

// The place of the lamp is the place the stage has for its type (4 - 7: the four lamps).
void grPictchatSideBarLamp::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            int index;
            switch (m_type) {
            case 4:
                index = 0;
                break;
            case 5:
                index = 1;
                break;
            case 6:
                index = 2;
                break;
            case 7:
                index = 3;
                break;
            default:
                return;
            }
            Vec3f* pos = &m_posWork[index];
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = pos->m_x;
            data->m_pos.m_y = pos->m_y;
            data->m_pos.m_z = pos->m_z;
        }
    }
}
