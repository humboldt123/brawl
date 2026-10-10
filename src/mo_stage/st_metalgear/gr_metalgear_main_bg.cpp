#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_metalgear/gr_metalgear.h>
#include <st_metalgear/gr_metalgear_anim.h>

// MATCH-ONLY: the flag byte of a collision line (bit 0 tells that the line is solid)
struct metalgearLineView {
    u8 _pad[0x10];
    u8 m_flags;
};

grMetalgearMainBg::grMetalgearMainBg(const char* taskName) : grMetalgear(taskName) {
    m_posGimmickWork = NULL;
    m_stateWork = NULL;
    m_nodeYaneNormal = 0;
    m_nodeYaneCrash = 0;
    m_motion = 1;
    m_frameCount = 0.0f;
    m_nodeMetalgear = 0;
    m_nodeGekko = 0;
    m_nodeSLightA = 0;
    m_nodeSLightB = 0;
    m_jointLeft = NULL;
    m_jointRight = NULL;
    m_stateWallWork = NULL;
}

grMetalgearMainBg* grMetalgearMainBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMetalgearMainBg* ground = new (Heaps::StageInstance) grMetalgearMainBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMetalgearMainBg::~grMetalgearMainBg() {
}

// The places of the locators in the stage model are given to the stage.
void grMetalgearMainBg::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        getNodePosition(&m_posGimmickWork[0], 0, m_nodeMetalgear);
        getNodePosition(&m_posGimmickWork[1], 0, m_nodeGekko);
        getNodePosition(&m_posGimmickWork[4], 0, m_nodeSLightB);
        getNodePosition(&m_posGimmickWork[5], 0, m_nodeSLightA);
    }
}

void grMetalgearMainBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateJoint(deltaFrame);
        updateBreak(deltaFrame);
        updateColl(deltaFrame);
    }
}

// Finds the collision joints of the two walls of the stage model.
void grMetalgearMainBg::updateJoint(float deltaFrame) {
    if (m_jointLeft == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 nodeLeft;
            u32 nodeRight;
            getNodeIndex(&nodeLeft, 0, "B_MAINSTAGE_KAGE");
            getNodeIndex(&nodeRight, 0, "B_DOOR_LIGHT_01");
            u16 count = collision->m_jointLen;
            for (u32 i = 0; i != count; i++) {
                grCollisionJoint* joint = collision->getJoint(i);
                if (joint != NULL && joint->m_ground == this) {
                    if (nodeLeft == joint->m_nodeIndex) {
                        m_jointLeft = joint;
                    } else if (nodeRight == joint->m_nodeIndex) {
                        m_jointRight = joint;
                    }
                }
                if (m_jointLeft != NULL && m_jointRight != NULL) {
                    return;
                }
            }
        }
    }
}

// The roof breaks when the stage says so (state 0): the whole roof is replaced by the broken one and pieces fly away.
void grMetalgearMainBg::updateBreak(float deltaFrame) {
    switch (m_state) {
    case 1:
        if (*m_stateWork == 0) {
            setMotion(0, false, true, &m_frameCount);
            setNodeVisibility(false, 0, m_nodeYaneNormal, true, false);
            setNodeVisibility(true, 0, m_nodeYaneCrash, true, false);
            fn_27_224DB8(snd_se_stage_Metalgear_10, 0.0f);
            u32 handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_tenzyo_crash);
            g_ecMgr->setParent(handle, m_sceneModels[0], "StgMetalgear00_base", false);
            handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_hahenkemuri);
            g_ecMgr->setParent(handle, m_sceneModels[0], "Yane_piece10", false);
            handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_hahenkemuri);
            g_ecMgr->setParent(handle, m_sceneModels[0], "Yane_piece15", false);
            handle = g_ecMgr->setEffect(ef_ptc_stg_metalgear_hahenkemuri);
            g_ecMgr->setParent(handle, m_sceneModels[0], "Yane_piece18", false);
            m_state = 2;
        } else {
            setMotionFrame(0.0f, 0);
        }
        break;
    case 0:
        setMotion(0, false, true, &m_frameCount);
        setNodeVisibility(true, 0, m_nodeYaneNormal, true, false);
        setNodeVisibility(false, 0, m_nodeYaneCrash, true, false);
        m_state = 1;
        break;
    case 2:
        if (getMotionFrame(0) >= m_frameCount) {
            setMotion(1, false, true, NULL);
            setNodeVisibility(false, 0, m_nodeYaneNormal, true, false);
            setNodeVisibility(true, 0, m_nodeYaneCrash, true, false);
            setNodeVisibility(false, 0, "YANE_brokenPiece", true, false);
            m_state = 3;
        }
        break;
    }
}

// The collisions of the walls stand while the walls do.
void grMetalgearMainBg::updateColl(float deltaFrame) {
    grCollisionJoint* joint = m_jointLeft;
    if (joint != NULL) {
        if (m_stateWallWork[1] == 6) {
            metalgearLineView* line = reinterpret_cast<metalgearLineView*>(joint->getLine(0));
            line->m_flags = line->m_flags & ~1;
        } else {
            metalgearLineView* line = reinterpret_cast<metalgearLineView*>(joint->getLine(0));
            line->m_flags = line->m_flags | 1;
        }
        if (m_stateWallWork[3] == 6) {
            metalgearLineView* line = reinterpret_cast<metalgearLineView*>(m_jointRight->getLine(0));
            line->m_flags = line->m_flags & ~1;
        } else {
            metalgearLineView* line = reinterpret_cast<metalgearLineView*>(m_jointRight->getLine(0));
            line->m_flags = line->m_flags | 1;
        }
    }
}

bool grMetalgearMainBg::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeYaneNormal, 0, "B_STAGE_YANE1_NOMAL");
    getNodeIndex(&m_nodeYaneCrash, 0, "B_STAGE_YANE1_CRASH");
    getNodeIndex(&m_nodeMetalgear, 0, "Metalgear_locator");
    getNodeIndex(&m_nodeGekko, 0, "GekkoKabe_locator");
    getNodeIndex(&m_nodeSLightA, 0, "A_SLightBeam_locator");
    getNodeIndex(&m_nodeSLightB, 0, "B_SLightBeam_locator");
    return result;
}

// The animation of the roof (only motion 0 exists; it has the bone animation and the visibility).
void grMetalgearMainBg::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    m_motion = animId;

    if (animId != 0) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grMetalgearSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grMetalgearSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->m_anmObjChrRes->SetFrame(0.0f);
    modelAnim->m_anmObjVisRes->SetFrame(0.0f);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoopNode(loop);
    modelAnim->setLoopVisible(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
    }
}

float grMetalgearMainBg::getMotionFrame(u32 index) {
    gfModelAnimation* modelAnim = m_modelAnims[index];
    if (modelAnim == NULL) {
        return 0.0f;
    }
    return modelAnim->m_anmObjChrRes->GetFrame();
}
