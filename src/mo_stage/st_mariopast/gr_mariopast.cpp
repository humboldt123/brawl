#include <ec/ec_mgr.h>
#include <gm/gm_global.h>
#include <gm/gm_stage_data.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <string.h>

#include <st_mariopast/gr_mariopast.h>

grMarioPast::grMarioPast(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    unk154 = 0.0f;
    setupMelee();
}

grMarioPast::~grMarioPast() {
}

// ---- Bg ----

grMarioPastBg* grMarioPastBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMarioPastBg* ground = new (Heaps::StageInstance) grMarioPastBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMarioPastBg::grMarioPastBg(const char* taskName) : grMarioPast(taskName) {
    m_frameLocator = NULL;
    m_posGimmick = NULL;
    memset(m_node, 0, sizeof(m_node));
    m_stage = 0;
    m_torchEffect = 1;
    m_sndId[0] = -1;
    m_sndId[1] = -1;
    m_sndId[2] = -1;
    m_sndId[3] = -1;
    m_sndId[4] = -1;
    m_sndId[5] = -1;
}

grMarioPastBg::~grMarioPastBg() {
}

void grMarioPastBg::processAnim() {
}

// In the first stage (0) the torches burn with the sound of the stage start; in the second (1) the torches have a sound of
// their own each.
void grMarioPastBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (m_stage) {
    case 1:
        switch (m_state) {
        case 1: {
            snd3DGenerator* snd = m_snd;
            int* sndId = m_sndId;
            for (int i = 0; i < 6; i++) {
                if (*sndId == -1) {
                    *sndId = snd->playSE(SndID(0x1B77), 0, 0, -1);
                }
                Vec3f pos;
                getNodePosition(&pos, 0, m_node[i + 6]);
                snd->setPos(&pos);
                snd++;
                sndId++;
            }
            break;
        }
        case 0: {
            g_ecMgr->setDrawPrio(1);
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x360002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "TorchLocator02", 0);
            effect = g_ecMgr->setEffect(static_cast<EfID>(0x360002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "TorchLocator03", 0);
            effect = g_ecMgr->setEffect(static_cast<EfID>(0x360002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "TorchLocator04", 0);
            effect = g_ecMgr->setEffect(static_cast<EfID>(0x360002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "TorchLocator05", 0);
            effect = g_ecMgr->setEffect(static_cast<EfID>(0x360002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "TorchLocator06", 0);
            effect = g_ecMgr->setEffect(static_cast<EfID>(0x360002));
            g_ecMgr->setParent(effect, m_sceneModels[0], "TorchLocator07", 0);
            g_ecMgr->setDrawPrio(-1);
            getNodeIndex(&m_node[1], 0, "LiftLocator01");
            getNodeIndex(&m_node[2], 0, "LiftLocator02");
            getNodeIndex(&m_node[3], 0, "LiftLocator04");
            getNodeIndex(&m_node[4], 0, "LiftLocator03");
            getNodeIndex(&m_node[6], 0, "TorchLocator02");
            getNodeIndex(&m_node[7], 0, "TorchLocator03");
            getNodeIndex(&m_node[8], 0, "TorchLocator04");
            getNodeIndex(&m_node[9], 0, "TorchLocator05");
            getNodeIndex(&m_node[10], 0, "TorchLocator06");
            getNodeIndex(&m_node[11], 0, "TorchLocator07");
            m_state = 1;
            break;
        }
        }
        break;
    case 0:
        switch (m_state) {
        case 0: {
            m_snd[0].playSE(SndID(0x1B76), 0, 0, -1);
            Vec3f pos(0.0f, 0.0f, 0.0f);
            m_snd[0].setPos(&pos);
            getNodeIndex(&m_node[0], 0, "Castle");
            getNodeIndex(&m_node[5], 0, "ca");
            m_state = 1;
        }
            // fall through
        case 1:
            if (m_torchEffect == 1) {
                if (getMotionFrame(0) <= 3600.0f) {
                    setNodeVisibility(false, 0, m_node[0], true, false);
                } else {
                    setNodeVisibility(true, 0, m_node[0], true, false);
                    m_torchEffect = 0;
                }
            }
            break;
        }
        break;
    }
    updateCallBack(deltaFrame);
    updateG3dProcCalcWorld();
    m_hasUpdatedG3dCalcWorld = false;
    Vec3f* posGimmick = m_posGimmick;
    if (posGimmick != NULL) {
        if (m_stage == 1) {
            getNodePosition(&posGimmick[7], 0, m_node[1]);
            getNodePosition(&m_posGimmick[8], 0, m_node[2]);
            getNodePosition(&m_posGimmick[9], 0, m_node[3]);
            getNodePosition(&m_posGimmick[10], 0, m_node[4]);
        } else if (m_stage == 0) {
            getNodePosition(&posGimmick[12], 0, m_node[5]);
        }
    }
}

void grMarioPastBg::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (m_stage == 1) {
                if (calcWorldCallBack->m_nodeCallbackDatas == NULL) {
                    calcWorldCallBack->m_numNodeCallbackData = 2;
                    calcWorldCallBack->initialize(false, Heaps::StageInstance);
                    calcWorldCallBack->m_nodeCallbackDatas[0].m_flags |= 1;
                    calcWorldCallBack->m_nodeCallbackDatas[1].m_flags |= 1;
                }
                if (scnMdl->m_calcWorldCallBack == NULL) {
                    calcWorldCallBack->m_index = 0;
                    getNodeIndex(reinterpret_cast<u32*>(&calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex), 0, "AshibaA");
                    getNodeIndex(reinterpret_cast<u32*>(&calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex), 0, "AshibaB");
                    scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                    scnMdl->EnableScnMdlCallbackTiming(1);
                    scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
                }
                Vec3f* src = m_posGimmick;
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = src->m_x;
                data->m_pos.m_y = src->m_y;
                data->m_pos.m_z = src->m_z;
                src = m_posGimmick;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data[1].m_pos.m_x = src[1].m_x;
                data[1].m_pos.m_y = src[1].m_y;
                data[1].m_pos.m_z = src[1].m_z;
            } else if (m_stage == 0) {
                if (calcWorldCallBack->m_nodeCallbackDatas == NULL) {
                    calcWorldCallBack->m_numNodeCallbackData = 3;
                    calcWorldCallBack->initialize(false, Heaps::StageInstance);
                    calcWorldCallBack->m_nodeCallbackDatas[0].m_flags |= 1;
                    calcWorldCallBack->m_nodeCallbackDatas[1].m_flags |= 1;
                    calcWorldCallBack->m_nodeCallbackDatas[2].m_flags |= 1;
                }
                if (scnMdl->m_calcWorldCallBack == NULL) {
                    calcWorldCallBack->m_index = 0;
                    getNodeIndex(reinterpret_cast<u32*>(&calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex), 0, "AshibaA");
                    getNodeIndex(reinterpret_cast<u32*>(&calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex), 0, "AshibaB");
                    getNodeIndex(reinterpret_cast<u32*>(&calcWorldCallBack->m_nodeCallbackDatas[2].m_nodeIndex), 0, "haikei");
                    scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                    scnMdl->EnableScnMdlCallbackTiming(1);
                    scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
                }
                Vec3f* src = m_posGimmick;
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = src->m_x;
                data->m_pos.m_y = src->m_y;
                data->m_pos.m_z = src->m_z;
                src = m_posGimmick;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data[1].m_pos.m_x = src[1].m_x;
                data[1].m_pos.m_y = src[1].m_y;
                data[1].m_pos.m_z = src[1].m_z;
                src = m_posGimmick;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data[2].m_pos.m_x = src[2].m_x;
                data[2].m_pos.m_y = src[2].m_y;
                data[2].m_pos.m_z = src[2].m_z;
            }
        }
    }
}

// ---- BgLocator ----

grMarioPastBgLocator* grMarioPastBgLocator::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grMarioPastBgLocator* ground = new (Heaps::StageInstance) grMarioPastBgLocator(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grMarioPastBgLocator::grMarioPastBgLocator(const char* taskName) : grMarioPast(taskName) {
    m_frameLocator = NULL;
    m_posGimmick = NULL;
    m_stage = 0;
    m_node[0] = 0;
    m_node[1] = 0;
    m_node[2] = 0;
    m_node[3] = 0;
    m_node[4] = 0;
    m_node[5] = 0;
}

grMarioPastBgLocator::~grMarioPastBgLocator() {
}

// Publishes the positions of the locators and keeps the scenery in place at the beginning and the end of the animation.
void grMarioPastBgLocator::processAnim() {
    Ground::processAnim();
    if (m_posGimmick != NULL) {
        if (m_stage != 1) {
            if (m_stage == 0) {
                getNodePosition(&m_posGimmick[2], 0, m_node[0]);
            }
        }
        getNodePosition(&m_posGimmick[3], 0, m_node[1]);
        getNodePosition(&m_posGimmick[4], 0, m_node[2]);
        getNodePosition(&m_posGimmick[5], 0, m_node[3]);
        getNodePosition(&m_posGimmick[6], 0, m_node[4]);
        getNodePosition(&m_posGimmick[11], 0, m_node[5]);
        float frame = *m_frameLocator;
        if (m_stage == 1) {
            if (frame < 0.0f || frame >= 1560.0f) {
                if (frame >= 1560.0f && frame <= 6090.0f) {
                    Vec3f* pos = m_posGimmick;
                    pos[0].m_x = pos[4].m_x;
                    pos[0].m_y = pos[4].m_y;
                    pos[0].m_z = pos[4].m_z;
                }
            } else {
                Vec3f* pos = m_posGimmick;
                pos[0].m_x = pos[3].m_x;
                pos[0].m_y = pos[3].m_y;
                pos[0].m_z = pos[3].m_z;
            }
            if (frame < 0.0f || frame >= 4570.0f) {
                if (frame >= 4570.0f && frame <= 6090.0f) {
                    Vec3f* pos = m_posGimmick;
                    pos[1].m_x = pos[6].m_x;
                    pos[1].m_y = pos[6].m_y;
                    pos[1].m_z = pos[6].m_z;
                }
            } else {
                Vec3f* pos = m_posGimmick;
                pos[1].m_x = pos[5].m_x;
                pos[1].m_y = pos[5].m_y;
                pos[1].m_z = pos[5].m_z;
            }
        } else if (m_stage == 0) {
            if (frame < 0.0f || frame >= 4700.0f) {
                if (frame >= 4700.0f && frame <= 7200.0f) {
                    Vec3f* pos = m_posGimmick;
                    pos[0].m_x = pos[4].m_x;
                    pos[0].m_y = pos[4].m_y;
                    pos[0].m_z = pos[4].m_z;
                }
            } else {
                Vec3f* pos = m_posGimmick;
                pos[0].m_x = pos[3].m_x;
                pos[0].m_y = pos[3].m_y;
                pos[0].m_z = pos[3].m_z;
            }
            if (frame < 0.0f || frame >= 970.0f) {
                if (frame >= 970.0f && frame <= 7200.0f) {
                    Vec3f* pos = m_posGimmick;
                    pos[1].m_x = pos[6].m_x;
                    pos[1].m_y = pos[6].m_y;
                    pos[1].m_z = pos[6].m_z;
                }
            } else {
                Vec3f* pos = m_posGimmick;
                pos[1].m_x = pos[5].m_x;
                pos[1].m_y = pos[5].m_y;
                pos[1].m_z = pos[5].m_z;
            }
        }
    }
}

void grMarioPastBgLocator::update(float deltaFrame) {
    if (m_state != 1) {
        if (m_state != 0) {
            return;
        }
        float* data = static_cast<float*>(getStageData());
        if (data == NULL) {
            return;
        }
        if (m_stage == 1) {
            m_motionRatio = data[2];
        } else if (m_stage == 0) {
            m_motionRatio = data[1];
            g_ecMgr->setDrawPrio(1);
            u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x360001));
            g_ecMgr->setDrawPrio(-1);
            g_ecMgr->setParent(effect, m_sceneModels[0], "X_Move", 0);
            Vec3f scale(0.85f, 0.85f, 0.85f);
            g_ecMgr->setScl(effect, &scale);
        }
        m_state = 1;
    }
    *m_frameLocator = getMotionFrame(0);
}

bool grMarioPastBgLocator::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "haikei_locator");
    getNodeIndex(&m_node[1], 0, "Ashiba_locator_A");
    getNodeIndex(&m_node[2], 0, "Ashiba_locator_A02");
    getNodeIndex(&m_node[3], 0, "Ashiba_locator_B");
    getNodeIndex(&m_node[4], 0, "Ashiba_locator_B02");
    getNodeIndex(&m_node[5], 0, "X_Move");
    return result;
}
