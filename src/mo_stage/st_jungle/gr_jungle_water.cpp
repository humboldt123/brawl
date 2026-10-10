#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleWater::grJungleWater(const char* taskName) : grJungle(taskName) {
    m_posWork = NULL;
    m_enableWork = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas->m_flags |= 1;
    }
}

grJungleWater* grJungleWater::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleWater* ground = new (Heaps::StageInstance) grJungleWater(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleWater::~grJungleWater() {
}

void grJungleWater::update(float deltaFrame) {
    grJungle::update(deltaFrame);
    if (m_isUpdate) {
        if (m_enableWork != NULL) {
            switch (*m_enableWork) {
            case 0:
                if (m_isVisible == 1) {
                    setVisibility(0);
                }
                break;
            case 1:
                if (!m_isVisible) {
                    setVisibility(1);
                }
                break;
            }
        }
        updateCallBack(deltaFrame);
    }
}

void grJungleWater::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = *m_sceneModels;
        if (scnMdl != NULL) {
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
