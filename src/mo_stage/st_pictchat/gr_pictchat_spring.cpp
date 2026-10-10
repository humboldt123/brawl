#include <memory.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <string.h>
#include <types.h>

#include <st_pictchat/gr_pictchat.h>

// grGimmickSpring::presentShootEvent (sora_melee, unnamed)
extern "C" void fn_27_2704CC(grGimmickSpring* spring);

// The spring of the picture (it is there only while the picture is).
grPictchatSpring::grPictchatSpring(const char* taskName) : grGimmickSpring(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_pictIDWork = NULL;
    m_pictID = 0;
    m_posWork = NULL;
    m_stateWork = NULL;
    m_flgWork = NULL;
    m_triggerMade = 0;
    m_startFlag = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
        setupMelee();
    }
}

grPictchatSpring* grPictchatSpring::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatSpring* ground = new (Heaps::StageInstance) grPictchatSpring(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatSpring::~grPictchatSpring() {
}

void grPictchatSpring::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
}

void grPictchatSpring::processGameProc() {
    Ground::processGameProc();
    if (m_startFlag == 1) {
        disableArea();
        m_startFlag = 0;
    }
}

void grPictchatSpring::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateYakumono(deltaFrame);
    updateActive(deltaFrame);
    updateCallBack(deltaFrame);
}

// Once the stage hands over the data of the spring, the area of it is added to the Yakumono and a spring trigger observes
// the Yakumono. After that the spring follows the position of its node.
void grPictchatSpring::updateYakumono(float deltaFrame) {
    if (m_triggerMade == 1) {
        Vec3f pos;
        getTopNode(&pos);
        setPos(&pos);
    } else {
        m_springData = static_cast<grGimmickSpringData*>(getGimmickData());
        memset(&m_ykData, 0, sizeof(m_ykData));
        // MATCH-ONLY: the shape type and the group of the area are set as two bytes
        reinterpret_cast<u8*>(&m_areaData)[0] = 0;
        reinterpret_cast<u8*>(&m_areaData)[1] = gfArea::Stage_Group_Gimmick_Normal;
        m_areaData.m_0x4 = 0;
        m_areaData.m_0x8 = 0;
        m_areaData.m_shapeFlag.m_mask = 1;
        m_areaData.m_nodeIndex = 0;
        m_areaData.m_offsetPos.m_x = m_springData->m_areaData.m_offsetPos.m_x;
        m_areaData.m_offsetPos.m_y = m_springData->m_areaData.m_offsetPos.m_y - 1.0f;
        m_areaData.m_range.m_x = m_springData->m_areaData.m_range.m_x;
        m_areaData.m_range.m_y = m_springData->m_areaData.m_range.m_y + 1.0f;
        grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
        stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Spring, -1);
        trigger->setObserveYakumono(m_yakumono);
        m_triggerMade = 1;
    }
}

// The spring is turned on when the picture is drawn; while it is on, the stage state 2 makes it shoot and the other
// states only report the place.
void grPictchatSpring::updateActive(float deltaFrame) {
    switch (m_state) {
    case 0:
        setVisibility(0);
        m_startFlag = 1;
        m_state = 1;
        break;
    case 1:
        if (*m_pictIDWork == m_pictID && *m_flgWork == 1) {
            setVisibility(1);
            enableArea();
            m_state = 2;
        }
        break;
    case 2:
        if (*m_pictIDWork != m_pictID && *m_stateWork == 5) {
            m_state = 0;
        } else if (*m_stateWork == 2) {
            fn_27_2704CC(this);
            *m_stateWork = 3;
        } else {
            presentPosEvent();
        }
        break;
    }
}

// The place of the spring is given to the callback that moves its model.
void grPictchatSpring::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f* pos = m_posWork;
            if (pos != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = pos->m_x;
                data->m_pos.m_y = pos->m_y;
                data->m_pos.m_z = pos->m_z;
            }
        }
    }
}

void grPictchatSpring::getTopNode(Vec3f* pos) {
    Vec3f* top = m_posWork;
    pos->m_x = top->m_x;
    pos->m_y = top->m_y;
    pos->m_z = top->m_z;
}

// A fighter jumps on the spring: the state of it says it is pushed, and the place of the top is given to the event.
void grPictchatSpring::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    Gimmick::EventKind kind = eventInfo->m_kind;
    if (kind == Gimmick::Event_Exit) {
        return;
    }
    if (kind < Gimmick::Spring_Event_On) {
        return;
    }
    if (kind > Gimmick::Spring_Event_Pos) {
        return;
    }
    if (kind != Gimmick::Spring_Event_On) {
        return;
    }
    *m_stateWork = 0;
    getTopNode(&static_cast<soGimmickSpringEventArgs*>(eventInfo)->m_topPos);
}
