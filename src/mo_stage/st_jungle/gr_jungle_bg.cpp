#include <memory.h>
#include <types.h>

#include <snd/snd_system.h>
#include <st_jungle/gr_jungle.h>

// (inline: the original has no constructor of its own)
inline grJungleBg::grJungleBg(const char* taskName) : grJungle(taskName), m_snd() {
    m_posGimmickWork = NULL;
    m_posLimitWork = NULL;
    m_frameLocWork = NULL;
    memset(m_node, 0, sizeof(m_node));
    m_seHandle[0] = -1;
    m_seHandle[1] = -1;
    m_seHandle[2] = -1;
    m_seHandle[3] = -1;
}

grJungleBg* grJungleBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleBg* ground = new (Heaps::StageInstance) grJungleBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleBg::~grJungleBg() {
}

// Publishes the places of the nodes to the stage (the fourth node does not exist).
void grJungleBg::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        getNodePosition(&m_posGimmickWork[0], 0, m_node[0]);
        getNodePosition(&m_posGimmickWork[1], 0, m_node[1]);
        getNodePosition(&m_posGimmickWork[2], 0, m_node[2]);
        getNodePosition(&m_posGimmickWork[4], 0, m_node[4]);
        getNodePosition(&m_posGimmickWork[5], 0, m_node[5]);
        getNodePosition(&m_posGimmickWork[6], 0, m_node[6]);
        getNodePosition(&m_posGimmickWork[7], 0, m_node[7]);
        getNodePosition(&m_posGimmickWork[8], 0, m_node[8]);
        getNodePosition(&m_posGimmickWork[9], 0, m_node[9]);
        getNodePosition(&m_posGimmickWork[10], 0, m_node[10]);
        getNodePosition(&m_posGimmickWork[11], 0, m_node[11]);
        getNodePosition(&m_posGimmickWork[12], 0, m_node[12]);
    }
}

void grJungleBg::update(float deltaFrame) {
    grJungle::update(deltaFrame);
    if (m_frameLocWork != NULL) {
        *m_frameLocWork = getMotionFrame(0);
    }
    updateSE();
}

// The sounds of the falls play while their nodes are on the screen (the main backdrop is the only one that plays them).
void grJungleBg::updateSE() {
    if (m_posGimmickWork == NULL) {
        Vec3f pos;
        pos.m_x = 0.0f;
        pos.m_y = 0.0f;
        pos.m_z = 0.0f;
        m_snd.setPos(&pos);
        getNodePosition(&pos, 0, "Takidamari");
        if (pos.m_y < m_posLimitWork[0].m_y + 50.0f && pos.m_y > m_posLimitWork[1].m_y - 50.0f) {
            if (m_seHandle[1] == -1) {
                m_seHandle[1] = m_snd.playSE(static_cast<SndID>(0x1B8B), 0, 0xF0, -1);
            }
        } else if (m_seHandle[1] != -1) {
            m_snd.stopSE(m_seHandle[1], 0xF0);
            m_seHandle[1] = -1;
        }
        getNodePosition(&pos, 0, "Taki3");
        if (pos.m_y > m_posLimitWork[1].m_y - 50.0f) {
            if (m_seHandle[1] == -1) {
                m_seHandle[1] = m_snd.playSE(static_cast<SndID>(0x1B8A), 0, 0xF0, -1);
            }
            if (m_seHandle[2] == -1) {
                m_seHandle[2] = m_snd.playSE(static_cast<SndID>(0x1B91), 0, 0xF0, -1);
            }
        } else {
            if (m_seHandle[1] != -1) {
                m_snd.stopSE(m_seHandle[1], 0xF0);
                m_seHandle[1] = -1;
            }
            if (m_seHandle[2] != -1) {
                m_snd.stopSE(m_seHandle[2], 0xF0);
                m_seHandle[2] = -1;
            }
        }
        if (getMotionFrame(0) >= 7700.0f) {
            if (m_seHandle[3] == -1) {
                g_sndSystem->playSE(static_cast<SndID>(0x1B92), 0, 0, 0, -1);
                m_seHandle[3] = 0;
            }
        } else {
            m_seHandle[3] = -1;
        }
    }
}

bool grJungleBg::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "LV01N");
    getNodeIndex(&m_node[1], 0, "LV02N");
    getNodeIndex(&m_node[2], 0, "LV03N");
    getNodeIndex(&m_node[4], 0, "LV05N");
    getNodeIndex(&m_node[5], 0, "LV06N");
    getNodeIndex(&m_node[6], 0, "LV07N");
    getNodeIndex(&m_node[7], 0, "LV08N");
    getNodeIndex(&m_node[8], 0, "LV09N");
    getNodeIndex(&m_node[9], 0, "LV10N");
    getNodeIndex(&m_node[10], 0, "LV11N");
    getNodeIndex(&m_node[11], 0, "LV12N");
    getNodeIndex(&m_node[12], 0, "LV13N");
    return result;
}
