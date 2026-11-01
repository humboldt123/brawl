#include <gm/gm_global.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_savepoint.h>
#include <st/st_trigger.h>

// HYPOTHESIS: the save request byte of the adventure data (gmAdvData is only partly described; 7 asks for a save).
static inline u8& advSaveRequest() {
    return reinterpret_cast<u8*>(g_GameGlobal->m_advData)[0x603];
}

grAdventureSavePoint* grAdventureSavePoint::create(int mdlIndex, const char* taskName) {
    grAdventureSavePoint* ground = new (Heaps::StageInstance) grAdventureSavePoint(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureSavePoint::~grAdventureSavePoint() { }

void grAdventureSavePoint::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    m_data = static_cast<grAdventureSavePointData*>(m_gimmickData);
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
    m_areaData.m_shapeType = soAreaInstance::Shape_Rectangle;
    m_areaData.m_group = gfArea::Stage_Group_Gimmick_Normal;
    m_areaData.m_0x4 = 0;
    m_areaData.m_0x8 = 0;
    m_areaData.m_shapeFlag.m_mask = 0;
    m_areaData.m_nodeIndex = 0;
    m_areaData.m_offsetPos.m_x = m_data->m_areaOffset.m_x;
    m_areaData.m_offsetPos.m_y = m_data->m_areaOffset.m_y;
    m_areaData.m_range.m_x = m_data->m_areaRange.m_x;
    m_areaData.m_range.m_y = m_data->m_areaRange.m_y;
    grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
    stTrigger* trigger = g_stTriggerMng->createTrigger(Gimmick::Area_SavePoint, -1);
    trigger->setObserveYakumono(m_yakumono);
    Vec3f pos(m_data->m_pos.m_x, m_data->m_pos.m_y, 0.0f);
    setPos(&pos);
}

void grAdventureSavePoint::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
    if (m_wasActivated == 0 && m_isActivated == 1) {
        advSaveRequest() = 7;
        m_timer = 60.0f;
    }
    m_wasActivated = m_isActivated;
    if (advSaveRequest() == 0) {
        if (m_timer > 0.0f) {
            m_timer -= 1.0f;
        } else {
            m_isActivated = 0;
        }
    }
}

void grAdventureSavePoint::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    if (advSaveRequest() == 0 && getPlayerNumber(taskId) == 0) {
        m_isActivated = 1;
    }
}
