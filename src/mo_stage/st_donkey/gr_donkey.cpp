#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

#include <st_donkey/gr_donkey.h>
#include <st_donkey/gr_donkey_anim.h>

// ---- base ----


grDonkey::grDonkey(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_scaleBase.m_x = 0.0f;
    m_scaleBase.m_y = 0.0f;
    m_scaleBase.m_z = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
    setupMelee();
}

grDonkey::~grDonkey() { }

void grDonkey::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

static inline bool donkeyNearZero(float v) {
    bool result = false;
    if ((float)fabs(v) < 1e-5f) {
        result = true;
    }
    return result;
}

// Reads the scale of the root node of the model once (the models are authored too big and shrunk by 0.9 in the callback).
void grDonkey::updateScaleBase(float deltaFrame) {
    bool unset = false;
    if (donkeyNearZero(m_scaleBase.m_x) && donkeyNearZero(m_scaleBase.m_y) && donkeyNearZero(m_scaleBase.m_z)) {
        unset = true;
    }
    if (unset) {
        nw4r::g3d::ResMdl model;
        if (m_sceneModels[0] != NULL) {
            model = m_sceneModels[0]->m_resMdl;
            if (model.IsValid()) {
                nw4r::g3d::ResNode node = model.GetResNode((u32)m_nodeIndex);
                if (node.IsValid()) {
                    m_scaleBase.m_x = node.ptr()->m_scale.m_x;
                    m_scaleBase.m_y = node.ptr()->m_scale.m_y;
                    m_scaleBase.m_z = node.ptr()->m_scale.m_z;
                }
            }
        }
    }
}

void grDonkey::updateCallBack(float deltaFrame) {
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
            Vec3f scale;
            donkeyVec3Scale(&scale, &m_scaleBase, 0.9f);
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// ---- main background ----

grDonkeyMainBg* grDonkeyMainBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyMainBg* ground = new (Heaps::StageInstance) grDonkeyMainBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyMainBg::grDonkeyMainBg(const char* taskName) : grDonkey(taskName) {
    m_motion = 5;
    m_posGimmickWork = NULL;
    memset(m_node, 0, sizeof(m_node));
}

grDonkeyMainBg::~grDonkeyMainBg() { }

// Publishes the positions of all the nodes to the stage.
void grDonkeyMainBg::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        getNodePosition(&m_posGimmickWork[0], 0, m_node[0]);
        getNodePosition(&m_posGimmickWork[1], 0, m_node[1]);
        getNodePosition(&m_posGimmickWork[2], 0, m_node[2]);
        getNodePosition(&m_posGimmickWork[3], 0, m_node[3]);
        getNodePosition(&m_posGimmickWork[4], 0, m_node[4]);
        getNodePosition(&m_posGimmickWork[5], 0, m_node[5]);
        getNodePosition(&m_posGimmickWork[6], 0, m_node[6]);
        getNodePosition(&m_posGimmickWork[7], 0, m_node[7]);
        getNodePosition(&m_posGimmickWork[8], 0, m_node[8]);
        getNodePosition(&m_posGimmickWork[9], 0, m_node[9]);
        getNodePosition(&m_posGimmickWork[10], 0, m_node[10]);
        getNodePosition(&m_posGimmickWork[11], 0, m_node[11]);
        getNodePosition(&m_posGimmickWork[12], 0, m_node[12]);
        getNodePosition(&m_posGimmickWork[13], 0, m_node[13]);
        getNodePosition(&m_posGimmickWork[14], 0, m_node[14]);
        getNodePosition(&m_posGimmickWork[15], 0, m_node[15]);
        getNodePosition(&m_posGimmickWork[16], 0, m_node[16]);
        getNodePosition(&m_posGimmickWork[17], 0, m_node[17]);
        getNodePosition(&m_posGimmickWork[18], 0, m_node[18]);
        getNodePosition(&m_posGimmickWork[19], 0, m_node[19]);
        getNodePosition(&m_posGimmickWork[20], 0, m_node[20]);
        getNodePosition(&m_posGimmickWork[21], 0, m_node[21]);
        getNodePosition(&m_posGimmickWork[22], 0, m_node[22]);
        getNodePosition(&m_posGimmickWork[23], 0, m_node[23]);
        getNodePosition(&m_posGimmickWork[24], 0, m_node[24]);
        getNodePosition(&m_posGimmickWork[25], 0, m_node[25]);
        getNodePosition(&m_posGimmickWork[26], 0, m_node[26]);
        getNodePosition(&m_posGimmickWork[27], 0, m_node[27]);
        getNodePosition(&m_posGimmickWork[28], 0, m_node[28]);
        getNodePosition(&m_posGimmickWork[29], 0, m_node[29]);
        getNodePosition(&m_posGimmickWork[30], 0, m_node[30]);
        getNodePosition(&m_posGimmickWork[31], 0, m_node[31]);
        getNodePosition(&m_posGimmickWork[32], 0, m_node[32]);
        getNodePosition(&m_posGimmickWork[33], 0, m_node[33]);
        getNodePosition(&m_posGimmickWork[34], 0, m_node[34]);
        getNodePosition(&m_posGimmickWork[35], 0, m_node[35]);
        getNodePosition(&m_posGimmickWork[36], 0, m_node[36]);
        getNodePosition(&m_posGimmickWork[37], 0, m_node[37]);
        getNodePosition(&m_posGimmickWork[38], 0, m_node[38]);
        getNodePosition(&m_posGimmickWork[39], 0, m_node[39]);
        getNodePosition(&m_posGimmickWork[40], 0, m_node[40]);
        getNodePosition(&m_posGimmickWork[41], 0, m_node[41]);
        getNodePosition(&m_posGimmickWork[42], 0, m_node[42]);
    }
}

// The node names: Donkey Kong, Jack, the four lift ends, the twelve score digits, the ten ladders, the two fireball tracks
// (five and seven waypoints) and the three items.
bool grDonkeyMainBg::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "Position_DonkeyKong");
    getNodeIndex(&m_node[1], 0, "Position_Jack");
    getNodeIndex(&m_node[2], 0, "Position_Elevator_L1");
    getNodeIndex(&m_node[3], 0, "Position_Elevator_L2");
    getNodeIndex(&m_node[4], 0, "Position_Elevator_R1");
    getNodeIndex(&m_node[5], 0, "Position_Elevator_R2");
    getNodeIndex(&m_node[6], 0, "Position__Score_01");
    getNodeIndex(&m_node[7], 0, "Position__Score_02");
    getNodeIndex(&m_node[8], 0, "Position__Score_03");
    getNodeIndex(&m_node[9], 0, "Position__Score_04");
    getNodeIndex(&m_node[10], 0, "Position__Score_05");
    getNodeIndex(&m_node[11], 0, "Position__Score_06");
    getNodeIndex(&m_node[12], 0, "Position__Score_07");
    getNodeIndex(&m_node[13], 0, "Position__Score_08");
    getNodeIndex(&m_node[14], 0, "Position__Score_09");
    getNodeIndex(&m_node[15], 0, "Position__Score_10");
    getNodeIndex(&m_node[16], 0, "Position__Score_11");
    getNodeIndex(&m_node[17], 0, "Position__Score_12");
    getNodeIndex(&m_node[18], 0, "hasigo_09");
    getNodeIndex(&m_node[19], 0, "hasigo_07");
    getNodeIndex(&m_node[20], 0, "hasigo_08");
    getNodeIndex(&m_node[21], 0, "hasigo_06");
    getNodeIndex(&m_node[22], 0, "hasigo_02");
    getNodeIndex(&m_node[23], 0, "hasigo_01");
    getNodeIndex(&m_node[24], 0, "hasigo_05");
    getNodeIndex(&m_node[25], 0, "hasigo_11");
    getNodeIndex(&m_node[26], 0, "hasigo_10");
    getNodeIndex(&m_node[27], 0, "hasigo_04");
    getNodeIndex(&m_node[28], 0, "Position_fireball_A_01");
    getNodeIndex(&m_node[29], 0, "Position_fireball_A_02");
    getNodeIndex(&m_node[30], 0, "Position_fireball_A_03");
    getNodeIndex(&m_node[31], 0, "Position_fireball_A_04");
    getNodeIndex(&m_node[32], 0, "Position_fireball_A_05");
    getNodeIndex(&m_node[33], 0, "Position_fireball_B_01");
    getNodeIndex(&m_node[34], 0, "Position_fireball_B_02");
    getNodeIndex(&m_node[35], 0, "Position_fireball_B_03");
    getNodeIndex(&m_node[36], 0, "Position_fireball_B_04");
    getNodeIndex(&m_node[37], 0, "Position_fireball_B_05");
    getNodeIndex(&m_node[38], 0, "Position_fireball_B_06");
    getNodeIndex(&m_node[39], 0, "Position_fireball_B_07");
    getNodeIndex(&m_node[40], 0, "Position_Parasol");
    getNodeIndex(&m_node[41], 0, "Position_Handbag");
    getNodeIndex(&m_node[42], 0, "Position_Hat");
    return result;
}

// The animation of the backdrop (5 animations).
void grDonkeyMainBg::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    if (animId >= 5) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grDonkeySetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grDonkeySetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grDonkeySetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grDonkeySetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDonkeySetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// ---- lift ----

grDonkeyLift* grDonkeyLift::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyLift* ground = new (Heaps::StageInstance) grDonkeyLift(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyLift::grDonkeyLift(const char* taskName) : grDonkey(taskName) {
    m_posWork = NULL;
    m_rate = 0.0f;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDonkeyLift::~grDonkeyLift() { }

// The lift travels between the two ends: state 0 starts it at the end it comes from, 1 moves it by the stage's lift speed
// and 2 waits a second before it appears again.
void grDonkeyLift::update(float deltaFrame) {
    if (m_isUpdate) {
        if (m_posWork != NULL) {
            float* data = static_cast<float*>(getStageData());
            if (data == NULL) {
                return;
            }
            m_timer -= deltaFrame;
            if (m_timer < 0.0f) {
                m_timer = 0.0f;
            }
            switch (m_state) {
            case 0: {
                Vec3f* ends = m_posWork;
                if (ends[0].m_y != ends[1].m_y) {
                    Vec3f dir;
                    if (m_rate > 0.0f) {
                        Vec3f diff;
                        Vec3fSub(&diff, &ends[0], &ends[1]);
                        dir = diff;
                    }
                    if (m_rate < 0.0f) {
                        ends = m_posWork;
                        Vec3f diff;
                        Vec3fSub(&diff, &ends[1], &ends[0]);
                        dir = diff;
                    }
                    float length = dir.m_z * dir.m_z + dir.m_x * dir.m_x + dir.m_y * dir.m_y;
                    if ((float)fabs(length) <= 1.17549435e-38f) {
                        length = 0.0f;
                    } else {
                        length = length * rsqrtf(length);
                    }
                    dir.normalize();
                    float scale = length * __fabsf(m_rate);
                    dir.m_x = dir.m_x * scale;
                    dir.m_y = dir.m_y * scale;
                    dir.m_z = dir.m_z * scale;
                    if (m_rate > 0.0f) {
                        ends = m_posWork;
                        m_pos.m_x = ends[1].m_x;
                        m_pos.m_y = ends[1].m_y + dir.m_y;
                        m_pos.m_z = ends[1].m_z;
                    }
                    if (m_rate < 0.0f) {
                        ends = m_posWork;
                        m_pos.m_x = ends[0].m_x;
                        m_pos.m_y = ends[0].m_y + dir.m_y;
                        m_pos.m_z = ends[0].m_z;
                    }
                    m_state = 1;
                }
                break;
            }
            case 1:
                if (m_rate > 0.0f) {
                    m_pos.m_y = m_pos.m_y + data[16] * deltaFrame;
                }
                if (m_rate < 0.0f) {
                    m_pos.m_y = m_pos.m_y - data[16] * deltaFrame;
                }
                bool arrived = false;
                if (m_rate > 0.0f && m_posWork[0].m_y < m_pos.m_y) {
                    arrived = true;
                }
                if (m_rate < 0.0f && m_pos.m_y < m_posWork[1].m_y) {
                    arrived = true;
                }
                if (arrived) {
                    setVisibility(false);
                    setEnableCollisionStatus(false);
                    if (m_rate > 0.0f) {
                        m_pos.m_y = m_posWork[1].m_y;
                    }
                    if (m_rate < 0.0f) {
                        m_pos.m_y = m_posWork[0].m_y;
                    }
                    m_state = 2;
                    m_timer = 1.0f;
                }
                break;
            case 2:
                if (m_timer == 0.0f) {
                    setVisibility(true);
                    setEnableCollisionStatus(true);
                    m_state = 1;
                }
                break;
            }
        }
        updateScaleBase(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grDonkeyLift::updateCallBack(float deltaFrame) {
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
            if (m_posWork != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = m_pos.m_x;
                data->m_pos.m_y = m_pos.m_y;
                data->m_pos.m_z = m_pos.m_z;
            }
            Vec3f scale;
            donkeyVec3Scale(&scale, &m_scaleBase, 0.9f);
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}

// ---- score digits ----

grDonkeyScore* grDonkeyScore::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDonkeyScore* ground = new (Heaps::StageInstance) grDonkeyScore(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDonkeyScore::grDonkeyScore(const char* taskName) : grDonkey(taskName) {
    m_posWork = NULL;
    m_scoreWork = NULL;
    m_lastScore = 999999;
    m_type = 8;
    m_motionRatio = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDonkeyScore::~grDonkeyScore() { }

void grDonkeyScore::update(float deltaFrame) {
    if (m_isUpdate) {
        updateScaleBase(deltaFrame);
        updateNumber();
        updateCallBack(deltaFrame);
    }
}

// Shows the digit at this decimal place of the score as the frame of the motion (frame 9 = blank).
void grDonkeyScore::updateNumber() {
    if (*m_scoreWork != m_lastScore) {
        m_lastScore = *m_scoreWork;
        u8 digit = 0;
        char buf[8];
        char text[16];
        strcpy(buf, "0");
        itoa(*m_scoreWork, text, 10);
        u32 length = strlen(text);
        switch (m_type) {
        case 7:
            if (length > 5) {
                strncpy(buf, text + length - 6, 1);
                digit = atoi(buf);
            }
            break;
        case 6:
            if (length > 4) {
                strncpy(buf, text + length - 5, 1);
                digit = atoi(buf);
            }
            break;
        case 5:
            if (length > 3) {
                strncpy(buf, text + length - 4, 1);
                digit = atoi(buf);
            }
            break;
        case 4:
            if (length > 2) {
                strncpy(buf, text + length - 3, 1);
                digit = atoi(buf);
            }
            break;
        case 3:
            if (length > 1) {
                strncpy(buf, text + length - 2, 1);
                digit = atoi(buf);
            }
            break;
        case 2:
            if (length != 0) {
                strncpy(buf, text + length - 1, 1);
                digit = atoi(buf);
            }
            break;
        }
        if (digit == 0) {
            setMotionFrame(9.0f, 0);
        } else {
            setMotionFrame(digit - 1, 0);
        }
    }
}

void grDonkeyScore::updateCallBack(float deltaFrame) {
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
            Vec3f* pos = m_posWork;
            if (pos != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = pos->m_x;
                data->m_pos.m_y = pos->m_y;
                data->m_pos.m_z = pos->m_z;
            }
            Vec3f scale;
            donkeyVec3Scale(&scale, &m_scaleBase, 0.9f);
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_scale.m_x = scale.m_x;
            data->m_scale.m_y = scale.m_y;
            data->m_scale.m_z = scale.m_z;
        }
    }
}
