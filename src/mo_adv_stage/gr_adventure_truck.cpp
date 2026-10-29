#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_truck.h>

grAdventureTruck* grAdventureTruck::create(int mdlIndex, const char* taskName) {
    grAdventureTruck* ground = new (Heaps::StageInstance) grAdventureTruck(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureTruck::~grAdventureTruck() { }

void grAdventureTruck::setInitializeFlag() {
    if (g_stTriggerMng->getTriggerFlag(&m_data->m_validTrigger, true) == false) {
        if (m_data->m_validTrigger.m_isValid) {
            setValid(0);
        }
    } else if (!m_data->m_validTrigger.m_isValid) {
        setValid(0);
    }
}
