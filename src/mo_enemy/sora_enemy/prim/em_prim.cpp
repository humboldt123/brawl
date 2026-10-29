// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot, or its own
// declaration adds one and every later Weapon virtual (deactivateArticle ...) is called one slot too far.
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/prim/em_prim.h>
#include <em/em_shot.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>

// MATCH-ONLY: names the wnemSwHolder pool's getInstanceAt first, so its weak copy comes out before the wnemSimple
// pool's as in the original (see SO_ARTICLE_MEDIATOR_POOL_1_FIRST in em/em_article_mediator.h).
SO_ARTICLE_MEDIATOR_POOL_1_FIRST(emPrimArticleMediator)

// The Primid: an Enemy with two wnemSimple shots and six wnemSwHolder articles. The variants (metal, big, boomerang,
// fire, super scope, sword) share this class; only the boomerang Primid does extra work in the code below.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emPrim::emPrim(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(44, 0),
    m_areaInstances(17, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emPrim::~emPrim() { }

// ---- the activators -------------------------------------------------------------------------------------------------

bool emPrim::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Enemy& enemy = dynamic_cast<Enemy&>(*moduleAccesser->m_stageObject);
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    if (enemy.getEnemyKind() == Enemy_Prim_Boomerang) {
        if (param->m_shotKind == 1 || param->m_shotKind == 2) {
            lbl_41_bss_504.m_unk6 = false;
            emWeaponUtil::calcWeaponRotation(lbl_41_bss_504.m_nodeId, param, moduleAccesser, false, 0);
        }
    }
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// Fires the shot: the plain shot (kind 4) is launched as emWeaponUtil::activeSimpleWeapon launches it, with the
// extended resource of the Primid in place of the weapon's resource id.
bool emPrim::activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser) {
    // MATCH-ONLY: the task id declared ahead of everything else gives the original's register order (as in
    // emWeaponUtil::activeSimpleWeapon, whose launch the plain shot repeats with the extended resource).
    int founderTaskId;
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Enemy& enemy = dynamic_cast<Enemy&>(*moduleAccesser->m_stageObject);
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    if (param->m_shotKind == 4) {
        int ownerId = enemy.getExtendBrresResourceID();
        StageObject* owner = moduleAccesser->m_stageObject;
        soPostureModule& posture = moduleAccesser->getPostureModule();
        founderTaskId = owner->m_taskId;
        fn_41_3091C(article, &lbl_41_bss_504, founderTaskId, ownerId, 0, flag.m_value, &pos, posture.getLr(),
                    soExternalValueAccesser::getTeamNo(owner), param->m_speed, -1,
                    soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0), -1);
    } else {
        emWeaponUtil::activeSimpleWeapon(&pos, flag.m_value, article, param, moduleAccesser);
    }
    return false;
}

// ---- events ---------------------------------------------------------------------------------------------------------

void emPrim::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    if (m_enemyKind == Enemy_Prim_Boomerang) {
        switch (eventInfo->m_eventKind) {
        // HYPOTHESIS: the boomerang came back to the thrower; AI unit 0x4D gets flag 1 and the event is consumed.
        case 0x13fe:
            m_aiModule.setFlag(1, 0x4d);
            eventInfo->_0x4 = true;
            break;
        }
    }
    Enemy::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}
