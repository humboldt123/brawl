// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/dekakuribo/em_dekakuribo.h>
#include <so/so_value_accesser.h>
#include <st/st_trigger.h>

// The Giant Goomba: an Enemy without articles of its own (the shared null mediator). Like the Goomba it overrides
// notifyEventLink, but a fighter jumping on it only deals a fixed damage.

emDekakuribo::emDekakuribo(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(28, 0),
    m_areaInstances(10, 0),
    m_cameraSubjects(1, 0) { }

emDekakuribo::~emDekakuribo() { }

// A fighter treading on the Giant Goomba bounces off it (jump speed multiplier: parameter 0xFA2) and deals the damage of
// parameter 0xFA4. The Giant Goomba then enters status 7 when the damage defeated it, else 0xE on the ground or 0xF in
// the air.
// HYPOTHESIS: addDamageHp returns true when the damage took the last of the strength; 7 is the defeat status, 0xE and
// 0xF the trodden statuses (ground, air).
void emDekakuribo::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* acc, StageObject* sender, int unk4) {
    switch (eventInfo->m_eventKind) {
    case LinkEvent_Tread: {
        emTreadLinkEventArgs* tread = static_cast<emTreadLinkEventArgs*>(eventInfo);
        tread->_0x4 = true;
        tread->m_keepTreadJumpCount = true;
        tread->m_handled = false;
        tread->m_node = -1;
        tread->m_pos.m_z = soValueAccesser::getValueFloat(&moduleAccesser, 0xfa2, 0);
        emDamageModuleImpl& damageModule = dynamic_cast<emDamageModuleImpl&>(*moduleAccesser.m_enumerationStart->m_damageModule);
        if (g_stTriggerMng != NULL) {
            fn_27_23313C(m_createId, sender->m_taskId, m_taskId, sender->m_taskCategory);
        }
        float damage = soValueAccesser::getValueFloat(&moduleAccesser, 0xfa4, 0);
        if (damageModule.addDamageHp(&damage, 0) == true) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(7, m_moduleAccesser);
        } else if (m_moduleAccesser->getSituationModule().getKind() == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
        } else {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0xf, m_moduleAccesser);
        }
        return;
    }
    }
    Enemy::notifyEventLink(eventInfo, acc, sender, unk4);
}
