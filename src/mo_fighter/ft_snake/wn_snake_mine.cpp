#define SO_EVENT_OBSERVER_CTOR_EXTERN // BYTES: the observer bases call the sora_melee constructor and initialize
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <ft/ft_kinetic_energy.h>
#include <wn/snake/wn_snake_mine.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <math.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_prng.h>
#include <so/so_value_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_impl.h>

// The float checks of this unit (defined after their first use, as in the original order).
bool mtIsNormal(float value);
bool mtIsZero(float value);
// HYPOTHESIS: vector forms of the checks (inline, both components).
inline bool mtIsNormal(const Vec2f& v) {
    return mtIsNormal(v.m_x) && mtIsNormal(v.m_y);
}
inline bool mtIsZero(const Vec2f& v) {
    bool result = false;
    if (mtIsZero(v.m_x) && mtIsZero(v.m_y)) result = true;
    return result;
}

// BYTES: the observer's (manage id, priority) constructor is expanded in this unit (only the Enemy header form is inline).
inline soCollisionSearchEventObserver::soCollisionSearchEventObserver(short manageId, s8 priority) :
    soEventObserver<soCollisionSearchEventObserver>(0x11) {
    initialize(manageId, priority);
}

wnSnakeMine::wnSnakeMine(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeMineData* data) :
    wnWeaponBuilder<wnSnakeMineModuleAccesserBuildConfig>(
        "SnakeMine", articleId, info,
        wnModuleAccesserBuildData(0x16, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    soCollisionSearchEventObserver(static_cast<soEventManager&>(m_moduleAccesser->getEventManageModule()).getManageId(), -1),
    soDamageEventObserver(static_cast<soEventManager&>(m_moduleAccesser->getEventManageModule()).getManageId(), -1),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_energyGround() {
    m_moduleAccesser->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u8*>(data->unk38)), 0);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGround, wnSnakeMine_Energy_Ground, soKineticEnergy::AttributeFlag(8), -1);
    wnKineticModuleImpl::setOutsideMovementGroundEnergyId(&m_moduleAccesser->getKineticModule(), wnSnakeMine_Energy_Ground);
    Weapon::deactivate(false);
}

// Snake brings the mine out (carried status): only its ground movement energy runs; powerMul scales the explosion.
void wnSnakeMine::activate(int founderTaskId, int etcResourceId, int team, float powerMul) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = etcResourceId;
    desc.unk8 = etcResourceId;
    desc.unkC = etcResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = position;
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getStatusModule().changeStatusForce(wnSnakeMine_Status_Carried, m_moduleAccesser);
    // The attack power multiplier of the explosion (read back when the mine goes off).
    m_moduleAccesser->getWorkManageModule().setFloat(powerMul, wnSnakeMine_WorkFloat_PowerMul);
    m_moduleAccesser->getKineticModule().unableEnergy(wnSnakeMine_Energy_Normal);
    m_moduleAccesser->getKineticModule().unableEnergy(wnSnakeMine_Energy_Rot);
    m_moduleAccesser->getKineticModule().enableEnergy(wnSnakeMine_Energy_Ground);
    m_moduleAccesser->getGroundModule().setCalcLineMove(false, 0);
}

// The mine is put down at the given position (on the ground depth): it arms itself when it lands on the ground
// (armed), otherwise it falls.
void wnSnakeMine::shoot(const Vec3f& position) {
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    Vec3f pos = position;
    pos.m_z = m_moduleAccesser->getGroundModule().getZ();
    m_moduleAccesser->getPostureModule().setPos(&pos);
    if (m_moduleAccesser->getGroundModule().attachGround(0) == true) {
        m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        m_moduleAccesser->getStatusModule().changeStatusRequest(wnSnakeMine_Status_Armed, m_moduleAccesser);
    } else {
        m_moduleAccesser->getSituationModule().setKind(Situation_Air, false);
        m_moduleAccesser->getStatusModule().changeStatusRequest(wnSnakeMine_Status_Fall, m_moduleAccesser);
    }
    m_moduleAccesser->getStageObject().updateNodeSRT();
}

// The proximity flag is cleared each frame before the collision search refreshes it.
void wnSnakeMine::processCollision() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == wnSnakeMine_Status_Armed) {
        m_moduleAccesser->getWorkManageModule().offFlag(wnSnakeMine_WorkFlag_TargetInRange);
    }
    StageObject::processCollision();
}

// A fighter, enemy, weapon or item (not a stage gimmick) in the search area of an armed mine sets the proximity flag.
void wnSnakeMine::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL) {
        StageObject* object = dynamic_cast<StageObject*>(gfTaskScheduler::getInstance()->getTask(collisionLog->m_taskId));
        // Stage gimmicks (yakumono) do not set the mine off.
        if (object == NULL || object->soGetKind() == StageObject_Yakumono) {
            return;
        }
        if (m_moduleAccesser->getStatusModule().getStatusKind() == wnSnakeMine_Status_Armed) {
            m_moduleAccesser->getWorkManageModule().onFlag(wnSnakeMine_WorkFlag_TargetInRange);
        }
    }
}

// An armed mine that saw a target in range detonates.
bool wnSnakeMine::notifyEventCollisionSearchCheck() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == wnSnakeMine_Status_Armed &&
        m_moduleAccesser->getWorkManageModule().isFlag(wnSnakeMine_WorkFlag_TargetInRange)) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(wnSnakeMine_Status_Explode, m_moduleAccesser);
        return true;
    }
    return false;
}

// While armed or exploding on the ground, the mine is tilted to lie flat on the floor below it; the model follows the
// posture, and an armed mine raises its node 1 (HYPOTHESIS: the trigger) by the float parameter 4000.
void wnSnakeMine::updateNodeSRT() {
    if ((m_moduleAccesser->getStatusModule().getStatusKind() == wnSnakeMine_Status_Armed || m_moduleAccesser->getStatusModule().getStatusKind() == wnSnakeMine_Status_Explode) &&
        m_moduleAccesser->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_DOWN, 0)) {
        Vec2f normal = m_moduleAccesser->getGroundModule().getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0);
        if (mtIsNormal(normal) == true && !mtIsZero(normal)) {
            float angle = atan2f(normal.m_x, normal.m_y) * -1.0f * 57.29578f;
            Vec3f rot(0.0f, 0.0f, angle);
            m_moduleAccesser->getPostureModule().setRot(&rot, 0);
        }
    }
    float scaleValue = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = scaleValue;
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rot = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
    if (m_moduleAccesser->getStatusModule().getStatusKind() == wnSnakeMine_Status_Armed) {
        float lift = soValueAccesser::getConstantFloat(m_moduleAccesser, wnSnakeMine_Param_TriggerLift, 0);
        Vec3f translate;
        translate.m_x = 0.0f;
        translate.m_y = lift;
        translate.m_z = 0.0f;
        m_moduleAccesser->getModelModule().setNodeTranslate(1, &translate);
    }
}

// Not an infinity or a NaN.
bool mtIsNormal(float value) {
    bool result = true;
    if (((*reinterpret_cast<u32*>(&value) >> 23) & 0xFF) == 0xFF) result = false;
    return result;
}

bool mtIsZero(float value) {
    bool result = false;
    double magnitude = __fabs(value);
    value = magnitude;
    if (value < 0.00001f) result = true;
    return result;
}

// Exploding applies the power multiplier Snake gave the mine and stops its spin; falling throws
// it with the speeds of the float parameters 4001-4003 and a random spin.
void wnSnakeMine::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == wnSnakeMine_Status_Explode) {
        m_moduleAccesser->getCollisionAttackModule().setPowerMulStatus(m_moduleAccesser->getWorkManageModule().getFloat(wnSnakeMine_WorkFloat_PowerMul));
        m_moduleAccesser->getKineticModule().unableEnergy(wnSnakeMine_Energy_Rot);
    } else if (statusKind == wnSnakeMine_Status_Fall) {
        soKineticEnergyRotNormal* rotEnergy = dynamic_cast<soKineticEnergyRotNormal*>(m_moduleAccesser->getKineticModule().getEnergy(wnSnakeMine_Energy_Rot));
        soKineticEnergyNormal* energy = dynamic_cast<soKineticEnergyNormal*>(m_moduleAccesser->getKineticModule().getEnergy(wnSnakeMine_Energy_Normal));
        if (rotEnergy != NULL && energy != NULL) {
            energy->m_speed = Vec2f(0.0f, soValueAccesser::getConstantFloat(m_moduleAccesser, wnSnakeMine_Param_FallSpeedY, 0));
            energy->m_accel = Vec2f(0.0f, soValueAccesser::getConstantFloat(m_moduleAccesser, wnSnakeMine_Param_FallAccelY, 0));
            energy->m_speedLimit = Vec2f(-1.0f, soValueAccesser::getConstantFloat(m_moduleAccesser, wnSnakeMine_Param_FallSpeedLimitY, 0));
            energy->enable();
            Vec3f spin(45.0f * randf(), 45.0f * randf(), 45.0f * randf());
            // BYTES: copied member by member (float moves; this unit's Vec3f assignment is the aggregate one).
            rotEnergy->m_rotSpeed.m_x = spin.m_x;
            rotEnergy->m_rotSpeed.m_y = spin.m_y;
            rotEnergy->m_rotSpeed.m_z = spin.m_z;
            rotEnergy->enable();
        }
    }
}

// Any hit sets the mine off.
void wnSnakeMine::notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser) {
    m_moduleAccesser->getStatusModule().changeStatusRequest(wnSnakeMine_Status_Explode, m_moduleAccesser);
}
