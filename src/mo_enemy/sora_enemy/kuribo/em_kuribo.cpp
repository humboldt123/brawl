// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/kuribo/em_kuribo.h>
#include <so/so_value_accesser.h>
#include <st/st_trigger.h>

// The Goomba: an Enemy without articles of its own (the shared null mediator). Besides its destructor the class
// overrides notifyEventLink: a fighter jumping on it defeats it outright.

// The current strength of the enemy (see enemy.cpp, Enemy::initStrength).
static const u32 WorkFloat_Strength = 0x11000001;

emKuribo::emKuribo(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(33, 0),
    m_areaInstances(9, 0),
    m_cameraSubjects(1, 0) { }

emKuribo::~emKuribo() { }

// A fighter treading on the Goomba bounces off it (jump speed multiplier: parameter 0xFA0) and the Goomba takes damage
// equal to its whole strength, then enters status 0xF.
// HYPOTHESIS: status 0xF is the Goomba's trodden (flattened) status.
void emKuribo::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* acc, StageObject* sender, int unk4) {
    switch (eventInfo->m_eventKind) {
    case LinkEvent_Tread: {
        emTreadLinkEventArgs* tread = static_cast<emTreadLinkEventArgs*>(eventInfo);
        tread->_0x4 = true;
        tread->m_keepTreadJumpCount = true;
        tread->m_handled = false;
        tread->m_node = -1;
        tread->m_pos.m_z = soValueAccesser::getValueFloat(&moduleAccesser, 0xfa0, 0);
        if (g_stTriggerMng != NULL) {
            fn_27_23313C(m_createId, sender->m_taskId, m_taskId, sender->m_taskCategory);
        }
        float damage = moduleAccesser.getWorkManageModule().getFloat(WorkFloat_Strength);
        dynamic_cast<emDamageModuleImpl&>(*moduleAccesser.m_enumerationStart->m_damageModule).addDamageHp(&damage, 0);
        m_moduleAccesser->getStatusModule().changeStatusRequest(0xf, m_moduleAccesser);
        return;
    }
    }
    Enemy::notifyEventLink(eventInfo, acc, sender, unk4);
}
