#define MT_VEC3F_BIKE_UTILITY_OPERATORS // the stage uses the operators with the register order of the native Utility code
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_resnode.h>
#include <st_dxbigblue/gr_dxbigblue_course.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>
#include <st_dxbigblue/st_dxbigblue_data.h>
#include <string.h>

// MATCH-ONLY: views of the node data of a model; the header names neither the name offset (+8) nor the id (+0xC).
static inline const char* dxbigblueNodeName(nw4r::g3d::ResNode& node) {
    u8* data = reinterpret_cast<u8*>(node.ptr());
    s32 offset = *reinterpret_cast<s32*>(data + 8);
    return offset != 0 ? reinterpret_cast<const char*>(data + offset) : NULL;
}
static inline u32 dxbigblueNodeId(nw4r::g3d::ResNode& node) {
    u8* data = reinterpret_cast<u8*>(node.ptr());
    return data != NULL ? *reinterpret_cast<u32*>(data + 0xC) : 0;
}

grDxBigBlueCourse* grDxBigBlueCourse::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grDxBigBlueCourse* ground = new (Heaps::StageInstance) grDxBigBlueCourse(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grDxBigBlueCourse::grDxBigBlueCourse(const char* taskName) : grDxBigBlue(taskName), m_pos(0.0f, 0.0f, 0.0f) {
    m_posWork = NULL;
    m_posLimitWork = NULL;
    m_posBgWork = NULL;
    m_rotWork = NULL;
    m_courseID = 12;
    m_courseIDTopWork = NULL;
    m_attachWork = NULL;
    m_nodeStart = 0;
    m_nodeEnd = 0;
}

grDxBigBlueCourse::~grDxBigBlueCourse() { }

// The start and end node of the course move with the model: they are read back every frame and made relative to the
// background.
void grDxBigBlueCourse::processAnim() {
    Ground::processAnim();
    if (m_isUpdate && m_posWork != NULL) {
        getNodePosition(&m_posWork[m_courseID * 2], 0, m_nodeStart);
        getNodePosition(&m_posWork[m_courseID * 2 + 1], 0, m_nodeEnd);
        m_posWork[m_courseID * 2] -= *m_posBgWork;
        m_posWork[m_courseID * 2 + 1] -= *m_posBgWork;
    }
}

void grDxBigBlueCourse::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateJoint(deltaFrame);
    updateActive(deltaFrame);
    updateMove(deltaFrame);
    updateCallBack(deltaFrame);
}

void grDxBigBlueCourse::updateJoint(float deltaFrame) { }

// The course waits (not shown) until the stage hands it its place in the line (the list of the stage says which course is
// next); then it appears at the end of the line, is solid while it flies by, and goes when it has passed the camera.
void grDxBigBlueCourse::updateActive(float deltaFrame) {
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
            case 1: {
                u8 attach = *m_attachWork;
                if (attach != 12) {
                    if (attach == 13) {
                        float y = data->unk00;
                        m_pos.m_x = -300.0f;
                        m_pos.m_y = y;
                        m_pos.m_z = 0.0f;
                    } else {
                        m_pos = m_posWork[attach * 2 + 1];
                    }
                    setVisibility(1);
                    setEnableCollisionStatus(true);
                    if (m_collision != NULL) {
                        m_collision->setEnable();
                    }
                    m_state = 2;
                }
                break;
            }
            case 2:
                if (m_posWork[m_courseID * 2 + 1].m_x < m_posLimitWork[0]) {
                    *m_attachWork = 12;
                    m_state = 0;
                }
                break;
        }
    }
}

// The course moves to the left with the speed of the race.
void grDxBigBlueCourse::updateMove(float deltaFrame) {
    stDxBigBlueData* data = static_cast<stDxBigBlueData*>(getStageData());
    if (data != NULL) {
        switch (m_state) {
            case 2:
                m_pos.m_x -= data->unk6C * deltaFrame;
                break;
        }
    }
}

void grDxBigBlueCourse::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (calcWorldCallBack->m_nodeCallbackDatas == NULL) {
                calcWorldCallBack->m_numNodeCallbackData = 1;
                calcWorldCallBack->initialize(false, Heaps::StageInstance);
                calcWorldCallBack->m_nodeCallbackDatas[0].m_flags |= 1;
            }
            DXBIGBLUE_BIND_CALLBACK(calcWorldCallBack, scnMdl)
            Vec3f pos;
            pos = m_pos + *m_posBgWork;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos = pos;
        }
    }
}

// Finds the start and end node of the course: the first nodes of the model that contain "StartN" and "EndN" in their name.
bool grDxBigBlueCourse::setNode() {
    bool result = grGimmick::setNode();
    nw4r::g3d::ResMdl resMdl(NULL);
    if (m_sceneModels[0] == NULL) {
        return result;
    }
    resMdl = m_sceneModels[0]->m_resMdl;
    if (resMdl.ptr() == NULL) {
        return result;
    }
    bool foundStart = false;
    bool foundEnd = false;
    u32 i = 0;
    u32 count = resMdl.GetResNodeNumEntries();
    for (; i != count; i++) {
        nw4r::g3d::ResNode node = resMdl.GetResNode(i);
        if (!foundStart) {
            if (strstr(dxbigblueNodeName(node), "StartN") != NULL) {
                m_nodeStart = dxbigblueNodeId(node);
                foundStart = true;
            }
        }
        if (!foundEnd) {
            if (strstr(dxbigblueNodeName(node), "EndN") != NULL) {
                m_nodeEnd = dxbigblueNodeId(node);
                foundEnd = true;
            }
        }
        if (foundStart == true && foundEnd == true) {
            break;
        }
    }
    return result;
}
