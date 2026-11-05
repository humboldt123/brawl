// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
// MATCH-ONLY: see the probe below.
template <class Mediator, class AssignTL, int N, bool Exists>
struct soArticleMediatorSlot;
#include <em/cataguard/em_cataguard.h>
#include <em/em_shot.h>

// MATCH-ONLY: the original's weak copies come out as the counting functor, then the getInstanceAt copies of pools 2,
// 1, 0. Declaring the soArticleMediatorSlot primary template before the mediator header puts the functor first, and
// the unused probe below, which names a member of the slots 2 and 1 while the file is parsed, moves their pools'
// copies before pool 0's (see SO_ARTICLE_MEDIATOR_POOL_1_FIRST in em/em_article_mediator.h). The probe is never emitted.
typedef soTypeList<emCataguardArticleAssignment,
                   soTypeList<emCataguardArticleAssignment, soTypeList<emCataguardArticleAssignment, soTypeListNullType> > >
    emCataguardAssignmentList;
static inline s32 emCataguardNameHighPoolsFirst() {
    s32 n = soArticleMediatorSlot<emCataguardArticleMediator, emCataguardAssignmentList, 2, true>::getGenerateMaxNum(
        SO_ARTICLE_SLOT_TAGS);
    return n + soArticleMediatorSlot<emCataguardArticleMediator, emCataguardAssignmentList, 1, true>::getGenerateMaxNum(
                   SO_ARTICLE_SLOT_TAGS);
}

// The Cataguard: an Enemy with three pools of wnemSwHolder articles. The class overrides only its destructor; its own
// code is the activator of its articles, which throws shots of kind 4 at the angle the shot work float holds. Everything
// else in this unit is the module storage and the template instances of em/em_article_mediator.h.

// The shared shot helpers of the enemy activators are declared in em/em_shot.h.

// HYPOTHESIS: the launch angle (degrees) of a thrown shot, left in the work floats by the AI or status code (the
// Bombhead reads the same float).
static const u32 WorkFloat_ShotAngle = 0x11000004;

// ---- construction ---------------------------------------------------------------------------------------------------

emCataguard::emCataguard(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(23, 0),
    m_areaInstances(12, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emCataguard::~emCataguard() { }

// ---- the activator --------------------------------------------------------------------------------------------------

// HYPOTHESIS: shot kind 4 is the thrown shot; the others fly from the launch position along the shot description.
bool emCataguard::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    if (param->m_shotKind == 4) {
        emWeaponUtil::calcWeaponStartVector_AimingTarget(param, moduleAccesser, true,
                moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotAngle));
    } else {
        // The launch position is computed a second time and dropped.
        emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    }
    emWeaponUtil::activeSimpleWeaponHolder(&pos, flag.m_value, article, param, moduleAccesser);
    if (param->m_shotKind == 4) {
        return true;
    }
    return param->m_shotKind == 0;
}
