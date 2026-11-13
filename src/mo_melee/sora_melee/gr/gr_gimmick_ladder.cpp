// The ykAreaData members are cleared inline by the constructor (as in gr_gimmick_area_common.cpp).
#define YK_DATA_INLINE_CTOR
#include <gf/gf_model.h>
#include <gr/gr_gimmick_ladder.h>
#include <so/area/so_area_module_impl.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <types.h>

grGimmickLadder* grGimmickLadder::create(s16 mdlIndex, const char* taskName) {
    grGimmickLadder* ladder = new (Heaps::StageInstance) grGimmickLadder(taskName);
    if (ladder != NULL) {
        ladder->setMdlIndex(mdlIndex);
    }
    return ladder;
}

grGimmickLadder::grGimmickLadder(const char* taskName) :
    grYakumono(taskName), m_ladderData(NULL), m_upperOffsetPos(0.0f, 0.0f, 0.0f), m_underOffsetPos(0.0f, 0.0f, 0.0f) {
    m_transparencySettings = 0;
}

grGimmickLadder::~grGimmickLadder() { }

void grGimmickLadder::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(archive, unk1, layerType);
    m_ladderData = static_cast<grGimmickLadderData*>(getGimmickData());
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(1);
    grGimmickMotionPathInfo motionPathInfo = { archive, &m_ladderData->m_motionPathData, false, true, NULL, NULL, NULL, NULL, NULL, NULL };
    createAttachMotionPath(&motionPathInfo, &m_ladderData->m_motionPathTriggerData, "path_locator");
    Ground::updateG3dProcCalcWorld();
    Vec3f pos;
    Ground::getNodePosition(&pos, 0, u32(0));
    setPos(&pos);
    m_areaData.m_shapeType = soAreaInstance::Shape_Rectangle;
    m_areaData.m_group = gfArea::Stage_Group_Gimmick_Ladder;
    m_areaData.m_0x4 = 0;
    m_areaData.m_0x8 = 0;
    m_areaData.m_shapeFlag.m_mask = 0;
    m_areaData.m_nodeIndex = 0;
    m_areaData.m_offsetPos.m_x = m_ladderData->m_areaData.m_offsetPos.m_x;
    m_areaData.m_offsetPos.m_y = m_ladderData->m_areaData.m_offsetPos.m_y;
    m_areaData.m_range.m_x = m_ladderData->m_areaData.m_range.m_x;
    m_areaData.m_range.m_y = m_ladderData->m_areaData.m_range.m_y;
    grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Ladder, -1);
    trigger->setObserveYakumono(m_yakumono);
    createIsValidTrigger(&m_ladderData->m_isValidTriggerData);
}

void grGimmickLadder::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&m_ladderData->m_isValidTriggerData, true)) {
        if (m_ladderData->m_isValidTriggerData.m_isValid) {
            setValid(0);
        }
    } else {
        if (!m_ladderData->m_isValidTriggerData.m_isValid) {
            setValid(0);
        }
    }
}

void grGimmickLadder::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
}

void grGimmickLadder::getUpperNode(Vec3f* pos) {
    getNodePosition(pos, 0, "upper");
    Vec3fAdd(pos, pos, &m_upperOffsetPos);
}

void grGimmickLadder::getUnderNode(Vec3f* pos) {
    getNodePosition(pos, 0, "under");
    Vec3fAdd(pos, pos, &m_underOffsetPos);
}

bool grGimmickLadder::disableArea() {
    bool result = grYakumono::disableArea();
    soGimmickEventArgs_Disable args;
    m_yakumono->presentEventGimmick(&args, -1);
    return result;
}

bool grGimmickLadder::setSleepArea(bool unk1) {
    bool result = grYakumono::setSleepArea(unk1);
    if (unk1 == 1) {
        soGimmickEventArgs_Disable args;
        m_yakumono->presentEventGimmick(&args, -1);
    }
    return result;
}

void grGimmickLadder::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    soGimmickLadderEventArgs* args = static_cast<soGimmickLadderEventArgs*>(eventInfo);
    switch (eventInfo->m_kind) {
    case Gimmick::Ladder_Event_Get_Info:
        getUpperNode(&args->m_upperNodePos);
        getUnderNode(&args->m_underNodePos);
        args->m_restrictUpExit = m_ladderData->m_restrictUpExit;
        args->m_33 = m_ladderData->m_51;
        break;
    }
}
