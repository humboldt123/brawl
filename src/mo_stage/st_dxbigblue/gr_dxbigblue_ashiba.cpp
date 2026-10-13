#include <string.h>

#include <st_dxbigblue/gr_dxbigblue_ashiba.h>
#include <st_dxbigblue/gr_dxbigblue_ashiba_trainer.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>
#include <st_dxbigblue/st_dxbigblue_data.h>

// grCollStatus::getPos (main): where the collision touched (x, y).
extern "C" int getPos__12grCollStatusFP5Vec2f(grCollStatus* status, Vec2f* out);

grDxBigBlueAshiba* grDxBigBlueAshiba::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grDxBigBlueAshiba* ground = new (Heaps::StageInstance) grDxBigBlueAshiba(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

// The platform comes from one side (picked by chance), flies across and goes when it has left the camera.
void grDxBigBlueAshiba::updateActive(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
            case 0:
                setVisibility(0);
                setEnableCollisionStatus(false);
                m_state = 1;
                break;
            case 1:
                if (m_flyDataWork->m_state != 10) {
                    if (randf() < 0.5f) {
                        m_flyDataWork->m_pos.m_x = m_posLimitWork[0] - data->unk68;
                        m_dir = 1;
} else {
                        m_flyDataWork->m_pos.m_x = m_posLimitWork[3] + data->unk68;
                        m_dir = 0;
                                        }
                    m_state = 2;
                }
                break;
            case 2:
                if (m_flyDataWork->m_state == 10) {
                    m_state = 0;
                } else {
                    if (!m_isVisible) {
                        setVisibility(1);
                        setEnableCollisionStatus(true);
                    }
                    updateMove(deltaFrame);
                }
                break;
        }
    }
}

void grDxBigBlueAshiba::updateMove(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        unk158 -= deltaFrame;
        if (unk158 < 0.0f) {
            unk158 = 0.0f;
        }
        m_sinkTimer -= deltaFrame;
        if (m_sinkTimer < 0.0f) {
            m_sinkTimer = 0.0f;
        }
        if (m_dir == 1) {
            m_flyDataWork->m_pos.m_x += data->unk98 * deltaFrame;
            if (m_flyDataWork->m_pos.m_x > m_posLimitWork[3] + data->unk68) {
                m_flyDataWork->m_state = 10;
            }
        } else {
            m_flyDataWork->m_pos.m_x -= data->unk98 * deltaFrame;
            if (m_flyDataWork->m_pos.m_x < m_posLimitWork[0] - data->unk68) {
                m_flyDataWork->m_state = 10;
            }
        }
        if (m_sinkTimer == 0.0f) {
            m_sinkOffset = 0.0f;
        }
    }
}

// The model follows the record of the stage; it tilts to the side the fighter landed on.
void grDxBigBlueAshiba::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            DXBIGBLUE_BIND_CALLBACK(calcWorldCallBack, scnMdl)
            stDxBigBlueData* data;
            if (m_flyDataWork != NULL && (data = static_cast<stDxBigBlueData*>(getStageData())) != NULL) {
                grNodeCallbackData* node = &calcWorldCallBack->m_nodeCallbackDatas[0];
                Vec3f rot;
                rot = node->m_rot;
                rot.m_z = rot.m_z + ((m_flyDataWork->m_rot.m_z + m_sinkOffset) - rot.m_z) * data->unk9C;
                node->m_pos = m_flyDataWork->m_pos;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot = rot;
            }
        }
    }
}

// A fighter lands: the platform sinks for a while, and tilts away from the side the fighter stands on.
void grDxBigBlueAshiba::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    grDxBigBlueFly::receiveCollMsg_Landing(collStatus, collisionJoint, isFirstContact);
    if (isFirstContact) {
        stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
        if (data != NULL) {
            Vec2f pos;
            getPos__12grCollStatusFP5Vec2f(collStatus, &pos);
            m_sinkTimer = data->unkB4;
            if (pos.m_x > m_flyDataWork->m_pos.m_x) {
                m_landSide = 1;
            } else {
                m_landSide = 0;
            }
            float tilt = data->unkA0 + (data->unkA4 - data->unkA0) * randf();
            m_sinkOffset = tilt;
            if (m_landSide == 1) {
                m_sinkOffset = -tilt;
            }
        }
    }
}

grDxBigBlueAshibaTrainer* grDxBigBlueAshibaTrainer::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grDxBigBlueAshibaTrainer* ground = new (Heaps::StageInstance) grDxBigBlueAshibaTrainer(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grDxBigBlueAshibaTrainer::~grDxBigBlueAshibaTrainer() { }

// The four trainer nodes are read back every frame for the stage.
void grDxBigBlueAshibaTrainer::processAnim() {
    Ground::processAnim();
    if (m_posTrainerWork != NULL) {
        getNodePosition(&m_posTrainerWork[0], 0, m_nodeTrainer[0]);
        getNodePosition(&m_posTrainerWork[1], 0, m_nodeTrainer[1]);
        getNodePosition(&m_posTrainerWork[2], 0, m_nodeTrainer[2]);
        getNodePosition(&m_posTrainerWork[3], 0, m_nodeTrainer[3]);
    }
}

// Like the other platform, but the trainer's platform comes in to the middle of the stage and turns around there (it
// never goes through the whole stage).
void grDxBigBlueAshibaTrainer::updateActive(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
        case 0:
            setVisibility(0);
            setEnableCollisionStatus(false);
            m_state = 1;
        case 1:
            if (m_flyDataWork->m_state == 10) {
                return;
            }
            if (randf() < 0.5f) {
                m_flyDataWork->m_pos.m_x = m_posLimitWork[3] * 0.5f;
                m_dir = 0;
            } else {
                m_flyDataWork->m_pos.m_x = m_posLimitWork[0] * 0.5f;
                m_dir = 1;
            }
            m_state = 2;
        case 2:
            if (m_flyDataWork->m_state == 10) {
                m_state = 0;
            } else {
                if (!m_isVisible) {
                    setVisibility(1);
                    setEnableCollisionStatus(true);
                }
                updateMove(deltaFrame);
            }
            break;
    }
}

void grDxBigBlueAshibaTrainer::updateMove(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        unk158 -= deltaFrame;
        if (unk158 < 0.0f) {
            unk158 = 0.0f;
        }
        if (m_dir == 1) {
            m_flyDataWork->m_pos.m_x += data->unk98 * deltaFrame;
            if (m_flyDataWork->m_pos.m_x > m_posLimitWork[3] * 0.7f) {
                m_dir = 0;
            }
        } else {
            m_flyDataWork->m_pos.m_x -= data->unk98 * deltaFrame;
            if (m_flyDataWork->m_pos.m_x < m_posLimitWork[0] * 0.7f) {
                m_dir = 1;
            }
        }
    }
}

void grDxBigBlueAshibaTrainer::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            DXBIGBLUE_BIND_CALLBACK(calcWorldCallBack, scnMdl)
            stDxBigBlueData* data;
            if (m_flyDataWork != NULL && (data = static_cast<stDxBigBlueData*>(getStageData())) != NULL) {
                grNodeCallbackData* node = &calcWorldCallBack->m_nodeCallbackDatas[0];
                Vec3f rot;
                rot = node->m_rot;
                rot.m_z = rot.m_z + ((m_flyDataWork->m_rot.m_z + m_sinkOffset) - rot.m_z) * data->unk9C;
                node->m_pos = m_flyDataWork->m_pos;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_z = -160.0f;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot = rot;
            }
        }
    }
}

bool grDxBigBlueAshibaTrainer::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeTrainer[0], 0, "PokemonTrainer1N");
    getNodeIndex(&m_nodeTrainer[1], 0, "PokemonTrainer2N");
    getNodeIndex(&m_nodeTrainer[2], 0, "PokemonTrainer3N");
    getNodeIndex(&m_nodeTrainer[3], 0, "PokemonTrainer4N");
    return result;
}
