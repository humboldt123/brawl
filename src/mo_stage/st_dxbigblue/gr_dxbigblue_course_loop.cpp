#include <gr/collision/gr_collision.h>
#include <st_dxbigblue/gr_dxbigblue_course_loop.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>
#include <st_dxbigblue/st_dxbigblue_data.h>

grDxBigBlueCourseLoop* grDxBigBlueCourseLoop::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grDxBigBlueCourseLoop* ground = new (Heaps::StageInstance) grDxBigBlueCourseLoop(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grDxBigBlueCourseLoop::~grDxBigBlueCourseLoop() { }

// As the course, and the vector from start to end is taken once.
void grDxBigBlueCourseLoop::processAnim() {
    grDxBigBlueCourse::processAnim();
    if (m_isUpdate && m_posWork != NULL && m_len.m_x == 0.0f) {
        Vec3f* start = &m_posWork[m_courseID * 2];
        Vec3f* end = &m_posWork[m_courseID * 2 + 1];
        m_len = *end - *start;
    }
}

// Finds the two collisions of the loop.
void grDxBigBlueCourseLoop::updateJoint(float deltaFrame) {
    if (m_jointStart == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u16 jointLen = collision->m_jointLen;
            for (u16 i = 0; i != jointLen; i++) {
                grCollisionJoint* joint = collision->getJoint(i);
                if (joint != NULL) {
                    if (joint->m_ground == this && joint->_0x4E == 0) {
                        if (m_jointStart == NULL && (u32)joint->m_nodeIndex == getNodeIndex(0, "LoopStartN")) {
                            joint->m_0x56_7 = true;
                            dxbigblueJointDisable(joint);
                            m_jointStart = joint;
                        } else if (m_jointEnd == NULL && (u32)joint->m_nodeIndex == getNodeIndex(0, "LoopEndN")) {
                            joint->m_0x56_7 = true;
                            dxbigblueJointDisable(joint);
                            m_jointEnd = joint;
                        }
                    }
                    if (m_jointStart != NULL && m_jointEnd != NULL) {
                        break;
                    }
                }
            }
        }
    }
}

// As the course; the collisions of the loop go on and off with it.
void grDxBigBlueCourseLoop::updateActive(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        switch (m_state) {
            case 0:
                setVisibility(0);
                setEnableCollisionStatus(false);
                if (m_jointStart != NULL) {
                    dxbigblueJointDisable(m_jointStart);
                }
                if (m_jointEnd != NULL) {
                    dxbigblueJointDisable(m_jointEnd);
                }
                m_state = 1;
                unk194 = 0.0f;
                break;
            case 1:
                if (*m_attachWork != 12) {
                    Vec3f* end = &m_posWork[*m_attachWork * 2 + 1];
                    m_pos = *end;
                    setVisibility(1);
                    setEnableCollisionStatus(true);
                    if (m_collision != NULL) {
                        m_collision->setEnable();
                    }
                    if (m_jointStart != NULL) {
                        dxbigblueJointEnable(m_jointStart);
                    }
                    if (m_jointEnd != NULL) {
                        dxbigblueJointDisable(m_jointEnd);
                    }
                    m_state = 2;
                }
                break;
            case 2:
                if (unk1A4 == 1 && m_posWork[m_courseID * 2 + 1].m_x < m_posLimitWork[0]) {
                    *m_attachWork = 12;
                    m_state = 0;
                }
                break;
        }
    }
}

// The loop turns while it is in the middle of the screen (the node of the center is left of 0), then the course moves on.
void grDxBigBlueCourseLoop::updateMove(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        if (m_state == 2) {
            if (*m_courseIDTopWork == m_courseID) {
                Vec3f center;
                getNodePosition(&center, 0, "LoopCenterN");
                if (center.m_x < 0.0f) {
                    if (m_rotWork->m_z != 360.0f) {
                        m_rotWork->m_z += deltaFrame * 0.24f;
                        if (m_rotWork->m_z < 0.0f) {
                            m_rotWork->m_z = 0.0f;
                        }
                        if (m_rotWork->m_z > 360.0f) {
                            m_rotWork->m_z = 360.0f;
                        }
                        if (m_rotWork->m_z == 360.0f) {
                            unk1A4 = 1;
                        } else {
                            unk1A4 = 0;
                        }
                    }
                }
                if (0.0f <= center.m_x || m_rotWork->m_z == 360.0f) {
                    m_timer = 1500.0f;
                    m_pos.m_x -= data->unk6C * deltaFrame;
                }
                unk194 = m_rotWork->m_z / 180.0f;
                if (unk194 < 0.0f) {
                    unk194 = 0.0f;
                }
                if (unk194 > 1.0f) {
                    unk194 = 1.0f;
                }
            } else {
                m_pos.m_x -= data->unk6C * deltaFrame;
            }
            if (m_rotWork->m_z >= 210.0f) {
                dxbigblueJointDisable(m_jointStart);
            } else {
                dxbigblueJointEnable(m_jointStart);
            }
            if (m_rotWork->m_z <= 140.0f) {
                dxbigblueJointDisable(m_jointEnd);
            } else {
                dxbigblueJointEnable(m_jointEnd);
            }
        }
    }
}

// The model has two nodes: the course itself and the loop (which turns).
void grDxBigBlueCourseLoop::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (calcWorldCallBack->m_nodeCallbackDatas == NULL) {
                calcWorldCallBack->m_numNodeCallbackData = 2;
                calcWorldCallBack->initialize(false, Heaps::StageInstance);
                calcWorldCallBack->m_nodeCallbackDatas[0].m_flags |= 1;
                calcWorldCallBack->m_nodeCallbackDatas[1].m_flags |= 2;
            }
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_nodeCenter;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (getStageData() != NULL) {
                Vec3f pos;
                pos = m_pos + *m_posBgWork;
                pos.m_z -= m_len.m_z * unk194;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = pos;
                Vec3f rot;
                rot = *m_rotWork;
                rot.m_z = -rot.m_z;
                calcWorldCallBack->m_nodeCallbackDatas[1].m_rot = rot;
            }
        }
    }
}

bool grDxBigBlueCourseLoop::setNode() {
    bool result = grDxBigBlueCourse::setNode();
    getNodeIndex(&m_nodeCenter, 0, "LoopCenterN");
    return result;
}
