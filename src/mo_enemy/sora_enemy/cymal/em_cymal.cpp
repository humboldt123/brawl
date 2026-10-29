// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/cymal/em_cymal.h>

// The Cymal: an Enemy with no articles. Its reflector remembers that it reflected something: the reflect event sets a
// work flag, and the reflector check reports a hit while the flag is set as well as when the Enemy's own check does.

// HYPOTHESIS: name; the flag is cleared by the constructor and Enemy::init and set only here (the Boss Packun uses the
// same id for its own purpose).
static const u32 WorkFlag_Reflected = 0x12000006;

emCymal::emCymal(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(14, 0),
    m_areaInstances(8, 0),
    m_cameraSubjects(1, 0) {
    m_moduleAccesser->getWorkManageModule().offFlag(WorkFlag_Reflected);
}

emCymal::~emCymal() { }

void emCymal::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog,
                                            int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    Enemy::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
    m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_Reflected);
}

bool emCymal::notifyEventCollisionReflectorCheck() {
    bool reflected = false;
    if (Enemy::notifyEventCollisionReflectorCheck() == true) {
        reflected = true;
    }
    if (m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Reflected) == true) {
        reflected = true;
    }
    return reflected;
}
