#include <ec/ec_mgr.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <memory.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdBg* grHalberdBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdBg* ground = new (Heaps::StageInstance) grHalberdBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdBg::grHalberdBg(const char* taskName) : grHalberd(taskName) {
    m_effect = 0;
    m_mtxGimmickWork = NULL;
    m_posTrainerWork = NULL;
    m_motion = 2;
    m_motionTimer = 0.0f;
    m_joint[0] = NULL;
    m_joint[1] = NULL;
    m_nodeTrainer[0] = 0;
    m_nodeTrainer[1] = 0;
    m_nodeTrainer[2] = 0;
    m_nodeTrainer[3] = 0;
    m_node[0] = 0;
    m_node[1] = 0;
    m_node[2] = 0;
    m_node[3] = 0;
    m_seId[0] = snd_se_stage_Halberd_01;
    m_seId[1] = snd_se_stage_Halberd_02;
    m_seId[2] = snd_se_stage_Halberd_03;
    m_seId[3] = snd_se_stage_Halberd_04;
    m_seId[4] = snd_se_stage_Halberd_05;
    m_seId[5] = snd_se_stage_Halberd_06;
    m_seId[6] = snd_se_stage_Halberd_07;
    m_seId[7] = snd_se_stage_Halberd_08;
    m_seId[8] = snd_se_stage_Halberd_09;
    m_seId[9] = snd_se_stage_Halberd_10;
    m_seId[10] = snd_se_stage_Halberd_wing;
    m_seData0[0].id = snd_se_stage_Halberd_02;
    m_seData0[0].unk4 = 0.0f;
    m_seData0[0].unk8 = 0.0f;
    m_seData0[0].unkC = 0.0f;
    m_seData0[1].id = snd_se_stage_Halberd_03;
    m_seData0[1].unk4 = 0.0f;
    m_seData0[1].unk8 = 468.0f;
    m_seData0[1].unkC = 0.0f;
    m_seData0[2].id = snd_se_stage_Halberd_10;
    m_seData0[2].unk4 = 0.0f;
    m_seData0[2].unk8 = 600.0f;
    m_seData0[2].unkC = 4930.0f;
    m_seData0[3].id = snd_se_stage_Halberd_01;
    m_seData0[3].unk4 = 0.0f;
    m_seData0[3].unk8 = 615.0f;
    m_seData0[3].unkC = 0.0f;
    m_seData0[4].id = snd_se_stage_Halberd_wing;
    m_seData0[4].unk4 = 0.0f;
    m_seData0[4].unk8 = 850.0f;
    m_seData0[4].unkC = 0.0f;
    m_seData0[5].id = snd_se_stage_Halberd_03;
    m_seData0[5].unk4 = 0.0f;
    m_seData0[5].unk8 = 933.0f;
    m_seData0[5].unkC = 0.0f;
    m_seData0[6].id = snd_se_stage_Halberd_04;
    m_seData0[6].unk4 = 0.0f;
    m_seData0[6].unk8 = 1080.0f;
    m_seData0[6].unkC = 0.0f;
    m_seData0[7].id = snd_se_stage_Halberd_06;
    m_seData0[7].unk4 = 0.0f;
    m_seData0[7].unk8 = 1225.0f;
    m_seData0[7].unkC = 0.0f;
    m_seData0[8].id = snd_se_stage_Halberd_05;
    m_seData0[8].unk4 = 0.0f;
    m_seData0[8].unk8 = 1210.0f;
    m_seData0[8].unkC = 0.0f;
    m_seData0[9].id = snd_se_stage_Halberd_07;
    m_seData0[9].unk4 = 0.0f;
    m_seData0[9].unk8 = 1915.0f;
    m_seData0[9].unkC = 0.0f;
    m_seData0[10].id = snd_se_stage_Halberd_03;
    m_seData0[10].unk4 = 0.0f;
    m_seData0[10].unk8 = 2193.0f;
    m_seData0[10].unkC = 0.0f;
    m_seData0[11].id = snd_se_stage_Halberd_04;
    m_seData0[11].unk4 = 0.0f;
    m_seData0[11].unk8 = 2340.0f;
    m_seData0[11].unkC = 0.0f;
    m_seData0[12].id = snd_se_stage_Halberd_08;
    m_seData0[12].unk4 = 0.0f;
    m_seData0[12].unk8 = 2445.0f;
    m_seData0[12].unkC = 0.0f;
    m_seData0[13].id = snd_se_stage_Halberd_05;
    m_seData0[13].unk4 = 0.0f;
    m_seData0[13].unk8 = 2460.0f;
    m_seData0[13].unkC = 0.0f;
    m_seData0[14].id = snd_se_stage_Halberd_09;
    m_seData0[14].unk4 = 0.0f;
    m_seData0[14].unk8 = 2840.0f;
    m_seData0[14].unkC = 0.0f;
    m_seData0[15].id = snd_se_stage_Halberd_07;
    m_seData0[15].unk4 = 0.0f;
    m_seData0[15].unk8 = 3750.0f;
    m_seData0[15].unkC = 0.0f;
    m_seData1[0].id = snd_se_stage_Halberd_10;
    m_seData1[0].unk4 = 0.0f;
    m_seData1[0].unk8 = 200.0f;
    m_seData1[0].unkC = 1100.0f;
    m_seData1[1].id = snd_se_stage_Halberd_03;
    m_seData1[1].unk4 = 0.0f;
    m_seData1[1].unk8 = 1033.0f;
    m_seData1[1].unkC = 0.0f;
    m_seData1[2].id = snd_se_stage_Halberd_04;
    m_seData1[2].unk4 = 0.0f;
    m_seData1[2].unk8 = 1180.0f;
    m_seData1[2].unkC = 0.0f;
    m_seSeq.registId(m_seId, 11);
    m_seSeq.registSeq(0, m_seData0, 16, Heaps::StageInstance);
    m_seSeq.registSeq(1, m_seData1, 3, Heaps::StageInstance);
    m_seSeq.m_sndGenerator = &m_snd;
}

grHalberdBg::~grHalberdBg() {
}

void grHalberdBg::processAnim() {
}

void grHalberdBg::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    m_node[0] = getNodeIndex(0, "EnkeiPosition");
    m_node[1] = getNodeIndex(0, "DomePosition");
    m_node[2] = getNodeIndex(0, "HalWarningPosition");
    m_node[3] = getNodeIndex(0, "HL2renPosition");
}

void grHalberdBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateJoint(deltaFrame);
    updateCollision(deltaFrame);
    updateMotion(deltaFrame);
    updateG3dProcCalcWorld();
    m_hasUpdatedG3dCalcWorld = false;
    if (m_mtxGimmickWork != NULL) {
        getNodeMatrix(&m_mtxGimmickWork[0], 0, m_node[0]);
        getNodeMatrix(&m_mtxGimmickWork[1], 0, m_node[1]);
        getNodeMatrix(&m_mtxGimmickWork[2], 0, m_node[2]);
        getNodeMatrix(&m_mtxGimmickWork[4], 0, m_node[3]);
    }
    if (m_posTrainerWork != NULL) {
        switch (m_motion) {
        case 0:
            if (!(getMotionFrame(0) < 1121.0f)) {
                getNodePosition(&m_posTrainerWork[0], 0, m_nodeTrainer[0]);
                getNodePosition(&m_posTrainerWork[1], 0, m_nodeTrainer[1]);
                getNodePosition(&m_posTrainerWork[2], 0, m_nodeTrainer[2]);
                getNodePosition(&m_posTrainerWork[3], 0, m_nodeTrainer[3]);
            }
            break;
        case 1:
            getNodePosition(&m_posTrainerWork[0], 0, m_nodeTrainer[0]);
            getNodePosition(&m_posTrainerWork[1], 0, m_nodeTrainer[1]);
            getNodePosition(&m_posTrainerWork[2], 0, m_nodeTrainer[2]);
            getNodePosition(&m_posTrainerWork[3], 0, m_nodeTrainer[3]);
            break;
        }
    }
}

// Finds the two joints of the deck (the hull and the gate) the first time they exist and puts them to sleep.
void grHalberdBg::updateJoint(float deltaFrame) {
    if (m_joint[0] == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 nodeGate;
            u32 nodeHull;
            if (getNodeIndex(&nodeHull, 0, "A__HL_MAIN_HULL02") && getNodeIndex(&nodeGate, 0, "HL_Gate_Col")) {
                u32 jointNum = (u16)collision->m_jointLen;
                for (u32 i = 0; i != jointNum; i++) {
                    grCollisionJoint* joint = collision->getJoint(i);
                    if (joint == NULL) {
                        break;
                    }
                    if (joint->m_ground == this && joint->_0x4E == 0) {
                        if (nodeHull == joint->m_nodeIndex) {
                            joint->m_0x56_7 = true;
                            grHalberdJointDisable(joint);
                            m_joint[0] = joint;
                        } else if (nodeGate == joint->m_nodeIndex) {
                            joint->m_0x56_7 = true;
                            grHalberdJointDisable(joint);
                            m_joint[1] = joint;
                        }
                    }
                }
            }
        }
    }
}

// The joints of the deck are solid or not depending on the part of the story and the frame of the animation.
void grHalberdBg::updateCollision(float deltaFrame) {
    float* frame = m_frameWork;
    if (frame == NULL) {
        return;
    }
    if (m_stateWork == NULL) {
        return;
    }
    grCollisionJoint* hull = m_joint[0];
    if (hull == NULL) {
        return;
    }
    if (m_joint[1] == NULL) {
        return;
    }
    int state = *m_stateWork;
    switch (state) {
    case 2:
        if (*frame >= 4710.0f) {
            grHalberdJointEnable(hull);
            grHalberdJointEnable(m_joint[1]);
            m_joint[0]->m_0x55_3 = false;
            return;
        }
        grHalberdJointDisable(hull);
        grHalberdJointDisable(m_joint[1]);
        return;
    case 3:
        grHalberdJointEnable(hull);
        grHalberdJointEnable(m_joint[1]);
        return;
    case 4:
        if (*frame >= 410.0f) {
            grHalberdJointDisable(hull);
            grHalberdJointDisable(m_joint[1]);
            return;
        }
        if (*frame >= 15.0f) {
            grHalberdJointEnable(hull);
            grHalberdJointDisable(m_joint[1]);
            m_joint[0]->m_0x55_3 = true;
            return;
        }
        grHalberdJointEnable(hull);
        grHalberdJointEnable(m_joint[1]);
        return;
    }
}

void grHalberdBg::updateMotion(float deltaFrame) {
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
        getNodeIndex(&m_nodeTrainer[0], 0, "ptPosition01HB");
        getNodeIndex(&m_nodeTrainer[1], 0, "ptPosition02HB");
        getNodeIndex(&m_nodeTrainer[2], 0, "ptPosition03HB");
        getNodeIndex(&m_nodeTrainer[3], 0, "ptPosition04HB");
        m_state = 2;
        // fall through
    case 2:
        if (*m_stateWork == 2) {
            setMotion(0, 0, 1, &m_motionTimer);
            setMotionFrame(*m_frameWork, 0);
            m_state = 3;
        }
        break;
    }
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_snd.setPos(&pos);
    if (m_motion == 1) {
        m_seSeq.playFrame(1, getMotionFrame(0));
        if (*m_stateWork == 2) {
            setMotion(0, 0, 1, &m_motionTimer);
            setMotionFrame(*m_frameWork, 0);
            m_seSeq.playFrame(0, getMotionFrame(0), 1121.0f);
        }
    } else if (m_motion == 0) {
        if (m_effect == 0 && getMotionFrame(0) >= 600.0f) {
            g_ecMgr->setDrawPrio(1);
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x3F0001));
            g_ecMgr->setDrawPrio(-1);
            g_ecMgr->setParent(effect, m_sceneModels[0], "chikeiHal", 0);
            m_effect++;
        }
        m_seSeq.playFrame(0, getMotionFrame(0));
        if (*m_stateWork == 4) {
            setMotion(1, 0, 1, &m_motionTimer);
            setMotionFrame(*m_frameWork, 0);
            m_seSeq.playFrame(1, getMotionFrame(0), 0.0f);
        }
    }
}

// The animation of the hull (2 animations).
void grHalberdBg::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grHalberdSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grHalberdSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grHalberdSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
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
