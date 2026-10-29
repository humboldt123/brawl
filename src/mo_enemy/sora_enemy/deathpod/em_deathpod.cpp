// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/deathpod/em_deathpod.h>
#include <em/em_shot.h>

// The Deathpod: an Enemy with six wnemSwHolder articles. The class overrides only its destructor; its own code is the
// activator of its articles, which aims each shot from one of two launch modes chosen by the shot work. Everything else
// in this unit is the module storage and the template instances of em/em_article_mediator.h.

// HYPOTHESIS: the model node of the Deathpod's second launch point (the shoot anim cmd names it in the shot work).
static const u32 Deathpod_ShotNodeAlt = 11;

// ---- construction ---------------------------------------------------------------------------------------------------

emDeathpod::emDeathpod(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(16, 0),
    m_areaInstances(10, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emDeathpod::~emDeathpod() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emDeathpod::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    // The shot flies along a model vector of its node: the one selected by 3 from the second launch point, by 2 otherwise.
    u32 nodeId = lbl_41_bss_504.m_nodeId;
    if (nodeId == Deathpod_ShotNodeAlt) {
        emWeaponUtil::calcWeaponForwordVector(nodeId, param, moduleAccesser, true, 3);
    } else {
        emWeaponUtil::calcWeaponForwordVector(nodeId, param, moduleAccesser, true, 2);
    }
    emWeaponUtil::activeSimpleWeaponHolder(&pos, false, article, param, moduleAccesser);
    return false;
}
