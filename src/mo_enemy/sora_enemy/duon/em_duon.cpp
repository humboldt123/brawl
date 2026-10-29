// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/duon/em_duon.h>
#include <em/em_shot.h>

// The Duon (boss): an Enemy with fourteen wnemSwHolder articles. The class overrides only its destructor; its own code
// is the activator of its articles, which picks the launch direction of each shot from the model node the shoot anim
// cmd fires it from. Everything else in this unit is the module storage and the template instances of
// em/em_article_mediator.h.

// HYPOTHESIS: the launch nodes of the Duon's shots, grouped by the model vector the shot flies along. The node ids are
// the original's constants; which gun or launcher each one is has not been checked against the model.
// Nodes whose shots fly along model vector 0, first group.
static const u32 Duon_ShotNodeA0 = 0x1a;
static const u32 Duon_ShotNodeA1 = 0x24;
static const u32 Duon_ShotNodeA2 = 0x31;
static const u32 Duon_ShotNodeA3 = 0x39;
// The node whose shots fly along model vector 4.
static const u32 Duon_ShotNodeVector4 = 0x15;
// Nodes whose shots fly along model vector 0, second group.
static const u32 Duon_ShotNodeB0 = 0x18;
static const u32 Duon_ShotNodeB1 = 0x22;
static const u32 Duon_ShotNodeB2 = 0x2f;
// The node whose shots fly along model vector 5.
static const u32 Duon_ShotNodeVector5 = 0x1f;
// The node whose shots are aimed at the target point the AI left in the work floats.
static const u32 Duon_ShotNodeAimed = 0x10;

// ---- construction ---------------------------------------------------------------------------------------------------

emDuon::emDuon(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(65, 0),
    m_areaInstances(10, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emDuon::~emDuon() { }

// ---- the activator --------------------------------------------------------------------------------------------------

// Shot kinds 2 and 4 take the basic launch vector; the others fly along a model vector of their launch node, or at the
// AI's target point from the aimed node. Unlike the shared activator, the flag of the shot description is not passed
// on (always false).
bool emDuon::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    if (param->m_shotKind == 2 || param->m_shotKind == 4) {
        emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    } else {
        u32 nodeId = lbl_41_bss_504.m_nodeId;
        if (nodeId == Duon_ShotNodeA0 || nodeId == Duon_ShotNodeA1 || nodeId == Duon_ShotNodeA2 ||
            nodeId == Duon_ShotNodeA3) {
            emWeaponUtil::calcWeaponForwordVector(nodeId, param, moduleAccesser, true, 0);
        } else if (nodeId == Duon_ShotNodeVector4) {
            emWeaponUtil::calcWeaponForwordVector(nodeId, param, moduleAccesser, true, 4);
        }
        if (nodeId == Duon_ShotNodeB0 || nodeId == Duon_ShotNodeB1 || nodeId == Duon_ShotNodeB2) {
            emWeaponUtil::calcWeaponForwordVector(nodeId, param, moduleAccesser, true, 0);
        } else if (nodeId == Duon_ShotNodeVector5) {
            emWeaponUtil::calcWeaponForwordVector(nodeId, param, moduleAccesser, true, 5);
        } else if (nodeId == Duon_ShotNodeAimed) {
            Vec3f target;
            target.m_x = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetX);
            target.m_y = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetY);
            target.m_z = 0.0f;
            emWeaponUtil::calcWeaponStartVector_AimingTarget1(param, moduleAccesser, &pos, &target, 0);
        }
    }
    emWeaponUtil::activeSimpleWeaponHolder(&pos, false, article, param, moduleAccesser);
    return false;
}
