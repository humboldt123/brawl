#include <gf/gf_model.h>
#include <gf/gf_task.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_shape.h>
#include <gr/collision/gr_collision_status.h>
#include <it/item.h>
#include <memory.h>
#include <types.h>

#include <st_homerun/gr_homerun.h>

// soExternalValueAccesser::isGroundTouch
extern "C" int fn_27_8CC2C(BaseItem* item, int kind);
// grCollStatus: the two ways to ask where a collision touched (the result holds the position in its last two floats)
extern "C" int fn_801341D8(grCollStatus* status, float* out);
extern "C" int fn_801342C8(grCollStatus* status, Vec2f* out);

// The Yakumono grounds of the contest all keep the position of the node they are bound to in a world callback.
// MATCH-ONLY: the original flips three bits of the joint's flag byte as a chain (HYPOTHESIS: they track "enabled").
static inline void grHomerunJointEnable(grCollisionJoint* joint) {
    joint->m_0x54_4 = true;
    joint->m_0x54_6 = joint->m_0x54_4;
}

static inline void grHomerunJointDisable(grCollisionJoint* joint) {
    joint->m_0x54_7 = false;
    joint->m_0x54_4 = joint->m_0x54_7;
    joint->m_0x54_6 = joint->m_0x54_4;
}


grHomerunBg* grHomerunBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grHomerunBg* ground = new (Heaps::StageInstance) grHomerunBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grHomerunBg::grHomerunBg(const char* taskName) : grHomerun(taskName) {
    m_posLimitWork = NULL;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_stateBarrierWork = NULL;
    m_hpBarrierWork = NULL;
    m_taskIdSandBagWork = NULL;
    m_taskIdHomerunBatWork = NULL;
    m_speedSandBagWork = NULL;
    m_landingSandBagWork = NULL;
    m_joint = NULL;
    m_frameSceneWork = NULL;
    m_node[0] = 0;
    m_node[1] = 0;
    m_node[2] = 0;
    m_lastHp = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 4;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[1].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[2].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[3].m_flags |= 0x10;
}

grHomerunBg::~grHomerunBg() { }

void grHomerunBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateJoint(deltaFrame);
        updateScroll(deltaFrame);
        updateCallBack(deltaFrame);
        if (m_frameSceneWork != NULL) {
            setMotionFrame(*m_frameSceneWork, 0);
        }
    }
}

// The barrier wall is the collision joint of the "ETC" node: it is looked up once and then follows the barrier state
// (it is gone while the barrier is broken).
void grHomerunBg::updateJoint(float deltaFrame) {
    grCollisionJoint* joint = m_joint;
    if (joint != NULL) {
        switch (*m_stateBarrierWork) {
        case 3:
        case 4:
            grHomerunJointDisable(joint);
            break;
        }
    } else {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 node;
            getNodeIndex(&node, 0, "ETC");
            u32 jointNum = (u16)collision->m_jointLen;
            for (u32 i = 0; i != jointNum; i++) {
                grCollisionJoint* candidate = collision->getJoint(i);
                if (candidate == NULL) {
                    break;
                }
                if (candidate->m_ground == this && candidate->_0x4E == 0 && node == candidate->m_nodeIndex) {
                    candidate->m_0x56_7 = true;
                    grHomerunJointEnable(candidate);
                    m_joint = candidate;
                    break;
                }
            }
        }
    }
}

void grHomerunBg::updateScroll(float deltaFrame) { }

// The three sky pieces follow the camera with an offset.
void grHomerunBg::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_node[0];
                calcWorldCallBack->m_nodeCallbackDatas[2].m_nodeIndex = m_node[1];
                calcWorldCallBack->m_nodeCallbackDatas[3].m_nodeIndex = m_node[2];
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_x = m_pos.m_x;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_y = m_pos.m_y;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_z = m_pos.m_z;
            Vec3f offset(0.0f, -250.0f, 250.0f);
            calcWorldCallBack->m_nodeCallbackDatas[1].m_offsetPos = offset;
            calcWorldCallBack->m_nodeCallbackDatas[2].m_offsetPos = offset;
            calcWorldCallBack->m_nodeCallbackDatas[3].m_offsetPos = offset;
        }
    }
}

bool grHomerunBg::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "Sky_01");
    getNodeIndex(&m_node[1], 0, "Sky_02");
    getNodeIndex(&m_node[2], 0, "Sky_03");
    setNodeVisibility(false, 0, m_node[2], true, false);
    return result;
}

// Only the colour animation of the backdrop follows the scene frame (the stadium changes with the distance).
void grHomerunBg::setMotionFrame(float frame, u32 animIndex) {
    gfModelAnimation* modelAnim = m_modelAnims[animIndex];
    if (modelAnim == NULL) {
        return;
    }
    modelAnim->m_anmObjMatClrRes->SetFrame(frame);
}

// The sandbag landed (it touched down below the stage): the backdrop reports it to the stage.
void grHomerunBg::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    if (m_landingSandBagWork != NULL) {
        if (*m_taskIdSandBagWork == collStatus->m_taskId) {
            Vec3f pos;
            if (collStatus->m_currentCollShape->getType() == 2) {
                float line[4];
                fn_801341D8(collStatus, line);
                pos.m_x = line[2];
                pos.m_y = line[3];
                pos.m_z = 0.0f;
            } else {
                Vec2f point;
                fn_801342C8(collStatus, &point);
                pos.m_x = point.m_x;
                pos.m_y = point.m_y;
                pos.m_z = 0.0f;
            }
            if (pos.m_y < -10.0f) {
                *m_landingSandBagWork = 1;
            }
        }
    }
}

// The sandbag or the bat touched the barrier: every hit with a new sandbag health takes health off the barrier.
void grHomerunBg::receiveCollMsg_Wall(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    if (m_hpBarrierWork != NULL) {
        bool isSandBag = true;
        int taskId = collStatus->m_taskId;
        if (*m_taskIdSandBagWork != taskId) {
            isSandBag = false;
        }
        bool isBat = true;
        if (*m_taskIdHomerunBatWork != taskId) {
            isBat = false;
        }
        if (isSandBag || isBat) {
            float* param = static_cast<float*>(getStageData());
            if (param != NULL) {
                BaseItem* item = static_cast<BaseItem*>(gfTask::getTask(*m_taskIdSandBagWork));
                if (item != NULL) {
                    int touch = fn_27_8CC2C(item, 8);
                    float hp = item->getParamFloat(2);
                    if (hp != m_lastHp) {
                        *m_hpBarrierWork -= param[1];
                        if (*m_hpBarrierWork < 0.0f) {
                            *m_hpBarrierWork = 0.0f;
                        }
                        if (0.0f == *m_hpBarrierWork) {
                            *m_stateBarrierWork = 4;
                            if (m_joint != NULL) {
                                grHomerunJointDisable(m_joint);
                            }
                        } else {
                            *m_stateBarrierWork = 1;
                        }
                    } else if (isSandBag == true && touch == 0) {
                        *m_stateBarrierWork = 1;
                    }
                    m_lastHp = hp;
                }
            }
        }
    }
}

