// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/hammerbros/em_hammerbros.h>

// The Hammer Bro: an Enemy with ten wnemSwHolder articles. The class overrides only its destructor, and its articles use
// the shared activation of the Enemy unit, so this unit holds no behavior of its own: the module storage, the article
// mediator and its pools (their code is the template instances of em/em_article_mediator.h).
// HYPOTHESIS: the throwing behavior is driven by the shared Enemy code from the Hammer Bro's parameters and AI data.

emHammerbros::emHammerbros(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(32, 0),
    m_areaInstances(8, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emHammerbros::~emHammerbros() { }
