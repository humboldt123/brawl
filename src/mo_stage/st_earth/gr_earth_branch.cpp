#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>
#include <types.h>

#include <st_earth/gr_earth.h>

// sora_melee's constant of the speed of the game (1.0 at 60 frames in a second)
extern "C" float lbl_27_data_547E0;

grEarthBranch::grEarthBranch(const char* taskName) : grEarth(taskName) {
    m_node[0] = 0;
    m_pos[0].m_x = 0.0f;
    m_pos[0].m_y = 0.0f;
    m_pos[0].m_z = 0.0f;
    m_phase[0] = 0.0f;
    m_node[1] = 0;
    m_pos[1].m_x = 0.0f;
    m_pos[1].m_y = 0.0f;
    m_pos[1].m_z = 0.0f;
    m_phase[1] = 0.0f;
    m_node[2] = 0;
    m_pos[2].m_x = 0.0f;
    m_pos[2].m_y = 0.0f;
    m_pos[2].m_z = 0.0f;
    m_phase[2] = 0.0f;
    m_node[3] = 0;
    m_pos[3].m_x = 0.0f;
    m_pos[3].m_y = 0.0f;
    m_pos[3].m_z = 0.0f;
    m_phase[3] = 0.0f;
    m_node[4] = 0;
    m_pos[4].m_x = 0.0f;
    m_pos[4].m_y = 0.0f;
    m_pos[4].m_z = 0.0f;
    m_phase[4] = 0.0f;
    m_posLeft.m_x = 0.0f;
    m_posLeft.m_y = 0.0f;
    m_posLeft.m_z = 0.0f;
    m_posRight.m_x = 0.0f;
    m_posRight.m_y = 0.0f;
    m_posRight.m_z = 0.0f;
    m_swing = 0.0f;
    m_swingSpeed = 0.1f;
    m_landTimer = 0.0f;
    m_joint = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 5;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[1].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[2].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[3].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[4].m_flags |= 0x10;
}

grEarthBranch* grEarthBranch::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthBranch* ground = new (Heaps::StageInstance) grEarthBranch(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthBranch::~grEarthBranch() {
}

bool grEarthBranch::setNode() {
    grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "tsutaJoint10");
    getNodeIndex(&m_node[1], 0, "tsutaJoint08");
    getNodeIndex(&m_node[2], 0, "tsutaJoint02");
    getNodeIndex(&m_node[3], 0, "tsutaJoint04");
    getNodeIndex(&m_node[4], 0, "tsutaJoint06");
    return false;
}

void grEarthBranch::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The branch swings after a fighter landed on it: the node that is the nearest to the fighter is the center of the wave and
// the others follow it. The swing goes down at first and up when the center has gone through the middle.
void grEarthBranch::updateActive(float deltaFrame) {
    m_landTimer = m_landTimer - deltaFrame;
    if (m_landTimer < 0.0f) {
        m_landTimer = 0.0f;
    }
    int center = 2;
    float best = 0.0f;
    if (__fabsf(getPosY(0)) > best) {
        center = 0;
        best = __fabsf(getPosY(0));
    }
    if (__fabsf(getPosY(1)) > best) {
        center = 1;
        best = __fabsf(getPosY(1));
    }
    if (__fabsf(getPosY(2)) > best) {
        center = 2;
        best = __fabsf(getPosY(2));
    }
    if (__fabsf(getPosY(3)) > best) {
        center = 3;
        best = __fabsf(getPosY(3));
    }
    if (__fabsf(getPosY(4)) > best) {
        center = 4;
        best = __fabsf(getPosY(4));
    }
    switch (m_state) {
    case 2:
        m_swing = m_swing * (1.0f - deltaFrame * 0.25f);
        updateCalcPos(deltaFrame);
        m_swing = m_swing + m_swingSpeed * deltaFrame;
        if (0.0f < m_pos[center].m_y) {
            m_state = 3;
            m_swingSpeed = m_swingSpeed * 0.85f;
        }
        break;
    case 3:
        m_swing = m_swing * (1.0f - deltaFrame * 0.25f);
        updateCalcPos(deltaFrame);
        m_swing = m_swing - m_swingSpeed * deltaFrame;
        if (m_pos[center].m_y < 0.0f) {
            m_state = 2;
            m_swingSpeed = m_swingSpeed * 0.85f;
        }
        break;
    }
}

void grEarthBranch::updateCalcPos(float deltaFrame) {
    for (u8 i = 0; i < 5; i++) {
        float cosine = nw4r::math::CosFIdx(static_cast<float>(static_cast<s16>(static_cast<int>(16384.0f * m_phase[i]))) * (1.0f / 256.0f));
        m_pos[i].m_y = m_pos[i].m_y + deltaFrame * (m_swing * cosine);
    }
    translateCollision();
}

void grEarthBranch::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_node[2];
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_node[3];
                calcWorldCallBack->m_nodeCallbackDatas[2].m_nodeIndex = m_node[4];
                calcWorldCallBack->m_nodeCallbackDatas[3].m_nodeIndex = m_node[1];
                calcWorldCallBack->m_nodeCallbackDatas[4].m_nodeIndex = m_node[0];
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data[0].m_offsetPos = m_pos[2];
            data[1].m_offsetPos = m_pos[3];
            data[2].m_offsetPos = m_pos[4];
            data[3].m_offsetPos = m_pos[1];
            data[4].m_offsetPos = m_pos[0];
        }
    }
}

// The fighter that lands on the branch starts the swing: the node that is the nearest to the fighter is the center of the
// wave, and the nodes get the part of the wave by how far they are from it.
void grEarthBranch::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    m_joint = collisionJoint;
    m_landTimer = 5.0f;
    if (isFirstContact == true) {
        for (int i = 0; i < 5; i++) {
            m_pos[i].m_x = 0.0f;
            m_pos[i].m_y = 0.0f;
            m_pos[i].m_z = 0.0f;
            m_phase[i] = 0.0f;
        }
        Vec2f fighterPos;
        collStatus->getPos(&fighterPos);
        Vec3f nodePos[5];
        for (int i = 0; i < 5; i++) {
            getNodePosition(&nodePos[i], 0, m_node[i]);
        }
        u32 nearest = 0xFFFFFFFF;
        float best = 1000000.0f;
        for (int i = 0; i < 5; i++) {
            if (__fabsf(fighterPos.m_x - nodePos[i].m_x) < best) {
                nearest = i;
                best = __fabsf(fighterPos.m_x - nodePos[i].m_x);
            }
        }
        if (nearest != 0xFFFFFFFF) {
            m_posLeft = nodePos[0];
            m_posLeft.m_x = nodePos[0].m_x - 15.0f;
            m_posRight = nodePos[4];
            m_posRight.m_x = nodePos[4].m_x + 15.0f;
            float origin = nodePos[nearest].m_x;
            for (u8 i = 0; i < 5; i++) {
                if (nearest == i) {
                    m_phase[i] = 0.0f;
                } else if (static_cast<int>(nearest) < static_cast<int>(i)) {
                    m_phase[i] = (nodePos[i].m_x - nodePos[nearest].m_x) / ((nodePos[4].m_x + 15.0f) - origin);
                } else if (static_cast<int>(i) < static_cast<int>(nearest)) {
                    m_phase[i] = (nodePos[i].m_x - nodePos[nearest].m_x) / ((nodePos[0].m_x - 15.0f) - origin);
                }
            }
            m_swing = lbl_27_data_547E0 * -3.5f;
            m_state = 2;
            m_swingSpeed = 0.25f;
        }
    }
}

// The line of the collision follows the nodes: the joint is turned into a line of five points that are as high as the nodes
// of the branch.
void grEarthBranch::translateCollision() {
    grCollisionJoint* joint = m_joint;
    if (joint != NULL) {
        // MATCH-ONLY: the bits 0x30000 of the flags are set on the joint (the other bits of the byte are cleared)
        u32* flags = reinterpret_cast<u32*>(reinterpret_cast<u8*>(joint) + 0x48);
        *flags = (*flags & 0xFF00FFFF) | 0x30000;
        if (joint->m_vtxDatas != NULL && joint->m_lines != NULL) {
            // The first vertex is the one of the left end of the line, the others are moved with the nodes.
            float top = 0.0f;
            u32 count = joint->m_vtxLen;
            int index = *reinterpret_cast<s16*>(joint);
            for (u32 i = 0; i != count; i++) {
                grCollisionLine* line = joint->getLine(index);
                if (line == NULL) {
                    return;
                }
                u16 vtx = *reinterpret_cast<u16*>(line);
                float* vtxPos = reinterpret_cast<float*>(reinterpret_cast<u8*>(joint->m_vtxDatas) + vtx * 8);
                if (i == 0) {
                    top = vtxPos[1];
                } else {
                    vtxPos[1] = top + m_pos[i - 1].m_y;
                }
                index = reinterpret_cast<s16*>(line)[3];
                if (index < 0) {
                    return;
                }
            }
        }
    }
}
