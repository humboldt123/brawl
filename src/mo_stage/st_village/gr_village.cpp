#include <memory.h>
#include <types.h>

#include <st_village/gr_village.h>

grVillage::grVillage(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_sceneWork = NULL;
    m_sceneBit = 0;
    m_stateWork = NULL;
    m_posGuestWork = NULL;
    setupMelee();
}

grVillage* grVillage::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillage* ground = new (Heaps::StageInstance) grVillage(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillage::~grVillage() {
}

void grVillage::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateVisible(deltaFrame);
    }
}

// The ground is shown only in the times of day whose bit is set in the bit mask (no mask = always shown).
void grVillage::updateVisible(float deltaFrame) {
    if (m_sceneWork == NULL) {
        return;
    }
    if (m_sceneBit == 0) {
        return;
    }
    if (m_sceneBit & (1 << *m_sceneWork)) {
        setVisibility(1);
    } else {
        setVisibility(0);
    }
}

bool grVillage::isSceneBit() {
    if (m_sceneWork == NULL) {
        return false;
    }
    if (m_sceneBit == 0) {
        return true;
    }
    return (m_sceneBit & (1 << *m_sceneWork)) != 0;
}
