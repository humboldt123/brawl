// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/rayquaza/em_rayquaza.h>
#include <em/em_shot.h>

// The Rayquaza (boss): an Enemy with five wnemSwHolder articles. The class overrides only its destructor; its own code
// is the activator of its articles, which for shot kinds 0 and 3 moves the launch position to the target point the AI
// left in the work floats (the shot appears there instead of at a model node). Everything else in this unit is the
// module storage and the template instances of em/em_article_mediator.h.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emRayquaza::emRayquaza(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(76, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emRayquaza::~emRayquaza() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emRayquaza::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    // HYPOTHESIS: shot kinds 0 and 3 are the shots that appear at the target point (the AI's aim), not at the mouth.
    if (param->m_shotKind == 0 || param->m_shotKind == 3) {
        pos.m_x = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetX);
        pos.m_y = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetY);
    }
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}
