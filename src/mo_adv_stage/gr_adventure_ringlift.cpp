#include <gf/gf_model.h>
#include <sora_adv_stage/gr_adventure_common.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_ringlift.h>

void grAdventureRingLift::update(float deltaFrame) {
    void (grAdventureRingLift::*frows[6])(float) = {
        &grAdventureRingLift::frowPreparation, &grAdventureRingLift::frowEntry, &grAdventureRingLift::frowStandby,
        &grAdventureRingLift::frowStart, &grAdventureRingLift::frowMoving, &grAdventureRingLift::frowStop,
    };
    (this->*frows[m_state])(deltaFrame);
    grGimmick::update(deltaFrame);
    updateCallback(0);
}

void grAdventureRingLift::frowPreparation(float deltaFrame) { }

void grAdventureRingLift::frowEntry(float deltaFrame) {
    *unk150 = static_cast<grAdventureRingLiftData*>(m_gimmickData)->m_motionSpeeds[advGetDifficultyLevel()];
    if (m_gimmickMotionPath->isMoveEnd()) {
        m_state = 2;
        m_gimmickMotionPath->changeNodeAnim(1, 0);
        m_gimmickMotionPath->applyEndFrame();
        m_gimmickMotionPath->m_frame = 0.0f;
    }
}

void grAdventureRingLift::frowStandby(float deltaFrame) { }

void grAdventureRingLift::frowStart(float deltaFrame) {
    m_gimmickMotionPath->startMove();
    changeNodeAnim(0, 0);
    float updateRate = m_modelAnims[0]->m_anmObjChrRes->GetUpdateRate();
    m_state = 4;
    m_motionRatio = m_animSpeed * updateRate;
    if (m_hasNodeAnim) {
        m_animFrame = m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
        m_modelAnims[0]->setFrame(m_animFrame);
    }
    startGimmickSE(1);
    startGimmickEffect(1);
}

void grAdventureRingLift::frowMoving(float deltaFrame) {
    if (m_gimmickMotionPath->isMoveEnd()) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
        m_state = 5;
    }
    if (m_hasNodeAnim) {
        m_animFrame -= deltaFrame * m_animSpeed;
        if (m_animFrame <= 0.0f) {
            m_animFrame = m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
        }
        m_modelAnims[0]->setFrame(m_animFrame);
    }
}

void grAdventureRingLift::updateCallback(u32 index) {
    if (m_gimmickMotionPath != NULL && (m_motionPathFlags & 1) && (m_motionPathFlags & 2)) {
        grNodeCallbackData* nodeData = m_calcWorldCallBack.m_nodeCallbackDatas;
        nodeData[1].m_rot = m_gimmickMotionPath->getRot();
    }
    grGimmick::updateCallback(0);
}

void grAdventureRingLift::notifyEventOnTrigger(int flag) {
    if (m_state == 0) {
        m_state = 1;
        m_gimmickMotionPath->setFrame(0.0f);
        m_gimmickMotionPath->startMove();
        startGimmickSE(0);
        startGimmickEffect(0);
    }
}

void grAdventureRingLift::notifyEventOffTrigger(int flag) { }
