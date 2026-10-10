#include <cm/cm_camera_controller.h>
#include <ec/ec_mgr.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/math/math_triangular.h>

#include <st_halberd/gr_halberd.h>
#include <st_halberd/gr_halberd_anim.h>

grHalberdDome* grHalberdDome::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHalberdDome* ground = new (Heaps::StageInstance) grHalberdDome(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHalberdDome::grHalberdDome(const char* taskName) : grHalberd(taskName) {
    m_mtxWork = NULL;
    m_mtxGimmickWork = NULL;
    m_posTrainerWork = NULL;
    m_motion = 1;
    m_motionTimer = 0.0f;
    m_joint[0] = NULL;
    m_joint[1] = NULL;
    m_node[0] = 0;
    m_node[1] = 0;
    m_node[2] = 0;
    m_node[3] = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
    m_seId[0] = snd_se_stage_Halberd_Doc;
    m_seId[1] = snd_se_stage_Halberd_Doc_open;
    m_seId[2] = snd_se_stage_Halberd_Doc_finish;
    m_seData[0].id = snd_se_stage_Halberd_Doc;
    m_seData[0].unk4 = 0.0f;
    m_seData[0].unk8 = 0.0f;
    m_seData[0].unkC = 0.0f;
    m_seData[1].id = snd_se_stage_Halberd_Doc_open;
    m_seData[1].unk4 = 0.0f;
    m_seData[1].unk8 = 0.0f;
    m_seData[1].unkC = 480.0f;
    m_seData[2].id = snd_se_stage_Halberd_Doc_finish;
    m_seData[2].unk4 = 0.0f;
    m_seData[2].unk8 = 480.0f;
    m_seData[2].unkC = 0.0f;
    m_seSeq.registId(m_seId, 3);
    m_seSeq.registSeq(0, m_seData, 3, Heaps::StageInstance);
    m_seSeq.m_sndGenerator = &m_snd;
}

grHalberdDome::~grHalberdDome() {
}

void grHalberdDome::processAnim() {
    Ground::processAnim();
    if (m_mtxGimmickWork != NULL) {
        getNodeMatrix(&m_mtxGimmickWork[3], 0, "DomeWarningPosition");
    }
}

void grHalberdDome::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateJoint(deltaFrame);
        updateCollision(deltaFrame);
        updateMotion(deltaFrame);
        updateCallBack(deltaFrame);
        updateG3dProcCalcWorld();
        m_hasUpdatedG3dCalcWorld = false;
        switch (*m_stateWork) {
        case 2:
            if (*m_frameWork < 720.0f && m_posTrainerWork != NULL) {
                getNodePosition(&m_posTrainerWork[0], 0, m_node[0]);
                getNodePosition(&m_posTrainerWork[1], 0, m_node[1]);
                getNodePosition(&m_posTrainerWork[2], 0, m_node[2]);
                getNodePosition(&m_posTrainerWork[3], 0, m_node[3]);
            }
            break;
        }
    }
}

// Finds the two joints of the dome floor ("D_maindome" and the gate) the first time they exist, then opens and closes the
// ledges of the dome with the frame of the animation.
void grHalberdDome::updateJoint(float deltaFrame) {
    if (m_joint[0] == NULL || m_joint[1] == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 nodeGate;
            u32 nodeDome;
            if (getNodeIndex(&nodeDome, 0, "D_maindome") && getNodeIndex(&nodeGate, 0, "D_dodai_gate01")) {
                u32 jointNum = (u16)collision->m_jointLen;
                for (u32 i = 0; i != jointNum; i++) {
                    grCollisionJoint* joint = collision->getJoint(i);
                    if (joint == NULL) {
                        break;
                    }
                    if (joint->m_ground == this && joint->_0x4E == 0) {
                        if (nodeDome == joint->m_nodeIndex) {
                            joint->m_0x56_7 = true;
                            grHalberdJointDisable(joint);
                            m_joint[0] = joint;
                        } else if (nodeGate == joint->m_nodeIndex) {
                            joint->m_0x56_7 = true;
                            grHalberdJointDisable(joint);
                            m_joint[1] = joint;
                        }
                    }
                    if (m_joint[0] != NULL && m_joint[1] != NULL) {
                        return;
                    }
                }
            }
        }
    } else if (getStageData() != NULL) {
        if (*m_stateWork == 2) {
            if (*m_frameWork <= 170.0f) {
                m_joint[0]->m_0x55_3 = false;
                m_joint[1]->m_0x55_3 = false;
            } else {
                m_joint[0]->m_0x55_3 = true;
                m_joint[1]->m_0x55_3 = true;
            }
        } else {
            m_joint[0]->m_0x55_3 = false;
            m_joint[1]->m_0x55_3 = false;
        }
    }
}

// The dome floor is solid while it is closed; while the dome opens (frame 170 - 435) the camera is lifted to the gate.
void grHalberdDome::updateCollision(float deltaFrame) {
    if (m_frameWork != NULL && m_stateWork != NULL && m_joint[0] != NULL && m_joint[1] != NULL) {
        int state = *m_stateWork;
        switch (state) {
        case 3:
            grHalberdJointDisable(m_joint[0]);
            grHalberdJointDisable(m_joint[1]);
            break;
        case 2:
            {
                float frame = *m_frameWork;
                if (frame > 720.0f) {
                    grHalberdJointDisable(m_joint[0]);
                    grHalberdJointDisable(m_joint[1]);
                } else if (frame > 435.0f) {
                    grHalberdJointEnable(m_joint[0]);
                    grHalberdJointEnable(m_joint[1]);
                } else if (frame > 170.0f) {
                    grHalberdJointEnable(m_joint[0]);
                    grHalberdJointDisable(m_joint[1]);
                    m_joint[0]->m_0x55_3 = true;
                    m_joint[1]->m_0x55_3 = true;
                } else {
                    grHalberdJointEnable(m_joint[0]);
                    grHalberdJointEnable(m_joint[1]);
                    m_joint[0]->m_0x55_3 = false;
                    m_joint[1]->m_0x55_3 = false;
                }
                // MATCH-ONLY: the camera controller keeps its current range at +0x148 and an extra lift at +0x18C; the
                // header names neither.
                if (*m_frameWork > 720.0f) {
                    CameraController* camera = CameraController::getInstance();
                    Rect2D range = *reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
                    CameraController::getInstance()->setCameraRange(&range);
                    *reinterpret_cast<float*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x18C) = 0.0f;
                } else {
                    float lift = 0.0f;
                    Vec3f gate;
                    getNodePosition(&gate, 0, "D_dodai_gate01");
                    CameraController* camera = CameraController::getInstance();
                    Rect2D range = *reinterpret_cast<Rect2D*>(reinterpret_cast<u8*>(camera) + 0x148);
                    if (range.m_down < gate.m_y) {
                        range.m_down = gate.m_y;
                        camera = CameraController::getInstance();
                        if (&camera->m_stageCameraParam != NULL) {
                            float cosine = nw4r::math::CosFIdx(10.666667f);
                            float sine = nw4r::math::SinFIdx(10.666667f);
                            range.m_down = range.m_down - camera->m_stageCameraParam.m_verticalRotationFactor * (sine / cosine) * 0.5f;
                        }
                        lift = gate.m_y - range.m_down;
                    }
                    CameraController::getInstance()->setCameraRange(&range);
                    *reinterpret_cast<float*>(reinterpret_cast<u8*>(CameraController::getInstance()) + 0x18C) = lift;
                }
            }
            break;
        case 4:
            grHalberdJointDisable(m_joint[0]);
            grHalberdJointDisable(m_joint[1]);
            break;
        }
    }
}

void grHalberdDome::updateMotion(float deltaFrame) {
    if (m_frameWork != NULL && m_stateWork != NULL) {
        m_motionTimer -= deltaFrame;
        if (m_motionTimer < 0.0f) {
            m_motionTimer = 0.0f;
        }
        switch (m_state) {
        case 2:
            if (*m_stateWork == 2) {
                setMotion(0, 0, 1, &m_motionTimer);
                setMotionFrame(*m_frameWork, 0);
                m_state = 3;
            }
            break;
        case 0:
            setMotion(1, 0, 1, NULL);
            getNodeIndex(&m_node[0], 0, "ptPosition01Dome");
            getNodeIndex(&m_node[1], 0, "ptPosition02Dome");
            getNodeIndex(&m_node[2], 0, "ptPosition03Dome");
            getNodeIndex(&m_node[3], 0, "ptPosition04Dome");
            m_state = 2;
            break;
        case 3: {
            Vec3f pos(0.0f, 0.0f, 0.0f);
            m_snd.setPos(&pos);
            m_seSeq.playFrame(0, getMotionFrame(0));
            if (m_motionTimer <= getMotionFrame(0)) {
                m_state = 4;
            }
            break;
        }
        }
    }
}

void grHalberdDome::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_mtxWork != NULL) {
                m_mtxWork->m[1][3] += 42.3864f;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
            }
        }
    }
}

// The animation of the dome (only the first one is played).
void grHalberdDome::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    if (animId != 0) {
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
