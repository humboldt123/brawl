// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/gyraan/em_gyraan.h>
#include <so/stop/so_stop_module_impl.h>

// The Gyraan: an Enemy with two wnemSimple articles (its axes, see wnemGyraanProc). Its own code is the link event
// handler; everything else in this unit is the module storage and the template instances of em/em_article_mediator.h.

// HYPOTHESIS: event kinds named from the handler's reaction. 0x13F7 comes from one of the Gyraan's own wnemSimple
// articles (the sender is cast to one); 0x1403, sent by a stuck axe (wnemGyraanProc::AxeStick), sets AI flag 0x18.
static const int LinkEvent_ArticleHitStop = 0x13f7;
static const int LinkEvent_AxeStuck = 0x1403;

// ---- construction ---------------------------------------------------------------------------------------------------

emGyraan::emGyraan(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(22, 0),
    m_areaInstances(9, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) {
    m_isAxeStuck = false;
}

emGyraan::~emGyraan() { }

// ---- events ---------------------------------------------------------------------------------------------------------

void emGyraan::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    switch (eventInfo->m_eventKind) {
    // The Gyraan shares the hit stop of its article.
    case LinkEvent_ArticleHitStop: {
        wnemSimple& article = dynamic_cast<wnemSimple&>(*sender);
        s32 hitStopFrame = article.m_moduleAccesser->getStopModule().getHitStopRealFrame();
        moduleAccesser->getStopModule().setHitStopFrame(hitStopFrame, 1);
        eventInfo->_0x4 = true;
        break;
    }
    case LinkEvent_AxeStuck:
        m_aiModule.setFlag(1, 0x18);
        eventInfo->_0x4 = true;
        m_isAxeStuck = true;
        break;
    }
    Enemy::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}
