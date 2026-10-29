#include <sora_adv_stage/gr_adventure_door.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>

grAdventureDoor::~grAdventureDoor() { }

void grAdventureDoor::setInitializeFlag() {
    if (g_stTriggerMng->getTriggerFlag(&m_data->m_validTrigger, true) == false) {
        if (m_data->m_validTrigger.m_isValid) {
            setValid(0);
        }
    } else if (!m_data->m_validTrigger.m_isValid) {
        setValid(0);
    }
}
