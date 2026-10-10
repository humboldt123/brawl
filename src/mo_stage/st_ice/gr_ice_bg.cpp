#include <memory.h>
#include <types.h>

#include <st_ice/gr_ice.h>

grIceBg* grIceBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceBg* ground = new (Heaps::StageInstance) grIceBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceBg::~grIceBg() {
}

// The places of the clouds, the ice blocks and the water are the places of their nodes.
void grIceBg::processAnim() {
    Ground::processAnim();
    if (m_mtxGimmickWork != NULL) {
        getNodeMatrix(&m_mtxGimmickWork[0], 0, "P_Kumoyuka1_a");
        getNodeMatrix(&m_mtxGimmickWork[1], 0, "P_Kumoyuka1_b");
        getNodeMatrix(&m_mtxGimmickWork[2], 0, "P_Kumoyuka2_a");
        getNodeMatrix(&m_mtxGimmickWork[3], 0, "P_Kumoyuka2_b");
        getNodeMatrix(&m_mtxGimmickWork[4], 0, "P_kaimen_a");
        getNodeMatrix(&m_mtxGimmickWork[5], 0, "P_kaimen_b");
    }
    if (m_posFishWork != NULL) {
        getNodePosition(&m_posFishWork[0], 0, "P_Fish_a");
        getNodePosition(&m_posFishWork[1], 0, "P_Fish_b");
    }
}

// The collision of the model is on only on the part of the mountain where the fighters can walk on it.
void grIceBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        u8 state = *m_stateWork;
        if (state == 3) {
            if (m_isEnableCollisionStatus == 1) {
                setEnableCollisionStatus(0);
            }
        } else if (state < 3) {
            if (state == 1) {
                if (m_isEnableCollisionStatus == 1) {
                    setEnableCollisionStatus(0);
                }
            } else if (state != 0 && m_isEnableCollisionStatus == 0) {
                setEnableCollisionStatus(1);
            }
        } else if (state < 5 && m_isEnableCollisionStatus == 1) {
            setEnableCollisionStatus(0);
        }
    }
}
