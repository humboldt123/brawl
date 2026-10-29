// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/robohoming/em_robohoming.h>
#include <em/em_shot.h>

// The Robohoming: an Enemy with five wnemSwHolder articles. The class overrides only its destructor; its own code is
// the activator of its articles (the Bombhead's wnemSimple activator, for a wnemSwHolder). Everything else in this unit
// is the module storage and the template instances of em/em_article_mediator.h.
// HYPOTHESIS: the homing itself is the wnemSwHolder's weapon code driven by the shot description, not this unit.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emRobohoming::emRobohoming(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(54, 0),
    m_areaInstances(8, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emRobohoming::~emRobohoming() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emRobohoming::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}
