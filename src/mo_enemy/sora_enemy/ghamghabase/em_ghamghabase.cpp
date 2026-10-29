// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/ghamghabase/em_ghamghabase.h>

// The Ghamgha base: an Enemy with no articles and no overrides of its own besides its destructor, so this unit only
// holds the module storage the Enemy constructor is handed (as for the Roada).

emGhamghabase::emGhamghabase(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(19, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0) { }

emGhamghabase::~emGhamghabase() { }
