// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/bombhead/em_bombhead.h>
#include <em/em_shot.h>

// MATCH-ONLY: names the wnemSwHolder pool's getInstanceAt first, so its weak copy comes out before the wnemSimple
// pool's as in the original (see SO_ARTICLE_MEDIATOR_POOL_1_FIRST in em/em_article_mediator.h).
SO_ARTICLE_MEDIATOR_POOL_1_FIRST(emBombheadArticleMediator)

// The Bombhead (Bombed): an Enemy with one wnemSimple and one wnemSwHolder article. The class overrides only its
// destructor; its own code is the two activators of its articles. Everything else in this unit is the module storage
// and the template instances of em/em_article_mediator.h (with one instance per pool, the instance holders'
// constructors are inlined into the Bombhead's).

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// HYPOTHESIS: the launch angle (degrees) of the wnemSwHolder shot, left in the work floats by the Bombhead's AI or
// status code.
static const u32 WorkFloat_ShotAngle = 0x11000004;

// ---- construction ---------------------------------------------------------------------------------------------------

emBombhead::emBombhead(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(29, 0),
    m_areaInstances(9, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emBombhead::~emBombhead() { }

// ---- the activators -------------------------------------------------------------------------------------------------

// The wnemSwHolder shot is thrown at the angle the shot work float holds.
bool emBombhead::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    emWeaponUtil::calcWeaponStartVector_AimingTarget(param, moduleAccesser, true,
            moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotAngle));
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

bool emBombhead::activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeapon(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}
