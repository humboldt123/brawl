#include <gf/gf_model.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <it/it_manager.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <so/so_external_value_accesser.h>
#include <stdio.h>
#include <string.h>

#include <st_gw/gr_gw_item.h>
#include <st_gw/gr_gw_scene_fire.h>
#include <st_gw/st_gw.h>

static grGWMarker sMarkerA(0xFF, 0);
static grGWMarker sMarkerB(0xFF, 1);

// MATCH-ONLY: the original flips the same bits of the joint's flag byte as a chain (HYPOTHESIS: they track "enabled").
static inline void gwJointEnable(grCollisionJoint* joint) {
    joint->m_0x54_4 = true;
    joint->m_0x54_6 = joint->m_0x54_4;
}

static inline void gwJointDisable(grCollisionJoint* joint) {
    joint->m_0x54_7 = false;
    joint->m_0x54_4 = joint->m_0x54_7;
    joint->m_0x54_6 = joint->m_0x54_4;
}

grGWSceneFire* grGWSceneFire::create(int modelIndex, const char* nodeName, const char* taskName) {
    grGWSceneFire* ground = new (Heaps::StageInstance) grGWSceneFire(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(modelIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grGWSceneFire::grGWSceneFire(const char* taskName) : grGWScene(taskName), m_snd(), m_steps() {
    m_timerStep = 120.0f;
    m_stepManhoreWork = NULL;
    m_typeManhoreWork = NULL;
    _1a0 = 0;
    m_joints = NULL;
    m_stateDiver = 0;
    m_timerDiver = 0.0f;
    m_posDiverWork = NULL;
    m_typeDiverWork = NULL;
    m_sideDiver = 0;
    m_posItemX = 0.0f;
    m_itemId = 0;
    m_posLimitWork = NULL;
}

grGWSceneFire::~grGWSceneFire() {
    if (m_joints != NULL) {
        delete[] m_joints;
    }
    m_joints = NULL;
}

// The places of the 20 steps and of the two divers come from the nodes of the model.
void grGWSceneFire::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        for (u32 i = 0; i < 20; i++) {
            char name[0x88];
            strcpy(name, "_FIRE_STEP_POSITION__");
            sprintf(name, "_FIRE_STEP_POSITION__%.2d", i + 1);
            getNodePosition(&m_posGimmickWork[i], 0, name);
        }
    }
    if (m_posDiverWork != NULL) {
        getNodePosition(&m_posDiverWork[0], 0, "_FIRE_DIVER_POSITION_01");
        getNodePosition(&m_posDiverWork[1], 0, "_FIRE_DIVER_POSITION_02");
    }
}

void grGWSceneFire::update(float deltaFrame) {
    grGWScene::update(deltaFrame);
    updateJoint(deltaFrame);
}

void grGWSceneFire::updateSub(float deltaFrame) {
    grGWScene::updateSub(deltaFrame);
    updateDiver(deltaFrame);
}

void grGWSceneFire::updateSubEnd(float deltaFrame) {
    grGWScene::updateSubEnd(deltaFrame);
    m_stateDiver = 0;
}

// The collision joints of the steps are looked up once (and marked) when the model has its collision.
void grGWSceneFire::updateJoint(float deltaFrame) {
    if (m_joints == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            m_joints = new (Heaps::StageInstance) grCollisionJoint*[20];
            if (m_joints != NULL) {
                u16 count = collision->m_jointLen;
                for (u32 i = 0; i != count; i++) {
                    grCollisionJoint* joint = collision->getJoint(i);
                    if (joint != NULL) {
                        joint->m_0x56_7 = true;
                        m_joints[i] = joint;
                    }
                }
            }
        }
    }
}

// The diver jumps for an item that lies on the stage and takes it away (and plays a sound) when it reaches it.
void grGWSceneFire::updateDiver(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        m_timerDiver -= deltaFrame;
        if (m_timerDiver < 0.0f) {
            m_timerDiver = 0.0f;
        }
        switch (m_stateDiver) {
        case 0:
            *m_typeDiverWork = 0x10;
            m_timerDiver = 60.0f;
            m_stateDiver = 1;
            break;
        case 1:
            if (m_timerDiver == 0.0f) {
                m_timerDiver = 60.0f;
                if (randf() <= data[8]) {
                    itManager* items = itManager::getInstance();
                    if (items != NULL) {
                        int i = 0;
                        int count = reinterpret_cast<gwItemManagerView*>(items)->m_list->getCount();
                        BaseItem* item = NULL;
                        if (count >= 0) {
                            while (i != count) {
                                item = fn_27_2A8A00(items, i);
                                if (item != NULL && isRemoveItem(*reinterpret_cast<int*>(reinterpret_cast<u8*>(item) + 0x8BC)) == 1) {
                                    break;
                                }
                                i++;
                            }
                            if (i != count && item != NULL) {
                                Vec3f itemPos = soExternalValueAccesser::getPos(item);
                                m_posItemX = itemPos.m_x;
                                m_itemId = *reinterpret_cast<int*>(reinterpret_cast<u8*>(item) + 0x8BC);
                                if (randf() >= 0.5f) {
                                    m_sideDiver = 2;
                                } else {
                                    m_sideDiver = 1;
                                }
                                switch (m_sideDiver) {
                                case 1:
                                    m_posDiverWork[2] = m_posDiverWork[0];
                                    *m_typeDiverWork = 10;
                                    break;
                                case 2:
                                    m_posDiverWork[2] = m_posDiverWork[1];
                                    *m_typeDiverWork = 13;
                                    break;
                                }
                                m_timerDiver = data[9];
                                m_stateDiver = 2;
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            if (m_timerDiver == 0.0f) {
                Vec3f dir = m_posLimitWork[1] - m_posLimitWork[0];
                float len = gwLength(dir);
                Vec3f dirN = dir;
                dirN.normalize();
                float half = len * 0.5f;
                dirN.m_x = dirN.m_x * half;
                dirN.m_y = dirN.m_y * half;
                dirN.m_z = dirN.m_z * half;
                Vec3f center;
                center.m_x = m_posLimitWork[0].m_x + dirN.m_x;
                center.m_y = m_posLimitWork[0].m_y + dirN.m_y;
                center.m_z = m_posLimitWork[0].m_z + dirN.m_z;
                switch (m_sideDiver) {
                case 1:
                    m_posDiverWork[2].m_x = m_posDiverWork[2].m_x + data[10];
                    if (*m_typeDiverWork == 11) {
                        *m_typeDiverWork = 10;
                    } else if (*m_typeDiverWork < 11 && *m_typeDiverWork > 9) {
                        *m_typeDiverWork = 11;
                    }
                    if (m_posDiverWork[2].m_x <= m_posItemX - 10.0f) {
                        m_timerDiver = data[9];
                    } else {
                        if (isRemoveItem(m_itemId) == 1) {
                            *m_typeDiverWork = 12;
                            m_timerDiver = 120.0f;
                        } else {
                            m_timerDiver = 60.0f;
                        }
                        m_stateDiver = 3;
                    }
                    break;
                case 2:
                    m_posDiverWork[2].m_x = m_posDiverWork[2].m_x - data[10];
                    if (*m_typeDiverWork == 14) {
                        *m_typeDiverWork = 13;
                    } else if (*m_typeDiverWork < 14 && *m_typeDiverWork > 12) {
                        *m_typeDiverWork = 14;
                    }
                    if (m_posItemX + 10.0f <= m_posDiverWork[2].m_x) {
                        m_timerDiver = data[9];
                    } else {
                        if (isRemoveItem(m_itemId) == 1) {
                            *m_typeDiverWork = 15;
                            m_timerDiver = 120.0f;
                        } else {
                            m_timerDiver = 60.0f;
                        }
                        m_stateDiver = 3;
                    }
                    break;
                }
            }
            break;
        case 3:
            if (m_timerDiver == 0.0f) {
                switch (m_sideDiver) {
                case 1:
                    *m_typeDiverWork = 13;
                    break;
                case 2:
                    *m_typeDiverWork = 10;
                    break;
                }
                m_stateDiver = 4;
            } else if (m_timerDiver < 60.0f && isRemoveItem(m_itemId) == 1) {
                itManager* items = itManager::getInstance();
                BaseItem* item = items->getItemFromInstanceId(m_itemId);
                if (item != NULL) {
                    Vec3f itemPos = soExternalValueAccesser::getPos(item);
                    if (fabsf(itemPos.m_x - m_posDiverWork[2].m_x) < 10.0f) {
                        itManager::getInstance()->removeItem(item);
                        m_snd.playSE(static_cast<SndID>(0x1C92), 0, 0, -1);
                        m_snd.setPos(&m_posDiverWork[2]);
                    }
                }
                m_itemId = 0;
            }
            break;
        case 4:
            if (m_timerDiver == 0.0f) {
                switch (m_sideDiver) {
                case 1:
                    m_posDiverWork[2].m_x = m_posDiverWork[2].m_x - data[10];
                    if (*m_typeDiverWork == 14) {
                        *m_typeDiverWork = 13;
                    } else if (*m_typeDiverWork < 14 && *m_typeDiverWork > 12) {
                        *m_typeDiverWork = 14;
                    }
                    if (m_posLimitWork[0].m_x <= m_posDiverWork[2].m_x) {
                        m_timerDiver = data[9];
                    } else {
                        m_stateDiver = 0;
                    }
                    break;
                case 2:
                    m_posDiverWork[2].m_x = m_posDiverWork[2].m_x + data[10];
                    if (*m_typeDiverWork == 11) {
                        *m_typeDiverWork = 10;
                    } else if (*m_typeDiverWork < 11 && *m_typeDiverWork > 9) {
                        *m_typeDiverWork = 11;
                    }
                    if (m_posDiverWork[2].m_x <= m_posLimitWork[1].m_x) {
                        m_timerDiver = data[9];
                    } else {
                        m_stateDiver = 0;
                    }
                    break;
                }
            }
            break;
        }
    }
}

// Every few frames (the stage data says how many) a new step is picked while the event of the scene is on.
void grGWSceneFire::updateStep(float deltaFrame) {
    if (*m_eventWork == 1 && m_motion == 1) {
        float* data = static_cast<float*>(getStageData());
        if (data != NULL) {
            m_timerStep -= deltaFrame;
            if (m_timerStep < 0.0f) {
                m_timerStep = 0.0f;
            }
            if (m_timerStep == 0.0f) {
                selectStepManhore();
                m_timerStep = data[6] + (data[7] - data[6]) * randf();
            }
        }
    }
}

// The event of the scene: the fire man is called (the event flag of the stage is set) for a while, then the steps are
// cleared and the fire is put out.
void grGWSceneFire::updateEvent(float deltaFrame) {
    if (m_eventOn != 0) {
        float* data = static_cast<float*>(getStageData());
        if (data != NULL) {
            switch (m_stateEvent) {
            case 0: {
                *m_eventWork = 1;
                float r = randf();
                m_stateEvent = 1;
                m_timerEvent = data[12] + r * (data[13] - data[12]);
                break;
            }
            case 1:
                m_timerEvent -= deltaFrame;
                if (m_timerEvent < 0.0f) {
                    m_timerEvent = 0.0f;
                }
                if (m_timerEvent == 0.0f) {
                    *m_eventWork = 0;
                    clearStep();
                    clearStepManhore();
                    m_timerEvent = data[14];
                    m_stateEvent = 3;
                }
                break;
            }
        }
    }
}

// The steps that the scene data lists for the current control step are switched on (the others off).
void grGWSceneFire::selectStep() {
    if (m_joints != NULL && m_ctrlStepWork != NULL && m_tblStep != NULL) {
        float* data = static_cast<float*>(getStageData());
        if (data != NULL) {
            u8* table = reinterpret_cast<u8*>(m_tblStep->getData(*m_ctrlStepWork));
            if (table == NULL) {
                *m_ctrlStepWork = 0;
                table = reinterpret_cast<u8*>(m_tblStep->getData(*m_ctrlStepWork));
                if (table == NULL) {
                    return;
                }
            }
            for (int i = 0; i < 10; i++) {
                if (table[0] == 1) {
                    gwJointEnable(m_joints[i * 2]);
                } else {
                    gwJointDisable(m_joints[i * 2]);
                }
                if (table[1] == 1) {
                    gwJointEnable(m_joints[i * 2 + 1]);
                } else {
                    gwJointDisable(m_joints[i * 2 + 1]);
                }
                table += 2;
            }
        }
    }
}

// Picks the step that the fire comes to: all the steps with a fire entry (10 to 14) in the table are candidates, one of them
// is chosen at random.
void grGWSceneFire::selectStepManhore() {
    u8* table = reinterpret_cast<u8*>(m_tblStep->getData(*m_ctrlStepWork));
    if (table != NULL) {
        for (u32 i = 0; i != 20; i++) {
            u8 entry = table[i & 0xFF];
            if (entry < 15 && entry > 9) {
                StepNode* node = new (Heaps::StageInstance) StepNode;
                if (node == NULL) {
                    return;
                }
                node->m_index = i;
                m_steps.PushBack(node);
            }
        }
        selectStepManhoreDetails();
        u32 count = m_steps.GetSize();
        for (u32 i = 0; (i & 0xFF) != (count & 0xFF); i++) {
            StepNode* node = &m_steps.GetFront();
            m_steps.Erase(m_steps.GetBeginIter());
            delete node;
        }
    }
}

// The step is chosen from the candidates and the type of the fire (6 to 9) is set from the entry of the table: low entries
// have no fire (0x10), the entry 10 picks any of the four at random.
void grGWSceneFire::selectStepManhoreDetails() {
    u8* table = reinterpret_cast<u8*>(m_tblStep->getData(*m_ctrlStepWork));
    if (table == NULL) {
        return;
    }
    u32 size = m_steps.GetSize();
    u32 pick = static_cast<u32>(static_cast<float>(size) * randf()) & 0xFF;
    if (pick > 19) {
        return;
    }
    nw4r::ut::LinkList<StepNode, 0>::Iterator node = m_steps.GetBeginIter();
    u32 count = m_steps.GetSize();
    u32 i = 0;
    for (; count != 0 && pick != (i & 0xFF); i++, ++node) {
        count--;
    }
    if ((i & 0xFF) == (m_steps.GetSize() & 0xFF)) {
        return;
    }
    u8 index = node->m_index;
    u8 current = *m_stepManhoreWork;
    if (current == index) {
        return;
    }
    if (current != 0xFF) {
        gwJointDisable(m_joints[current]);
    }
    *m_stepManhoreWork = index;
    switch (table[index]) {
    case 10: {
        float r = randf();
        if (r >= 0.25f) {
            if (r >= 0.5f) {
                if (r >= 0.75f) {
                    *m_typeManhoreWork = 9;
                } else {
                    *m_typeManhoreWork = 8;
                }
            } else {
                *m_typeManhoreWork = 7;
            }
        } else {
            *m_typeManhoreWork = 6;
        }
        break;
    }
    case 11:
        *m_typeManhoreWork = 6;
        break;
    case 12:
        *m_typeManhoreWork = 7;
        break;
    case 13:
        *m_typeManhoreWork = 8;
        break;
    case 14:
        *m_typeManhoreWork = 9;
        break;
    default:
        *m_typeManhoreWork = 0x10;
        return;
    }
    gwJointEnable(m_joints[index]);
}

void grGWSceneFire::clearStep() {
    if (m_joints == NULL) {
        return;
    }
    for (int i = 0; i < 4; i++) {
        gwJointDisable(m_joints[i * 5]);
        gwJointDisable(m_joints[i * 5 + 1]);
        gwJointDisable(m_joints[i * 5 + 2]);
        gwJointDisable(m_joints[i * 5 + 3]);
        gwJointDisable(m_joints[i * 5 + 4]);
    }
}

void grGWSceneFire::clearStepManhore() {
    if (*m_stepManhoreWork != 0xFF) {
        gwJointDisable(m_joints[*m_stepManhoreWork]);
    }
    *m_stepManhoreWork = 0xFF;
    m_timerStep = 0.0f;
}

// An item is taken away when a fighter could pick it up, nobody holds it and it fell below the stage.
bool grGWSceneFire::isRemoveItem(int itemId) {
    if (itemId == 0) {
        return false;
    }
    itManager* items = itManager::getInstance();
    if (items == NULL) {
        return false;
    }
    BaseItem* item = items->getItemFromInstanceId(itemId);
    if (item == NULL) {
        return false;
    }
    if (!item->isPickUp()) {
        return false;
    }
    if (reinterpret_cast<gwItemView*>(item)->m_box->m_holder->getCarryKind() == 2) {
        return false;
    }
    if (item->getParamFloat(0x3EE) == 0.0f) {
        Vec3f pos = soExternalValueAccesser::getPos(item);
        return !(-40.0f < pos.m_y);
    }
    return false;
}
