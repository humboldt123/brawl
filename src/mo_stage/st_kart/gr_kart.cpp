#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <memory.h>

#include <st_kart/gr_kart.h>

grKart::grKart(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grKart::~grKart() {
}

grKart* grKart::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grKart* ground = new (Heaps::StageInstance) grKart(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grKartBg::grKartBg(const char* taskName) : grKart(taskName) {
    m_stateAIWork = NULL;
    m_joint[0] = NULL;
    m_joint[1] = NULL;
    m_joint[2] = NULL;
    m_joint[3] = NULL;
}

grKartBg::~grKartBg() {
}

grKartBg* grKartBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grKartBg* ground = new (Heaps::StageInstance) grKartBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

void grKartBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateJoint(deltaFrame);
}

// Finds the joints of the road the first time they exist. Afterwards the stage decides which of them are solid: while it
// opens a side (8 or 9) the joints of the other parts are switched off.
void grKartBg::updateJoint(float deltaFrame) {
    if (m_joint[0] == NULL || m_joint[1] == NULL || m_joint[2] == NULL || m_joint[3] == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 nodeCenter;
            u32 nodeLeft;
            u32 nodeRight;
            u32 nodeLord;
            if (getNodeIndex(&nodeCenter, 0, "AshibaCN") && getNodeIndex(&nodeLeft, 0, "AshibaLN") &&
                getNodeIndex(&nodeRight, 0, "AshibaRN") && getNodeIndex(&nodeLord, 0, "lord")) {
                u32 jointNum = (u16)collision->m_jointLen;
                for (u32 i = 0; i != jointNum; i++) {
                    grCollisionJoint* joint = collision->getJoint(i);
                    if (joint == NULL) {
                        break;
                    }
                    if (joint->m_ground == this && joint->_0x4E == 0) {
                        u32 node = joint->m_nodeIndex;
                        if (nodeCenter == node) {
                            m_joint[0] = joint;
                        } else if (nodeLeft == node) {
                            m_joint[1] = joint;
                        } else if (nodeRight == node) {
                            m_joint[2] = joint;
                        } else if (nodeLord == node) {
                            m_joint[3] = joint;
                        }
                    }
                    if (m_joint[0] != NULL && m_joint[1] != NULL && m_joint[2] != NULL && m_joint[3] != NULL) {
                        return;
                    }
                }
            }
        }
    } else {
        switch (*m_stateAIWork) {
        case 9:
            m_joint[0]->m_0x55_3 = true;
            m_joint[1]->m_0x55_3 = true;
            m_joint[2]->m_0x55_3 = true;
            m_joint[3]->m_0x55_3 = false;
            break;
        case 8:
            m_joint[0]->m_0x55_3 = false;
            m_joint[1]->m_0x55_3 = false;
            m_joint[2]->m_0x55_3 = false;
            m_joint[3]->m_0x55_3 = true;
            break;
        default:
            m_joint[0]->m_0x55_3 = false;
            m_joint[1]->m_0x55_3 = false;
            m_joint[2]->m_0x55_3 = false;
            m_joint[3]->m_0x55_3 = false;
            break;
        }
    }
}
