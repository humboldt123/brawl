#include <gf/gf_model.h>
#include <gr/gr_yakumono_switch.h>
#include <sora_adv_stage/gr_adventure_common.h>
#include <st/st_trigger.h>

grYakumonoSwitch::grYakumonoSwitch(bool* isOn, const char* taskName) : grYakumono(taskName), m_isOn(isOn), m_state(0), m_158(0.0f) { }

grYakumonoSwitch::~grYakumonoSwitch() { }

// HYPOTHESIS: an inline helper that fills one sound-effect slot (the stores reload the slot array and copy the offset by value).
static inline void setSwitchSoundEffect(grGimmick* gimmick, u32 index, SndID soundId, Vec2f offset) {
    gimmick->m_soundEffects[index].m_id = soundId;
    gimmick->m_soundEffects[index].m_repeatFrame = 0;
    gimmick->m_soundEffects[index].m_nodeIndex = 0;
    gimmick->m_soundEffects[index].m_endFrame = 0;
    gimmick->m_soundEffects[index].m_offsetPos = offset;
}

void grYakumonoSwitch::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    m_switchData = static_cast<grYakumonoSwitchData*>(getGimmickData());
    m_switchData->m_motionRatio = m_switchData->m_difficultyMotionRatios[advGetDifficultyLevel()];
    grGimmickMotionPathInfo motionPathInfo = { data, &m_switchData->m_motionPathData, true, true, NULL, NULL, NULL, NULL, NULL, NULL };
    createAttachMotionPath(&motionPathInfo, &m_switchData->m_motionPathTriggerData, "path_locator");
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
    Vec3f pos(m_switchData->m_pos.m_x, m_switchData->m_pos.m_y, 0.0f);
    Vec3f rot(0.0f, 0.0f, m_switchData->m_rotZ);
    setPos(&pos);
    setRot(&rot);
    setSimpleEffectVisibleProduction();
    createIsValidTrigger(&m_switchData->m_isValidTriggerData);
    createSoundWork(2, 1);
    setSwitchSoundEffect(this, 0, (SndID)0x225b, Vec2f(0.0f, 0.0f));
    setSwitchSoundEffect(this, 1, (SndID)0x225c, Vec2f(0.0f, 0.0f));
    createEffectWork(1);
}

void grYakumonoSwitch::setSimpleEffectVisibleProduction() {
    static u32 visibleProductionIndex = 4;
    SimpleEffectData simpleEffectData;
    createSimpleEffectData(&simpleEffectData, 0x1030023, "effect_locator");
    createEffectVisibleProductionForExcel(&simpleEffectData, &visibleProductionIndex, m_visibleProductions);
}

void grYakumonoSwitch::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&m_switchData->m_isValidTriggerData, true)) {
        if (m_switchData->m_isValidTriggerData.m_isValid) {
            setValid(0);
        }
    } else {
        if (!m_switchData->m_isValidTriggerData.m_isValid) {
            setValid(0);
        }
    }
    bool hasChrAnim = m_modelAnims[0]->m_resFile.GetResAnmChrNumEntries() > 0;
    if (hasChrAnim) {
        if (*m_isOn == true) {
            setOn(false);
        }
    }
}

void grYakumonoSwitch::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    judgeTriggerState();
    grGimmick::updateCallback(0);
}

void grYakumonoSwitch::judgeTriggerState() {
    if (*m_isOn == true) {
        if (m_state == 0) {
            setOn(true);
        }
    } else {
        if (m_state == 1) {
            setOff(true);
        }
    }
}

void grYakumonoSwitch::setOn(int withEffect) {
    *m_isOn = true;
    bool hasChrAnim = m_modelAnims[0]->m_resFile.GetResAnmChrNumEntries() > 0;
    if (hasChrAnim == true) {
        grGimmick::changeNodeAnim(0, 0);
        if (withEffect == 0) {
            gfModelAnimation* modelAnim = m_modelAnims[0];
            modelAnim->setFrame(modelAnim->getFrameCount());
        }
    }
    m_state = 1;
    m_158 = 0.0f;
    if (withEffect) {
        startGimmickSE(0);
        startGimmickEffect(0);
    }
}

void grYakumonoSwitch::setOff(int withEffect) {
    *m_isOn = false;
    bool hasChrAnim = m_modelAnims[0]->m_resFile.GetResAnmChrNumEntries() > 1;
    if (hasChrAnim == true && m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    m_state = 0;
    m_158 = 0.0f;
    m_switchData->m_motionRatio = m_switchData->m_motionRatio * m_switchData->m_offMotionRatioScale;
    if (withEffect) {
        startGimmickSE(1);
    }
}
