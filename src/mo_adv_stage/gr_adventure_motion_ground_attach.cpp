// MATCH-ONLY: the attach offset is copy-constructed with the aggregate word copy of a plain-base Vec2f.
#define MT_VEC2F_PLAIN_BASE
#define MT_VEC2F_IMPLICIT_ASSIGN
#include <gf/gf_model.h>
#include <sora_adv_stage/gr_adventure_common.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_motion_ground_attach.h>
#include <st/st_trigger.h>

grAdventureMotionGroundAttach* grAdventureMotionGroundAttach::create(int mdlIndex, const char* taskName) {
    grAdventureMotionGroundAttach* ground = new (Heaps::StageInstance) grAdventureMotionGroundAttach(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

void grAdventureMotionGroundAttach::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureMotionGround::startup(data, unk1, layerType);
    bool hasTexSrt = m_modelAnims[0]->m_resFile.GetResAnmTexSrtNumEntries() > 0;
    if (hasTexSrt) {
        changeTexSrtAnim(0, 0);
    }
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    m_attachData = static_cast<grGimmickMotionGroundAttachData*>(m_gimmickData);
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
    Vec3f pos = m_attachData->m_pos;
    setPos(&pos);
    Vec3f rot = m_attachData->m_rot;
    setRot(&rot);
    updateCallback(0);
    updateG3dProcCalcWorld();
}

void grAdventureMotionGroundAttach::update(float deltaFrame) {
    grAdventureMotionGround::update(deltaFrame);
    updateSEandEffect();
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
}

void grAdventureMotionGroundAttach::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&m_attachData->m_motionGroundData.m_isValidTrigger, true)) {
        if (m_attachData->m_motionGroundData.m_isValidTrigger.m_isValid) {
            setValid(0);
        }
    } else {
        if (!m_attachData->m_motionGroundData.m_isValidTrigger.m_isValid) {
            setValid(0);
        }
    }
    if (g_stTriggerMng->getTriggerFlag(&m_attachData->m_motionGroundData.m_motionTrigger, false) == true) {
        changeNodeAnim(0, 0);
        m_modelAnims[0]->m_anmObjChrRes->SetFrame(m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength);
    }
}

void grAdventureMotionGroundAttach::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, reinterpret_cast<int*>(&sOwnerFlags)) && isFirstContact) {
        g_stTriggerMng->setTriggerFlag(&m_attachData->m_motionGroundData.m_landingTrigger);
    }
}

grAdventureMotionGroundAttach::~grAdventureMotionGroundAttach() { }
