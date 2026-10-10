#include <memory.h>
#include <types.h>

#include <st_village/gr_village.h>

grVillageStage* grVillageStage::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageStage* ground = new (Heaps::StageInstance) grVillageStage(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageStage::~grVillageStage() {
}

// The places of the guests follow the nodes of the stage model.
void grVillageStage::processAnim() {
    Ground::processAnim();
    if (m_posGuestWork != NULL) {
        getNodePosition(&m_posGuestWork[0], 0, "transLeftChrA");
        getNodePosition(&m_posGuestWork[1], 0, "transLeftChrB");
        getNodePosition(&m_posGuestWork[2], 0, "transCenterChrA");
        getNodePosition(&m_posGuestWork[3], 0, "transCenterChrB");
        getNodePosition(&m_posGuestWork[4], 0, "transGreas");
        getNodePosition(&m_posGuestWork[5], 0, "transGreasCar");
        getNodePosition(&m_posGuestWork[6], 0, "transHatoNoSu");
        getNodePosition(&m_posGuestWork[7], 0, "transMaster");
        getNodePosition(&m_posGuestWork[8], 0, "transTotakeke");
    }
}
