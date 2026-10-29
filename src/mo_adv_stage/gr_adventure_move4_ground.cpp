#include <sora_adv_stage/gr_adventure_common.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_move4_ground.h>

grAdventureMove4Ground* grAdventureMove4Ground::create(int mdlIndex, const char* taskName) {
    grAdventureMove4Ground* ground = new (Heaps::StageInstance) grAdventureMove4Ground(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureMove4Ground::~grAdventureMove4Ground() { }

void grAdventureMove4Ground::setInitializeFlag() {
    if (g_stTriggerMng->getTriggerFlag(&m_data->m_validTrigger, true) == false) {
        if (m_data->m_validTrigger.m_isValid) {
            setValid(0);
        }
    } else if (!m_data->m_validTrigger.m_isValid) {
        setValid(0);
    }
    for (u8 i = 0; i < 4; i++) {
        if (g_stTriggerMng->getTriggerFlag(&m_data->m_pathTriggers[i], false) == true) {
            m_gimmickMotionPath = m_motionPaths[i];
            m_gimmickMotionPath->setFrame(m_gimmickMotionPath->m_frameCount);
            m_gimmickMotionPath->setFrameModelAnim();
        }
    }
}

void grAdventureMove4Ground::changePath(u8 index) {
    m_gimmickMotionPath = m_motionPaths[index];
    if (m_gimmickMotionPath->getFrame() == 0.0f) {
        m_gimmickMotionPath->startMove();
    } else {
        m_gimmickMotionPath->setFrameUpdate(m_data->m_motionSpeed);
    }
}

void grAdventureMove4Ground::stopPath(u8 index) {
    m_gimmickMotionPath = m_motionPaths[index];
    m_gimmickMotionPath->setFrameUpdate(0.0f);
}

void grAdventureMove4Ground::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
}

void grAdventureMove4Ground::setVisibility(u32 unk1) {
    Ground::setVisibility(unk1);
}

void grAdventureMove4Ground::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, reinterpret_cast<int*>(&sOwnerFlags))) {
        s8 playerNo = getPlayerNumber(collStatus);
        if (playerNo == 0 && isFirstContact) {
            g_stTriggerMng->setTriggerFlag(&m_data->m_landTrigger);
        }
    }
}
