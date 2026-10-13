#include <st_dxbigblue/gr_dxbigblue_falcon.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>
#include <st_dxbigblue/st_dxbigblue_data.h>

grDxBigBlueFalcon* grDxBigBlueFalcon::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grDxBigBlueFalcon* ground = new (Heaps::StageInstance) grDxBigBlueFalcon(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grDxBigBlueFalcon::~grDxBigBlueFalcon() { }

void grDxBigBlueFalcon::updateActive(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
            case 0:
                m_flyDataWork->m_pos.m_y = data->unk00 + data->unkCC;
                m_state = 1;
                break;
            case 1:
                if (m_flyDataWork->m_state != 10) {
                    if (unk168 == 1) {
                        m_flyDataWork->m_pos.m_x = 0.0f;
                        unk168 = 0;
                    } else {
                        m_flyDataWork->m_pos.m_x = m_posLimitWork[0] - 100.0f;
                    }
                    m_state = 2;
                }
                break;
            case 2:
                if (m_flyDataWork->m_state == 10) {
                    setVisibility(0);
                    setEnableCollisionStatus(false);
                    m_state = 0;
                } else {
                    if (!m_isVisible) {
                        setVisibility(1);
                        setEnableCollisionStatus(true);
                    }
                    updateMove(deltaFrame);
                }
                break;
        }
    }
}

void grDxBigBlueFalcon::updateMove(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        unk158 -= deltaFrame;
        if (unk158 < 0.0f) {
            unk158 = 0.0f;
        }
        switch (m_flyDataWork->m_state) {
            case 1:
                m_flyDataWork->m_pos.m_x += data->unkD0 * deltaFrame;
                if (m_flyDataWork->m_pos.m_x > 0.0f) {
                    m_flyDataWork->m_pos.m_x = 0.0f;
                    unk158 = data->unkD8;
                    m_flyDataWork->m_state = 2;
                }
                break;
            case 2:
                if (0.0f == unk158) {
                    m_flyDataWork->m_state = 3;
                }
                break;
            case 3:
                m_flyDataWork->m_pos.m_x += data->unkD0 * deltaFrame;
                if (m_flyDataWork->m_pos.m_x > 100.0f + m_posLimitWork[3]) {
                    m_flyDataWork->m_state = 10;
                }
                break;
        }
    }
}

void grDxBigBlueFalcon::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            DXBIGBLUE_BIND_CALLBACK(calcWorldCallBack, scnMdl)
            stDxBigBlueData* data;
            if (m_flyDataWork != NULL && (data = static_cast<stDxBigBlueData*>(getStageData())) != NULL) {
                grNodeCallbackData* node = &calcWorldCallBack->m_nodeCallbackDatas[0];
                Vec3f rot;
                rot = node->m_rot;
                rot.m_z = rot.m_z + (m_flyDataWork->m_rot.m_z - rot.m_z) * data->unkD4;
                node->m_pos = m_flyDataWork->m_pos;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot = rot;
            }
        }
    }
}
