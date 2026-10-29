// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/ridley/em_ridley.h>

// Ridley: an Enemy without articles of its own (the shared null mediator) that owns three wind areas. The class
// overrides only its destructor; its behavior is the shared Enemy code driven by its parameters, statuses and anim
// cmds. (The four wnemGalleomProc functions in front of this unit's carve belong to the Galleom's weapon procs.)

emRidley::emRidley(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), &m_areaWinds, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(115, 0),
    m_areaInstances(11, 0),
    m_cameraSubjects(1, 0),
    m_areaWinds(3, 0) { }

emRidley::~emRidley() { }
