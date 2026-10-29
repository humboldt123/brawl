// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/robodistance/em_robodistance.h>

// The Robodistance: an Enemy with one wnemSwHolder article. The class overrides only its destructor, and its article
// uses the shared activation of the Enemy unit, so this unit holds no behavior of its own: the module storage, the
// article mediator and its pool (their code is the template instances of em/em_article_mediator.h; with one instance
// in the pool, the instance holder's constructor is inlined into the Robodistance's, as for the Bombhead).
// HYPOTHESIS: the shot is fired by the shared Enemy code from the Robodistance's parameters and AI data.

emRobodistance::emRobodistance(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(54, 0),
    m_areaInstances(12, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emRobodistance::~emRobodistance() { }
