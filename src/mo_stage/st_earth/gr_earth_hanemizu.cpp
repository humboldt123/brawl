#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

#include <st_earth/gr_earth.h>

grEarthHaneMizu* grEarthHaneMizu::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthHaneMizu* ground = new (Heaps::StageInstance) grEarthHaneMizu(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthHaneMizu::~grEarthHaneMizu() {
}

void grEarthHaneMizu::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateWeather();
        updateCallBack();
    }
}

// The drops show while it rains (the weather 1 or 2) and are hidden when it is fine.
void grEarthHaneMizu::updateWeather() {
    switch (m_state) {
    case 0:
        setVisibility(0);
        m_state = 0xB;
        break;
    case 0xA:
        m_state = 0xB;
        break;
    case 0xB:
        if (*m_weatherWork != 3 && *m_weatherWork != 0) {
            m_state = 0xD;
        }
        break;
    case 0xD:
        if (*m_weatherWork == 2) {
            m_state = 0xE;
        }
        break;
    case 0xE:
        if (*m_weatherWork == 1 || *m_weatherWork == 2) {
            setVisibility(1);
        } else {
            setVisibility(0);
            m_state = 0xA;
        }
        break;
    }
}

void grEarthHaneMizu::updateCallBack() {
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
            Vec3f* offset = m_posOffsetWork;
            if (offset != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_offsetPos.m_x = offset->m_x;
                data->m_offsetPos.m_y = offset->m_y;
                data->m_offsetPos.m_z = offset->m_z;
            }
        }
    }
}
