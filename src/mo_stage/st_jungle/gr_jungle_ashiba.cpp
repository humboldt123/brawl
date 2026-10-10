#include <memory.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>

grJungleAshiba* grJungleAshiba::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba* ground = new (Heaps::StageInstance) grJungleAshiba(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba::grJungleAshiba(const char* taskName) : grJungle(taskName) {
    m_posWork = NULL;
    m_posGimmickWork = NULL;
    m_posHashigoWork = NULL;
    m_posLimitWork = NULL;
    m_mtxGimmickWork = NULL;
    m_enableWork = NULL;
    m_posPrev.m_x = 0.0f;
    m_posPrev.m_y = 0.0f;
    m_posPrev.m_z = 0.0f;
    m_yakumonoMade = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
}

grJungleAshiba::~grJungleAshiba() {
}

// The platform is updated while it is on the screen; when the place it follows has gone up (the stage was scrolled back, or
// the platform was reused) the hit object is reset.
void grJungleAshiba::update(float deltaFrame) {
    grJungle::update(deltaFrame);
    m_hasUpdatedG3dCalcWorld = false;
    if (m_isUpdate) {
        updateBaseActive();
        if (m_posPrev.m_y < m_posWork->m_y) {
            resetYakumono();
        }
        m_posPrev.m_x = m_posWork->m_x;
        m_posPrev.m_y = m_posWork->m_y;
        m_posPrev.m_z = m_posWork->m_z;
        updateCallBack(deltaFrame);
    }
}

void grJungleAshiba::resetYakumono() {
}

// The platform is shown and has its collision while the stage says it is on the screen.
void grJungleAshiba::updateBaseActive() {
    if (m_enableWork != NULL) {
        switch (*m_enableWork) {
        case 0:
            if (m_isVisible == 1) {
                setVisibility(0);
            }
            if (m_isEnableCollisionStatus == 1) {
                setEnableCollisionStatus(false);
            }
            setDisableYakumono();
            break;
        case 1:
            if (!m_isVisible) {
                setVisibility(1);
            }
            if (!m_isEnableCollisionStatus) {
                setEnableCollisionStatus(true);
                if (m_collision != NULL) {
                    m_collision->setEnable();
                }
            }
            setEnableYakumono();
            break;
        }
    }
}

void grJungleAshiba::setDisableYakumono() {
}

void grJungleAshiba::setEnableYakumono() {
}

void grJungleAshiba::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = *m_sceneModels;
        if (scnMdl != NULL) {
            if (calcWorldCallBack->m_nodeCallbackDatas == NULL) {
                calcWorldCallBack->m_numNodeCallbackData = getCallBackNodeCount();
                calcWorldCallBack->initialize(false, Heaps::StageInstance);
                calcWorldCallBack->m_nodeCallbackDatas[0].m_flags |= 1;
            }
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_posWork != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = m_posWork->m_x;
                data->m_pos.m_y = m_posWork->m_y;
                data->m_pos.m_z = m_posWork->m_z;
            }
        }
    }
}

int grJungleAshiba::getCallBackNodeCount() {
    return 1;
}

void grJungleAshiba::setFrameWork() {
}
