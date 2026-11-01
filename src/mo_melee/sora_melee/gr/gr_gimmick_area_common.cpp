#include <gr/gr_gimmick_area_common.h>
#include <gf/gf_memory_util.h>
#include <st/st_trigger.h>
#include <types.h>

grGimmickAreaCommon::grGimmickAreaCommon(const char* taskName) : grYakumono(taskName) { }

grGimmickAreaCommon::~grGimmickAreaCommon() { }

void grGimmickAreaCommon::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
}

void grGimmickAreaCommon::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(archive, unk1, layerType);
    m_gimmickAreaData = static_cast<stGimmickAreaData*>(getGimmickData());
    Ground::updateG3dProcCalcWorld();
    Vec3f pos;
    Ground::getNodePosition(&pos, 0, u32(0));
    setPos(&pos);
    gfMemFill(&m_ykData, 0, sizeof(m_ykData));
    m_areaData.m_shapeType = soAreaInstance::Shape_Rectangle;
    m_areaData.m_group = gfArea::Stage_Group_Gimmick_Normal;
    m_areaData.m_0x4 = 0;
    m_areaData.m_0x8 = 0;
    m_areaData.m_shapeFlag.m_mask = soAreaData::SHAPE_MASK_FOLLOW_NODE;
    m_areaData.m_nodeIndex = 0;
    m_areaData.m_offsetPos.m_x = m_gimmickAreaData->m_offsetPos.m_x;
    m_areaData.m_offsetPos.m_y = m_gimmickAreaData->m_offsetPos.m_y;
    m_areaData.m_range.m_x = m_gimmickAreaData->m_range.m_x;
    m_areaData.m_range.m_y = m_gimmickAreaData->m_range.m_y;
    grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Common, -1);
    trigger->setObserveYakumono(m_yakumono);
}

void grGimmickAreaCommon::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) { }
