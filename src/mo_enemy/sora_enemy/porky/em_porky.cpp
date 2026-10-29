// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
// MATCH-ONLY: the Vec3f subtraction in updateNodeSRT allocates its four registers in ascending order (f0..f3).
#define MT_VEC3F_SUB_ASCENDING
#include <em/porky/em_porky.h>
#include <em/em_shot.h>
#include <em/em_weapon_manager.h>
#include <mt/mt_trig.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>
#include <wn/wn_em_simple.h>

// The Porky (boss): an Enemy with seven wnemSwHolder articles. Besides its destructor the class overrides
// updateNodeSRT, which keeps one model node on a weapon of its own while it is in two of its statuses; its articles get
// an activator of their own that launches each shot itself (wnemSwHolder::active) instead of through the shared
// emWeaponUtil::activeSimpleWeaponHolder. Everything else in this unit is the module storage and the template instances
// of em/em_article_mediator.h.

// HYPOTHESIS: the Porky statuses during which its weapon drags the model node along (no status names are known).
static const int Porky_Status_WeaponHold0 = 0x20;
static const int Porky_Status_WeaponHold1 = 0x25;
// HYPOTHESIS: the kinds (wnemSimple 0x2210) of the Porky weapons that node follows.
static const u32 Porky_WeaponKindHeld0 = 0x2c;
static const u32 Porky_WeaponKindHeld1 = 0x2d;
// The model node whose translation follows the weapon, and the node it is measured from.
static const u32 Porky_Node_WeaponTranslate = 0x22;
static const u32 Porky_Node_WeaponBase = 0x21;
// HYPOTHESIS: the shot node from which a random-angle shot flies to the left.
static const u32 Porky_ShotNodeMirrored = 13;

// ---- construction ---------------------------------------------------------------------------------------------------

emPorky::emPorky(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(61, 0),
    m_areaInstances(9, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emPorky::~emPorky() { }

// ---- the activator --------------------------------------------------------------------------------------------------

// Shot kind 0 is thrown at a random speed and angle like the Popperam's (to the left from the mirrored node, to the
// right otherwise), kind 2 is aimed at the AI's target point, the others take the basic launch vector. The activator
// then clears the weapon node's translation and launches the article itself, facing the enemy (or the throw side).
bool emPorky::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    float lr = moduleAccesser->getPostureModule().getLr();
    if (param->m_shotKind == 0) {
        if (lbl_41_bss_504.m_nodeId == Porky_ShotNodeMirrored) {
            lr = -1.0f;
        } else {
            lr = 1.0f;
        }
        // The direction of the shot (cos, sin) in the xy plane.
        Vec3f dir;
        mtSinCosf(0.017453292f * Enemy::getValueRndFloat(param->m_angleRange, moduleAccesser), &dir.m_y, &dir.m_x);
        float speed = Enemy::getValueRndFloat(param->m_speedRange, moduleAccesser);
        lbl_41_bss_504.m_speedX = dir.m_x * speed;
        lbl_41_bss_504.m_speedY = dir.m_y * speed;
        lbl_41_bss_504.m_accelX = dir.m_x * param->m_accel;
        lbl_41_bss_504.m_accelY = dir.m_y * param->m_accel;
        lbl_41_bss_504.m_speedX *= lr;
        lbl_41_bss_504.m_accelX *= lr;
    } else if (param->m_shotKind == 2) {
        Vec3f target;
        target.m_x = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetX);
        target.m_y = moduleAccesser->getWorkManageModule().getFloat(WorkFloat_ShotTargetY);
        target.m_z = 0.0f;
        emWeaponUtil::calcWeaponStartVector_AimingTarget1(param, moduleAccesser, &pos, &target, 0);
    } else {
        emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    }
    Vec3f zero(0.0f, 0.0f, 0.0f);
    moduleAccesser->getModelModule().setNodeTranslate(Porky_Node_WeaponTranslate, &zero);
    // MATCH-ONLY: the task id and the owner declared ahead of the Enemy reference give the original's register order
    // (r27 / r26 / r25); the original also evaluates the call's arguments right to left (life, resource id, team).
    int founderTaskId;
    StageObject* owner;
    Enemy& enemy = dynamic_cast<Enemy&>(*moduleAccesser->m_stageObject);
    owner = moduleAccesser->m_stageObject;
    founderTaskId = owner->m_taskId;
    u32 life = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0);
    article->active(&pos, lr, soExternalValueAccesser::getTeamNo(owner), -1, founderTaskId,
                    enemy.getSimpleWeaponResourceID(param->m_weaponId), &lbl_41_bss_504, param->m_speed, flag.m_value,
                    life);
    return false;
}

// ---- model nodes ----------------------------------------------------------------------------------------------------

// In the two weapon statuses, the weapon node is moved onto the Porky's managed weapon of one of two kinds (its
// translation is the weapon's position relative to the base node), before the shared node update.
void emPorky::updateNodeSRT() {
    int status = moduleAccesser.getStatusModule().getStatusKind();
    if (status == Porky_Status_WeaponHold0 || status == Porky_Status_WeaponHold1) {
        // MATCH-ONLY: the manager declared before the owner task id keeps the id read before the singleton lookup
        // (TIPS.md, section 6, item 1).
        emWeaponManager* manager;
        int taskId = moduleAccesser.getStageObject().m_taskId;
        manager = emWeaponManager::getInstance();
        wnemSimple* weapon = manager->findWeapon(taskId, 0xffff, 0);
        if (weapon != NULL) {
            u32 kind = weapon->m_weaponKind;
            // One of the two held kinds (an unsigned range check; the two-sided compare gives two branches).
            if (kind - Porky_WeaponKindHeld0 <= Porky_WeaponKindHeld1 - Porky_WeaponKindHeld0) {
                Vec3f basePos(0.0f, 0.0f, 0.0f);
                Vec3f weaponPos(0.0f, 0.0f, 0.0f);
                Vec3f translate(0.0f, 0.0f, 0.0f);
                weaponPos = weapon->m_moduleAccesser->getPostureModule().getPos();
                basePos = moduleAccesser.getModelModule().getNodeGlobalPosition(Porky_Node_WeaponBase, false);
                translate = weaponPos - basePos;
                moduleAccesser.getModelModule().setNodeTranslate(Porky_Node_WeaponTranslate, &translate);
            }
        }
    }
    Enemy::updateNodeSRT();
}
