#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba09::grJungleAshiba09(const char* taskName) : grJungleAshiba(taskName), m_snd() {
    m_node[0] = 0;
    m_node[1] = 0;
    m_node[2] = 0;
    m_node[3] = 0;
    m_node[4] = 0;
    m_seHandle = -1;
}

grJungleAshiba09* grJungleAshiba09::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba09* ground = new (Heaps::StageInstance) grJungleAshiba09(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba09::~grJungleAshiba09() {
}

void grJungleAshiba09::processAnim() {
    Ground::processAnim();
    if (m_isUpdate && m_posGimmickWork != NULL) {
        getNodePosition(m_posGimmickWork, 0, m_node[0]);
        getNodePosition(&m_posGimmickWork[1], 0, m_node[1]);
        getNodePosition(&m_posGimmickWork[2], 0, m_node[2]);
        getNodePosition(&m_posGimmickWork[3], 0, m_node[3]);
    }
}

void grJungleAshiba09::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    if (m_isUpdate) {
        updateSE();
    }
}

// The sound of the wheels plays while the big wheel is on the screen.
void grJungleAshiba09::updateSE() {
    Vec3f pos;
    getNodePosition(&pos, 0, m_node[4]);
    m_snd.setPos(&pos);
    if (pos.m_y < m_posLimitWork[0].m_y + 15.0f && pos.m_y > m_posLimitWork[1].m_y - 15.0f) {
        if (m_seHandle == -1) {
            m_seHandle = m_snd.playSE(static_cast<SndID>(0x1B8E), 0, 0x78, -1);
        }
    } else if (m_seHandle != -1) {
        m_snd.stopSE(m_seHandle, 0x78);
        m_seHandle = -1;
    }
}

bool grJungleAshiba09::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_node[0], 0, "L9wheel1N");
    getNodeIndex(&m_node[1], 0, "L9wheel2N");
    getNodeIndex(&m_node[2], 0, "L9wheel3N");
    getNodeIndex(&m_node[3], 0, "L9wheel4N");
    getNodeIndex(&m_node[4], 0, "L9BigWheel");
    return result;
}
