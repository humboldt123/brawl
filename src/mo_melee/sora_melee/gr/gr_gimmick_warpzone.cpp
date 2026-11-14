// The ykAreaData members are cleared inline by the constructor (as in gr_gimmick_area_common.cpp).
#define YK_DATA_INLINE_CTOR
#include <ft/ft_manager.h>
#include <gf/gf_model.h>
#include <gr/gr_gimmick_warpzone.h>
#include <so/area/so_area_module_impl.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>
#include <types.h>

grGimmickWarpZone::grGimmickWarpZone(const char* taskName) :
    grYakumono(taskName), m_warpData(NULL), m_state(0), m_frameCount(0.0f), m_warpType(7), m_isNotAuto(false) {
    m_warpDest.m_x = 0.0f;
    m_warpDest.m_y = 0.0f;
    m_warpDest.m_z = 0.0f;
}

grGimmickWarpZone::~grGimmickWarpZone() { }

void grGimmickWarpZone::setWarpAttrData(Vec3f* warpDest, u8 warpType, bool isNotAuto) {
    m_warpDest = *warpDest;
    m_warpType = warpType;
    m_isNotAuto = isNotAuto;
}

// HYPOTHESIS: an inline helper that fills one sound-effect slot when the stage data gives a sound (the stores reload the
// slot array and copy the offset by value, as in gr_yakumono_switch.cpp).
static inline void setWarpSoundEffect(grGimmick* gimmick, u32 index, SndID soundId, Vec2f offset) {
    if (soundId > 0) {
        gimmick->m_soundEffects[index].m_id = soundId;
        gimmick->m_soundEffects[index].m_repeatFrame = 0;
        gimmick->m_soundEffects[index].m_nodeIndex = 0;
        gimmick->m_soundEffects[index].m_endFrame = 0;
        gimmick->m_soundEffects[index].m_offsetPos = offset;
    }
}

// HYPOTHESIS: an inline helper that fills one effect slot (the node is looked up by name, the offset passed by value).
static inline void setWarpEffect(grGimmick* gimmick, u32 index, int effectId, Vec2f offset, float scale) {
    gimmick->m_effects[index].m_id = effectId;
    gimmick->m_effects[index].m_repeatFrame = 0;
    gimmick->m_effects[index].m_nodeIndex = gimmick->getNodeIndex(0, "effect_locator");
    gimmick->m_effects[index].m_endFrame = 0;
    gimmick->m_effects[index].m_offsetPos = offset;
    gimmick->m_effects[index].m_scale = scale;
}

void grGimmickWarpZone::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(archive, unk1, layerType);
    m_warpData = static_cast<grGimmickWarpData*>(m_gimmickData);
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
    m_areaData.m_shapeType = soAreaInstance::Shape_Rectangle;
    m_areaData.m_group = gfArea::Stage_Group_Gimmick_Normal;
    m_areaData.m_0x4 = 0;
    m_areaData.m_0x8 = 0;
    m_areaData.m_shapeFlag.m_mask = 0;
    m_areaData.m_nodeIndex = 0;
    m_areaData.m_offsetPos.m_x = m_warpData->m_areaData.m_offsetPos.m_x;
    m_areaData.m_offsetPos.m_y = m_warpData->m_areaData.m_offsetPos.m_y;
    m_areaData.m_range.m_x = m_warpData->m_areaData.m_range.m_x;
    m_areaData.m_range.m_y = m_warpData->m_areaData.m_range.m_y;
    grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
    grGimmickMotionPathInfo motionPathInfo = { archive, &m_warpData->m_motionPathData, false, true, NULL, NULL, NULL, NULL, NULL, NULL };
    createAttachMotionPath(&motionPathInfo, NULL, "path_locator");
    stTrigger* trigger;
    if (m_isNotAuto == false) {
        // HYPOTHESIS: 0x2b is the automatic warp area (the header calls it Area_Block).
        trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Block, -1);
    } else {
        trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Warp, -1);
    }
    trigger->setObserveYakumono(m_yakumono);
    Vec3f pos(m_warpData->m_pos.m_x, m_warpData->m_pos.m_y, 0.0f);
    setPos(&pos);
    pos = Vec3f(0.0f, 0.0f, 0.0f);
    setRot(&pos);
    createIsValidTrigger(&m_warpData->m_isValidTriggerData);
    createSoundWork(2, 1);
    setWarpSoundEffect(this, 0, m_warpData->m_sndIDs[0], Vec2f(0.0f, 0.0f));
    setWarpSoundEffect(this, 1, m_warpData->m_sndIDs[1], Vec2f(0.0f, 0.0f));
    createEffectWork(1);
    setWarpEffect(this, 0, 0x103000C, Vec2f(0.0f, 0.0f), 1.0f);
}

void grGimmickWarpZone::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&m_warpData->m_isValidTriggerData, true)) {
        if (m_warpData->m_isValidTriggerData.m_isValid) {
            setValid(0);
        }
    } else {
        if (!m_warpData->m_isValidTriggerData.m_isValid) {
            setValid(0);
        }
    }
}

void grGimmickWarpZone::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
}

void grGimmickWarpZone::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    int kind = eventInfo->m_kind;
    s8 playerNo = getPlayerNumber(taskId);
    if (kind == Gimmick::Event_Boot) {
        g_stTriggerMng->setTriggerFlag(&m_warpData->m_warpTriggerData);
        if (m_warpType == 7) {
            int entryId = g_ftManager->getEntryId(playerNo);
            g_ftManager->setWarpFighter(entryId, &m_warpDest, 1.0f, 0);
            m_state = 1;
            m_frameCount = 0.0f;
            startGimmickEffect(0);
        }
    }
}
