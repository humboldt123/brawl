// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/arman/em_arman.h>
#include <em/em_shot.h>

// MATCH-ONLY: names the wnemSwHolder pool's getInstanceAt first, so its weak copy comes out before the wnemSimple
// pool's as in the original (see SO_ARTICLE_MEDIATOR_POOL_1_FIRST in em/em_article_mediator.h).
SO_ARTICLE_MEDIATOR_POOL_1_FIRST(emArmanArticleMediator)

// The Arman (Armank): an Enemy with one wnemSimple article (its arm, whose processes are wnemArmanProc in
// wnem_proc_arman.cpp) and six wnemSwHolder shots. Besides its destructor the class overrides notifyEventLink, through
// which its arm and its linked objects report back; each pool has an activator of its own. Everything else in this unit
// is the module storage and the template instances of em/em_article_mediator.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emArman::emArman(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(43, 0),
    m_areaInstances(15, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emArman::~emArman() { }

// ---- the activators -------------------------------------------------------------------------------------------------

// The wnemSwHolder shot is aimed at the target point the AI leaves in the shot work floats, as the Kokkon's.
bool emArman::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    Vec3f target;
    target.m_x = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetX);
    target.m_y = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetY);
    target.m_z = 0.0f;
    emWeaponUtil::calcWeaponStartVector_AimingTarget1(param, moduleAccesser, &pos, &target, 0);
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// The arm (wnemSimple) starts along the shared start vector.
bool emArman::activeArticle1(wnemSimple* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeapon(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// ---- events ---------------------------------------------------------------------------------------------------------

void emArman::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender,
                              int unk4) {
    switch (eventInfo->m_eventKind) {
    // HYPOTHESIS: the arm (a wnemSimple) reports a hit; the Arman takes over the arm's hit stop, and the event is
    // consumed.
    case 0x13f7: {
        int hitStopFrame = dynamic_cast<wnemSimple&>(*sender).m_moduleAccesser->getStopModule().getHitStopRealFrame();
        moduleAccesser->getStopModule().setHitStopFrame(hitStopFrame, true);
        eventInfo->_0x4 = true;
        break;
    }
    // HYPOTHESIS: as for the boomerang Primid's 0x13fe, a linked object reports back; AI unit 0x2D gets flag 1 and the
    // event is consumed.
    case 0x13ff:
        m_aiModule.setFlag(1, 0x2d);
        eventInfo->_0x4 = true;
        break;
    }
    Enemy::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}
