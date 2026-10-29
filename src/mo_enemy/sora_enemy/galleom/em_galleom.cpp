// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/galleom/em_galleom.h>
#include <em/em_shot.h>

// The Galleom (boss): an Enemy with five wnemSwHolder articles. The class overrides only its destructor; its own code
// is the activator of its articles, the same as the Robohoming's (launch from the shot's start position along its
// start vector). Everything else in this unit is the module storage and the template instances of
// em/em_article_mediator.h. HYPOTHESIS: its attacks are driven by the AI and the anim cmds, not by this unit.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// ---- construction ---------------------------------------------------------------------------------------------------

emGalleom::emGalleom(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(92, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emGalleom::~emGalleom() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emGalleom::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}
