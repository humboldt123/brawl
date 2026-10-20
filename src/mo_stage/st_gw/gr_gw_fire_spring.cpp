#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <types.h>

#include <st_gw/gr_gw_fire_spring.h>
#include <st_gw/gr_gw_item.h>

// grGimmickSpring::presentShootEvent (sora_melee, unnamed)
extern "C" void fn_27_2704CC(grGimmickSpring* spring);

static grGWMarker sMarkerA(0xFF, 0);
static grGWMarker sMarkerB(0xFF, 1);

grGWFireSpring::grGWFireSpring(const char* taskName) : grGimmickSpring(taskName), m_snd() {
    m_sceneIdWork = NULL;
    m_sceneId = 4;
    m_eventWork = NULL;
    m_eventLast = 1;
    m_posWork = NULL;
    m_triggerMade = 0;
    m_startFlag = 0;
    m_scale.m_x = 0.0f;
    m_scale.m_y = 0.0f;
    m_scale.m_z = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    setupMelee();
}

grGWFireSpring* grGWFireSpring::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grGWFireSpring* ground = new (Heaps::StageInstance) grGWFireSpring(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grGWFireSpring::~grGWFireSpring() {
}

void grGWFireSpring::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(archive, unk1, layerType);
}

void grGWFireSpring::processGameProc() {
    Ground::processGameProc();
    if (m_startFlag == 1) {
        disableArea();
        m_startFlag = 0;
    }
}

void grGWFireSpring::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateYakumono(deltaFrame);
    updateScaleBase(deltaFrame);
    updateActive(deltaFrame);
    updateCallBack(deltaFrame);
}

// Once the stage hands over the data of the spring, the area of it is added to the Yakumono and a spring trigger observes
// the Yakumono. After that the spring follows the place of its top.
void grGWFireSpring::updateYakumono(float deltaFrame) {
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
        m_areaData.m_offsetPos.m_y = m_springData->m_areaData.m_offsetPos.m_y;
        m_areaData.m_range.m_x = m_springData->m_areaData.m_range.m_x;
        m_areaData.m_range.m_y = m_springData->m_areaData.m_range.m_y;
        grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
        stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Spring, -1);
        trigger->setObserveYakumono(m_yakumono);
        m_triggerMade = 1;
    }
}

static inline bool gwNearZero(float v) {
    bool result = false;
    if ((float)fabs(v) < 1e-5f) {
        result = true;
    }
    return result;
}

void grGWFireSpring::updateScaleBase(float deltaFrame) {
    bool unset = false;
    if (gwNearZero(m_scale.m_x) && gwNearZero(m_scale.m_y) && gwNearZero(m_scale.m_z)) {
        unset = true;
    }
    if (unset) {
        nw4r::g3d::ResMdl model;
        if (m_sceneModels[0] != NULL) {
            model = m_sceneModels[0]->m_resMdl;
            if (model.IsValid()) {
                nw4r::g3d::ResNode node = model.GetResNode((u32)m_nodeIndex);
                if (node.IsValid()) {
                    m_scale.m_x = node.ptr()->m_scale.m_x;
                    m_scale.m_y = node.ptr()->m_scale.m_y;
                    m_scale.m_z = node.ptr()->m_scale.m_z;
                }
            }
        }
    }
}

// The spring is turned on while its scene plays; when the event of the scene turns on it is folded away (hidden, no area), when
// it turns off the spring comes back (shown, with its area).
void grGWFireSpring::updateActive(float deltaFrame) {
    switch (m_state) {
    case 0:
        setVisibility(0);
        m_startFlag = 1;
        m_eventLast = 1;
        m_state = static_cast<State>(1);
        break;
    case 1:
        if (*m_sceneIdWork == m_sceneId) {
            m_state = static_cast<State>(2);
        }
        break;
    case 2:
        if (*m_sceneIdWork != m_sceneId) {
            m_state = static_cast<State>(0);
        } else {
            u8 event = *m_eventWork;
            if (m_eventLast != event) {
                if (event == 1) {
                    setVisibility(0);
                    m_startFlag = 1;
                } else if (event == 0) {
                    setVisibility(1);
                    enableArea();
                }
            }
            m_eventLast = *m_eventWork;
        }
        break;
    }
}

// The place of the trampoline (from the stage) goes to the callback that moves its model.
void grGWFireSpring::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* data;
            Vec3f* pos = m_posWork;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = pos->m_x;
            data->m_pos.m_y = pos->m_y;
            data->m_pos.m_z = pos->m_z;
            Vec3f scale;
            gwVec3Scale(&scale, &m_scale, 0.9f);
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// The top of the spring is its place plus a little (8 scaled units) when the spring is out; it is nowhere when it is folded.
void grGWFireSpring::getTopNode(Vec3f* pos) {
    if (*m_eventWork == 1) {
        pos->m_x = 0.0f;
        pos->m_y = 0.0f;
        pos->m_z = 0.0f;
        return;
    }
    if (*m_eventWork == 0) {
        Vec3f* top = m_posWork;
        pos->m_x = top->m_x;
        float y = top->m_y;
        pos->m_y = y;
        pos->m_z = top->m_z;
        pos->m_y = y + m_scale.m_y * 0.9f * 8.0f;
    }
}

// A fighter jumps on the spring: the spring shoots and plays its sound at the place of the spring.
void grGWFireSpring::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
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
    getTopNode(&static_cast<soGimmickSpringEventArgs*>(eventInfo)->m_topPos);
    fn_27_2704CC(this);
    m_snd.playSE(static_cast<SndID>(0x1C91), 0, 0, -1);
    m_snd.setPos(m_posWork);
}
