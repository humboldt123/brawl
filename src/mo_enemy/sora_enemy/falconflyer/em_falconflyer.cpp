// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/falconflyer/em_falconflyer.h>

// The Falcon Flyer: an Enemy without articles of its own (the shared null mediator). The class overrides only its
// destructor; its behavior is the shared Enemy code driven by its parameters, statuses and anim cmds. (The five
// wnemMetaridleyProc functions in front of this unit's carve belong to Meta Ridley's weapon procs.)

emFalconflyer::emFalconflyer(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(25, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0) { }

emFalconflyer::~emFalconflyer() { }
