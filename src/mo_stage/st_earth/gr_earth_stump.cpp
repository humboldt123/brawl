#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <types.h>

#include <st_earth/gr_earth.h>

grEarthStump* grEarthStump::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthStump* ground = new (Heaps::StageInstance) grEarthStump(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthStump::~grEarthStump() {
}

// The places of the nodes of the stump are given to the stage (the pellets that fall on the stump use them).
void grEarthStump::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        getNodePosition(&m_posGimmickWork[2], 0, 1);
        m_posGimmickWork[2].m_x = m_posGimmickWork[2].m_x + 15.0f;
        getNodePosition(&m_posGimmickWork[1], 0, 2);
        getNodePosition(&m_posGimmickWork[0], 0, 3);
    }
}

void grEarthStump::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    updateJoint();
    updatePellet();
}

// The joint of the collision that belongs to the stump is looked for once.
void grEarthStump::updateJoint() {
    if (m_joint == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u16 count = collision->m_jointLen;
            u32 index = 0;
            grCollisionJoint* joint = reinterpret_cast<grCollisionJoint*>(this);
            while (index != count) {
                joint = collision->getJoint(index);
                if (joint == NULL) {
                    return;
                }
                if (reinterpret_cast<Ground*>(joint->m_ground) == this && joint->_0x4E == 0
                    && joint->m_nodeIndex == getNodeIndex(0, "stageKirikabu")) {
                    break;
                }
                index++;
            }
            if (index != count) {
                m_joint = joint;
            }
        }
    }
}

// The pellets that lie on the stump (the leaf 3) follow the line between the two nodes of it.
void grEarthStump::updatePellet() {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (data != NULL) {
        u8 count = data->unk3C;
        for (u32 i = 0; i != count; i++) {
            if (m_pelletData[i].m_leaf == 3) {
                getPosPellet(m_pelletData[i].unk24, &m_pelletData[i].m_pos);
            }
        }
    }
}

// The place on the stump where the pellet lies: a line from the first to the second node of the stump, and the pellet is on it
// by the rate (0 - 1).
void grEarthStump::getPosPellet(float rate, Vec3f* pos) {
    if (pos != NULL) {
        pos->m_x = 0.0f;
        pos->m_y = 0.0f;
        pos->m_z = 0.0f;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (1.0f < rate) {
            rate = 1.0f;
        }
        Vec3f start;
        Vec3f end;
        getNodePosition(&start, 0, "taibokuPelletP1");
        getNodePosition(&end, 0, "taibokuPelletP2");
        Vec3f dir;
        dir.m_x = end.m_x - start.m_x;
        dir.m_y = end.m_y - start.m_y;
        dir.m_z = end.m_z - start.m_z;
        float length = grEarthVecLength(&dir);
        float distance = rate * length;
        dir.normalize();
        start.m_z = start.m_z + dir.m_z * distance;
        pos->m_z = start.m_z;
        pos->m_x = start.m_x + dir.m_x * distance;
        pos->m_y = start.m_y + dir.m_y * distance;
        pos->m_z = start.m_z - 10.0f;
    }
}

void grEarthStump::setAILowPriority(u8 flag) {
    if (m_joint == NULL) {
        return;
    }
    m_joint->m_0x55_3 = flag & 1;
}

void grEarthStump::setCollisionAttr(u8 material) {
    if (m_joint != NULL) {
        u16 count = *reinterpret_cast<u16*>(reinterpret_cast<u8*>(m_joint) + 2);
        for (u32 i = 0; i != count; i++) {
            grCollisionLine* line = m_joint->getLine(i);
            if (line != NULL) {
                line->m_materialType = static_cast<grCollisionLine::MaterialType>(material);
            }
        }
    }
}
