// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/spar/em_spar.h>
#include <em/em_shot.h>
#include <so/so_value_accesser.h>

// The Spar: an Enemy with three wnemSwHolder articles. Besides its module storage and its article mediator (the template
// instances of em/em_article_mediator.h), the class has one function of its own: the activation of its articles.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emSpar::emSpar(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(39, 0),
    m_areaInstances(10, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emSpar::~emSpar() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emSpar::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    if (param->m_lifeConstantId > ConstantId_CommonLast) {
        emShotLifeConstant* lifeConstant = static_cast<emShotLifeConstant*>(
            soValueAccesser::getConstantIndefinite(moduleAccesser, param->m_lifeConstantId, 0));
        param->m_life = Enemy::getValueFloat(&lifeConstant->m_life, moduleAccesser);
    }
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    if (param->m_shotKind == 0 || param->m_shotKind == 2) {
        Vec3f target;
        target.m_x = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetX);
        target.m_y = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetY);
        target.m_z = 0.0f;
        emWeaponUtil::calcWeaponStartVector_AimingTarget1(param, moduleAccesser, &pos, &target, 0);
    } else {
        emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    }
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}
