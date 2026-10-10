#include <gf/gf_model.h>
#include <memory.h>
#include <string.h>
#include <types.h>

#include <st_norfair/gr_norfair.h>

grNorfairBg* grNorfairBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grNorfairBg* ground = new (Heaps::StageInstance) grNorfairBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grNorfairBg::~grNorfairBg() {
}

// The stage gets the places of the zones and of the platforms of the trainer from the nodes of the background.
void grNorfairBg::processAnim() {
    Ground::processAnim();
    if (m_posZoneWork != NULL) {
        getNodePosition(&m_posZoneWork[0], 0, m_nodeZone[0]);
        getNodePosition(&m_posZoneWork[1], 0, m_nodeZone[1]);
        getNodePosition(&m_posZoneWork[2], 0, m_nodeZone[2]);
        getNodePosition(&m_posZoneWork[3], 0, m_nodeZone[3]);
        getNodePosition(&m_posZoneWork[4], 0, m_nodeZone[4]);
    }
    if (m_posAshibaTWork != NULL) {
        getNodePosition(&m_posAshibaTWork[0], 0, m_nodeAshibaT[0]);
        getNodePosition(&m_posAshibaTWork[1], 0, m_nodeAshibaT[1]);
        getNodePosition(&m_posAshibaTWork[2], 0, m_nodeAshibaT[2]);
        getNodePosition(&m_posAshibaTWork[3], 0, m_nodeAshibaT[3]);
    }
}

bool grNorfairBg::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeZone[0], 0, "L_Safety_asiba_a");
    getNodeIndex(&m_nodeZone[1], 0, "L_Safety_asiba_b");
    getNodeIndex(&m_nodeZone[2], 0, "L_Safety_asiba_c");
    getNodeIndex(&m_nodeZone[3], 0, "L_Safety_asiba_d");
    getNodeIndex(&m_nodeZone[4], 0, "L_Safety_asiba_e");
    getNodeIndex(&m_nodeAshibaT[0], 0, "L_asiba_trainer_1");
    getNodeIndex(&m_nodeAshibaT[1], 0, "L_asiba_trainer_2");
    getNodeIndex(&m_nodeAshibaT[2], 0, "L_asiba_trainer_3");
    getNodeIndex(&m_nodeAshibaT[3], 0, "L_asiba_trainer_4");
    return result;
}
