#include <ec/ec_mgr.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <memory.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdStage* grHalberdStage::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdStage* ground = new (Heaps::StageInstance) grHalberdStage(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdStage::grHalberdStage(const char* taskName) : grHalberd(taskName) {
    m_effect = 0;
    m_motion = 2;
    m_motionTimer = 0.0f;
    m_joint = NULL;
    m_seId[0] = snd_se_stage_Halberd_Shutter_open;
    m_seId[1] = snd_se_stage_Halberd_Shutter_stop;
    m_seId[2] = snd_se_stage_Halberd_Float_start;
    m_seId[3] = snd_se_stage_Halberd_Float_stop;
    m_seData0[0].id = snd_se_stage_Halberd_Shutter_open;
    m_seData0[0].unk4 = 0.0f;
    m_seData0[0].unk8 = 150.0f;
    m_seData0[0].unkC = 200.0f;
    m_seData0[1].id = snd_se_stage_Halberd_Shutter_stop;
    m_seData0[1].unk4 = 0.0f;
    m_seData0[1].unk8 = 200.0f;
    m_seData0[1].unkC = 0.0f;
    m_seData0[2].id = snd_se_stage_Halberd_Float_start;
    m_seData0[2].unk4 = 0.0f;
    m_seData0[2].unk8 = 200.0f;
    m_seData0[2].unkC = 4930.0f;
    m_seData0[3].id = snd_se_stage_Halberd_Shutter_open;
    m_seData0[3].unk4 = 0.0f;
    m_seData0[3].unk8 = 400.0f;
    m_seData0[3].unkC = 450.0f;
    m_seData0[4].id = snd_se_stage_Halberd_Shutter_stop;
    m_seData0[4].unk4 = 0.0f;
    m_seData0[4].unk8 = 450.0f;
    m_seData0[4].unkC = 0.0f;
    m_seData0[5].id = snd_se_stage_Halberd_Shutter_open;
    m_seData0[5].unk4 = 0.0f;
    m_seData0[5].unk8 = 4770.0f;
    m_seData0[5].unkC = 4925.0f;
    m_seData0[6].id = snd_se_stage_Halberd_Shutter_stop;
    m_seData0[6].unk4 = 0.0f;
    m_seData0[6].unk8 = 4925.0f;
    m_seData0[6].unkC = 0.0f;
    m_seData0[7].id = snd_se_stage_Halberd_Float_stop;
    m_seData0[7].unk4 = 0.0f;
    m_seData0[7].unk8 = 4741.0f;
    m_seData0[7].unkC = 0.0f;
    m_seData0[8].id = snd_se_stage_Halberd_Shutter_open;
    m_seData0[8].unk4 = 0.0f;
    m_seData0[8].unk8 = 5010.0f;
    m_seData0[8].unkC = 5100.0f;
    m_seData0[9].id = snd_se_stage_Halberd_Shutter_stop;
    m_seData0[9].unk4 = 0.0f;
    m_seData0[9].unk8 = 5100.0f;
    m_seData0[9].unkC = 0.0f;
    m_seData1[0].id = snd_se_stage_Halberd_Shutter_open;
    m_seData1[0].unk4 = 0.0f;
    m_seData1[0].unk8 = 150.0f;
    m_seData1[0].unkC = 200.0f;
    m_seData1[1].id = snd_se_stage_Halberd_Shutter_stop;
    m_seData1[1].unk4 = 0.0f;
    m_seData1[1].unk8 = 200.0f;
    m_seData1[1].unkC = 0.0f;
    m_seData1[2].id = snd_se_stage_Halberd_Float_start;
    m_seData1[2].unk4 = 0.0f;
    m_seData1[2].unk8 = 200.0f;
    m_seData1[2].unkC = 4930.0f;
    m_seData1[3].id = snd_se_stage_Halberd_Shutter_open;
    m_seData1[3].unk4 = 0.0f;
    m_seData1[3].unk8 = 400.0f;
    m_seData1[3].unkC = 450.0f;
    m_seData1[4].id = snd_se_stage_Halberd_Shutter_stop;
    m_seData1[4].unk4 = 0.0f;
    m_seData1[4].unk8 = 450.0f;
    m_seData1[4].unkC = 0.0f;
    m_seSeq.registId(m_seId, 4);
    m_seSeq.registSeq(0, m_seData0, 10, Heaps::StageInstance);
    m_seSeq.registSeq(1, m_seData1, 5, Heaps::StageInstance);
    m_seSeq.m_sndGenerator = &m_snd;
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_snd.setPos(&pos);
}

grHalberdStage::~grHalberdStage() {
}

void grHalberdStage::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateJoint(deltaFrame);
        updateMotion(deltaFrame);
    }
}

// Finds the joint of the deck ("PLAY_CATWARK_BIG") the first time it exists.
void grHalberdStage::updateJoint(float deltaFrame) {
    if (m_joint == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 node;
            if (getNodeIndex(&node, 0, "PLAY_CATWARK_BIG")) {
                u32 jointNum = (u16)collision->m_jointLen;
                u32 i = 0;
                grCollisionJoint* joint;
                while (true) {
                    if (i == jointNum) {
                        return;
                    }
                    joint = collision->getJoint(i);
                    if (joint == NULL) {
                        return;
                    }
                    if (joint->m_ground == this && joint->_0x4E == 0 && node == joint->m_nodeIndex) {
                        break;
                    }
                    i++;
                }
                m_joint = joint;
                joint->m_0x56_7 = true;
            }
        }
    }
}

void grHalberdStage::updateMotion(float deltaFrame) {
    if (m_frameWork == NULL) {
        return;
    }
    if (m_stateWork == NULL) {
        return;
    }
    m_motionTimer -= deltaFrame;
    if (m_motionTimer < 0.0f) {
        m_motionTimer = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(2, 0, 1, NULL);
        m_state = 2;
        // fall through
    case 2:
        if (*m_stateWork == 2) {
            setMotion(1, 0, 1, &m_motionTimer);
            setMotionFrame(*m_frameWork, 0);
            m_state = 3;
        }
        break;
    }
    if (m_motion == 1) {
        if (m_effect == 0 && getMotionFrame(0) >= 4800.0f) {
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x3F0002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "StgHalberdStage", 0);
            m_effect++;
        }
        if (m_joint != NULL) {
            if (getMotionFrame(0) >= 5050.0f) {
                grHalberdJointDisable(m_joint);
            }
            if (getMotionFrame(0) < 4900.0f) {
                m_joint->m_0x52 = 0;
            } else {
                m_joint->m_0x52 = 0x6000;
            }
        }
        m_seSeq.playFrame(0, getMotionFrame(0));
        if (*m_stateWork == 4) {
            setMotion(0, 0, 1, &m_motionTimer);
            setMotionFrame(*m_frameWork, 0);
            if (m_joint != NULL) {
                grHalberdJointEnable(m_joint);
                m_joint->m_0x52 = 0;
            }
            m_seSeq.playFrame(1, getMotionFrame(0), 0.0f);
        }
    } else if (m_motion == 0) {
        m_seSeq.playFrame(1, getMotionFrame(0));
        if (*m_stateWork == 2) {
            setMotion(1, 0, 1, &m_motionTimer);
            setMotionFrame(*m_frameWork, 0);
            m_effect = 0;
            m_seSeq.playFrame(0, getMotionFrame(0), 1121.0f);
        }
    }
}

// The animation of the deck (2 animations).
void grHalberdStage::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_motion = animId;

    if (animId >= 2) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grHalberdSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grHalberdSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grHalberdSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grHalberdSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grHalberdSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}
