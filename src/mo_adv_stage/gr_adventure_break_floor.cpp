#include <sora_adv_stage/gr_adventure_break_floor.h>
#include <sora_adv_stage/gr_adventure_common.h>

grAdventureBreakFloor* grAdventureBreakFloor::create(int mdlIndex, const char* taskName) {
    grAdventureBreakFloor* ground = new (Heaps::StageInstance) grAdventureBreakFloor(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureBreakFloor::~grAdventureBreakFloor() { }

void grAdventureBreakFloor::setInitializeFlag() {
    if (g_stTriggerMng->getTriggerFlag(&m_data->m_validTrigger, true) == false) {
        if (m_data->m_validTrigger.m_isValid) {
            setValid(0);
        }
    } else if (!m_data->m_validTrigger.m_isValid) {
        setValid(0);
    }
}

void grAdventureBreakFloor::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, reinterpret_cast<int*>(&sOwnerFlags)) && isFirstContact) {
        g_stTriggerMng->setTriggerFlag(&m_data->m_breakTrigger);
        m_isLanded = 1;
        m_timer = 0.0f;
    }
}
