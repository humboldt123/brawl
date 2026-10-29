// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/killer/em_killer.h>

// The Killer: an Enemy with no articles and no overrides of its own besides its destructor. Unlike the other small
// enemies it owns a reflect module (kind 0) and hands it to the Enemy constructor instead of the shared null one.

emKiller::emKiller(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          &m_reflectModule),
    m_interpolations(19, 0),
    m_areaInstances(9, 0),
    m_cameraSubjects(1, 0),
    m_reflectModule(&moduleAccesser, 0) { }

emKiller::~emKiller() { }
