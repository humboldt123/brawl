// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/metaridley/em_metaridley.h>
#include <em/em_shot.h>
#include <st/stage.h>

// Meta Ridley (the boss): an Enemy with twenty wnemSwHolder articles. Besides its destructor the class overrides two
// event handlers: a status change that makes its body intangible on some stages, and a link event that raises an AI
// flag. Its articles get an activator of their own. Everything else in this unit is the module storage and the template
// instances of em/em_article_mediator.h (the twenty soInstancePoolSub levels of the pool).

// ---- construction ---------------------------------------------------------------------------------------------------

emMetaridley::emMetaridley(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(106, 0),
    m_areaInstances(8, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emMetaridley::~emMetaridley() { }

// ---- events ---------------------------------------------------------------------------------------------------------

// HYPOTHESIS: hit status 2 is the intangible one (0 normal, 1 invincible, as in the fighters' body collision states).
static const int HitStatus_Intangible = 2;

void emMetaridley::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                           soModuleAccesser* moduleAccesser) {
    Enemy::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    // After every status change, while Stage's flag at 0x19F is set, the whole body of Meta Ridley stays intangible.
    if (g_Stage->unk19F == 1) {
        moduleAccesser->getCollisionHitModule().setWhole(HitStatus_Intangible, 0);
    }
}

// ---- the activator --------------------------------------------------------------------------------------------------
// (The original places it between the two event handlers.)

// Shot kind 2 flies along the forward vector of the shot's model node; the others along the shared start vector.
bool emMetaridley::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    if (param->m_shotKind == 2) {
        emWeaponUtil::calcWeaponForwordVector(lbl_41_bss_504.m_nodeId, param, moduleAccesser, true, 0);
    } else {
        emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    }
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    return false;
}

// ---- events (continued) ---------------------------------------------------------------------------------------------

void emMetaridley::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject,
                                   int unk4) {
    switch (eventInfo->m_eventKind) {
    // HYPOTHESIS: as for the boomerang Primid's 0x13fe, a linked object reports back; AI unit 0x13 gets flag 1 and the
    // event is consumed.
    case 0x13ff:
        m_aiModule.setFlag(1, 0x13);
        eventInfo->_0x4 = true;
        break;
    }
    Enemy::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
}
