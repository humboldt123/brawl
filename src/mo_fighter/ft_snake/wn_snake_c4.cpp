#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
#define WN_WEAPON_BUILDER
#define SO_KINETIC_ENERGY_ABSTRACT
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define SO_EVENT_OBSERVER_CTOR_EXTERN // BYTES: the search observer base calls the sora_melee constructor and initialize
// MATCH-ONLY: retain the native one-word null-input result (as in the Mario cape unit).
#define IP_INPUT_BUTTON_RAW
#define SO_COLLISION_SEARCH_OBSERVER_INLINE // BYTES: the observer subobject is built inline and its default check is weak here
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <types.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/snake/wn_snake_c4.h>
#include <wn/snake/wn_snake_c4_def.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <mt/mt_prng.h>
#include <so/so_slow.h>
#include <so/so_utility_lot_service.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <ft/ft_external_value_accesser.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/model/so_model_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_kinetic_energy_rot_normal.h>

// The status unique processes of the charge (wn_snake_c4_status_uniq_process_establish_*.cpp): sticking to the ground and
// to a wall.
extern soStatusUniqProcess g_wnSnakeC4StatusUniqProcessEstablishGround;
extern soStatusUniqProcess g_wnSnakeC4StatusUniqProcessEstablishWall;

extern "C" {
// wnSnakeC4Utility::detachConstraint (module map); outside any configured unit.
void fn_122_1AF04(soModuleAccesser*);
// wnSnakeC4Utility::stickStageRot (module map); outside any configured unit.
void fn_122_1B000(soModuleAccesser*);
} // extern "C"

using namespace wnSnakeC4Def;

wnSnakeC4::wnSnakeC4(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeC4Data* data) :
    wnWeaponBuilder<wnSnakeC4ModuleAccesserBuildConfig>(
        "SnakeC4", articleId, info,
        wnModuleAccesserBuildData(0x18, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)),
    soCollisionSearchEventObserver(ftGetManageId(m_moduleAccesser), -1), m_param(data->m_param),
    m_energyGravity(0.0f, -data->m_param->m_gravity, data->m_param->m_speedLimit), m_energyGroundMovement() {
    // Statuses 1 and 2 (stuck to the ground, stuck to a wall) have their own processes; the others use none.
    soStatusUniqProcess* uniqProcs[7] = {NULL};
    uniqProcs[Status_EstablishGround] = &g_wnSnakeC4StatusUniqProcessEstablishGround;
    uniqProcs[Status_EstablishWall] = &g_wnSnakeC4StatusUniqProcessEstablishWall;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 7);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGravity, Energy_Gravity, soKineticEnergy::AttributeFlag(1), -1);
    m_moduleAccesser->getKineticModule().addEnergy(&m_energyGroundMovement, Energy_GroundMovement, soKineticEnergy::AttributeFlag(8), -1);
    wnKineticModuleImpl::setOutsideMovementGroundEnergyId(&m_moduleAccesser->getKineticModule(), Energy_GroundMovement);
    Weapon::deactivate(false);
}

void wnSnakeC4::activate(int founderTaskId, int etcResourceId, int team, float lr) {
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
    desc.lr = lr;
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
    m_moduleAccesser->getWorkManageModule().setInt(m_param->m_life, WorkInt_Life);
    float roll = randf();
    if (m_param->m_blinkChance > 0.0f && 1.0f / m_param->m_blinkChance >= roll) {
        m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 1, true);
        m_moduleAccesser->getVisibilityModule().setDefaultAll();
    }
    m_moduleAccesser->getKineticModule().unableEnergy(Energy_Rot);
    m_moduleAccesser->getKineticModule().unableEnergy(Energy_Gravity);
    m_moduleAccesser->getKineticModule().enableEnergy(Energy_GroundMovement);
    m_moduleAccesser->getGroundModule().setCalcLineMove(false, 0);
    m_moduleAccesser->getGroundModule().setDetachChangeStatus(false);
}

void wnSnakeC4::shoot() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case Status_EstablishGround:
        if (m_moduleAccesser->getLinkModule().isLink(LinkNo_Parent) == 1) {
            if (static_cast<u32>(m_moduleAccesser->getLinkModule().getParentTaskId(LinkNo_Parent)) == m_moduleAccesser->getWorkManageModule().getInt(Weapon::Instance::Work::Int_Activate_Founder_Id)) {
                m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_Drop);
            }
        }
        break;
    case Status_EstablishWall:
        if (m_moduleAccesser->getLinkModule().isLink(LinkNo_Parent) == 1) {
            if (static_cast<u32>(m_moduleAccesser->getLinkModule().getParentTaskId(LinkNo_Parent)) == m_moduleAccesser->getWorkManageModule().getInt(Weapon::Instance::Work::Int_Activate_Founder_Id)) {
                m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_Drop);
            }
        }
        break;
    case Status_EstablishFighter:
    case Status_Landing:
        break;
    default:
        if (m_moduleAccesser->getLinkModule().isModelConstraint() == 1) {
            fn_122_1AF04(m_moduleAccesser);
        }
        m_moduleAccesser->getStatusModule().changeStatusForce(Status_Fly, m_moduleAccesser);
        break;
    }
}

bool wnSnakeC4::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

void wnSnakeC4::processUpdate() {
    // Count the charge's life down (not during hit stop); when it runs out the charge fizzles (status 6).
    if (soSlow::getInstance()->isEstimate() == true && m_moduleAccesser->getStopModule().isStop() == false &&
        m_moduleAccesser->getWorkManageModule().getInt(WorkInt_Life) > 0) {
        m_moduleAccesser->getWorkManageModule().decInt(WorkInt_Life);
        if (m_moduleAccesser->getWorkManageModule().getInt(WorkInt_Life) <= 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(Status_Disappear, m_moduleAccesser);
        }
    }
    // Stuck to a fighter (status 4): follow the node it is stuck to, and fall off with a chance that grows with the
    // damage that fighter takes.
    if (m_moduleAccesser->getStatusModule().getStatusKind() == Status_EstablishFighter && m_moduleAccesser->getLinkModule().isModelConstraint()) {
        Vec3f pos = m_moduleAccesser->getLinkModule().getModelConstraintTargetNodePosition();
        m_moduleAccesser->getPostureModule().setPos(&pos);
        float damage = m_moduleAccesser->getLinkModule().getParentDamagePowerMax(LinkNo_Parent);
        if (damage > 0.0f && soUtilityLotService::lotRate(damage / m_param->m_dropDamage)) {
            m_moduleAccesser->getStatusModule().changeStatusForce(Status_Fly, m_moduleAccesser);
        }
    }
    Weapon::processUpdate();
}

void wnSnakeC4::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case Status_EstablishGround:
        // Stuck to the ground: the stick test reaches this far below the charge.
        m_moduleAccesser->getWorkManageModule().setFloat(m_param->m_groundStickRange, WorkFloat_StickRange);
        m_moduleAccesser->getWorkManageModule().setInt(-1, WorkInt_TargetTaskId);
        break;
    case Status_EstablishWall:
        // Stuck to a wall: the stick test reaches this far to the side.
        m_moduleAccesser->getWorkManageModule().setFloat(m_param->m_wallStickRange, WorkFloat_StickRange);
        m_moduleAccesser->getWorkManageModule().setInt(-1, WorkInt_TargetTaskId);
        break;
    case Status_Landing:
        // Landed after a throw: lie on the stage.
        switch (prevStatusKind) {
        case Status_Fly:
            if (m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
                fn_122_1AF04(m_moduleAccesser);
            }
            if (m_moduleAccesser->getGroundModule().isTouch(8, 0) == true) {
                // HYPOTHESIS: the stage position of the charge is computed and left unused.
                Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
                pos.m_z = m_moduleAccesser->getGroundModule().getZ();
                fn_122_1B000(m_moduleAccesser);
                m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
                m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
            }
            break;
        }
        break;
    case Status_Fly: {
        // Thrown (or knocked off a fighter): fall with the launch speed and tumble.
        switch (prevStatusKind) {
        case Status_EstablishFighter:
            if (m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
                fn_122_1AF04(m_moduleAccesser);
            }
            if (m_moduleAccesser->getLinkModule().isLink(LinkNo_Parent) == true) {
                m_moduleAccesser->getLinkModule().unlink(LinkNo_Parent);
            }
            break;
        }
        soKineticEnergyRotNormal* rot = dynamic_cast<soKineticEnergyRotNormal*>(m_moduleAccesser->getKineticModule().getEnergy(Energy_Rot));
        wnKineticEnergyGravity* gravity = dynamic_cast<wnKineticEnergyGravity*>(m_moduleAccesser->getKineticModule().getEnergy(Energy_Gravity));
        if (rot != NULL && gravity != NULL) {
            gravity->m_speedY = m_param->m_throwSpeedY;
            gravity->enable();
            Vec3f rotSpeed(randf() * 45.0f, randf() * 45.0f, randf() * 45.0f);
            // BYTES: the rotation speed is copied float by float (the unit's Vec3f assignment copies words).
            rot->m_rotSpeed.m_x = rotSpeed.m_x;
            rot->m_rotSpeed.m_y = rotSpeed.m_y;
            rot->m_rotSpeed.m_z = rotSpeed.m_z;
            rot->enable();
        }
        break;
    }
    case Status_Have:
    case Status_EstablishFighter:
        m_moduleAccesser->getWorkManageModule().setInt(-1, WorkInt_TargetTaskId);
        break;
    case Status_Disappear:
    case Status_Explode:
        // Exploding or fizzling: stop riding the ground.
        m_moduleAccesser->getKineticModule().unableEnergy(Energy_GroundMovement);
        break;
    }
}

void wnSnakeC4::updateNodeSRT() {
    float scale = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale3;
    scale3.m_x = scale3.m_y = scale3.m_z = scale;
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rot = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale3, &rot, &pos);
    // Whether the charge lies stuck on the stage (stuck to the ground or a wall once the stick is done, or landed).
    bool isStuck = false;
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case Status_Landing:
        isStuck = true;
        break;
    case Status_EstablishGround:
        if (m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Established)) {
            isStuck = true;
        }
        break;
    case Status_EstablishWall:
        if (m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Established)) {
            isStuck = true;
        }
        break;
    }
    if (isStuck == true) {
        // HYPOTHESIS: the stage rotation is read and left unused.
        Vec3f stageRot = m_moduleAccesser->getPostureModule().getRot(1);
    }
}

void wnSnakeC4::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 2:
        // The fighter the charge is stuck to (link 3) went away: drop off it.
        if (unk4 == LinkNo_Parent && m_moduleAccesser->getStatusModule().getStatusKind() == Status_EstablishFighter) {
            m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_Established);
            m_moduleAccesser->getStatusModule().changeStatusRequest(Status_Fly, m_moduleAccesser);
        }
        break;
    case 0x3D:
        if (unk4 == LinkNo_Parent && m_moduleAccesser->getStatusModule().getStatusKind() == Status_EstablishFighter &&
            m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Established) == false) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(Status_Fly, m_moduleAccesser);
        }
        break;
    case 0x38:
        if (unk4 == LinkNo_Parent && m_moduleAccesser->getStatusModule().getStatusKind() == Status_EstablishFighter) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(Status_Fly, m_moduleAccesser);
        }
        break;
    case 0x37:
        m_moduleAccesser->getVisibilityModule().setWhole(static_cast<wnSnakeC4LinkEventVisibility*>(eventInfo)->m_visible);
        break;
    case 0x839:
        // HYPOTHESIS: the owner (the founder task) removes its articles: so does the charge when its parent is the owner.
        if (static_cast<u32>(m_moduleAccesser->getWorkManageModule().getInt(Weapon::Instance::Work::Int_Activate_Founder_Id)) ==
            m_moduleAccesser->getLinkModule().getParentTaskId(LinkNo_Parent)) {
            setNotifyArticleEventRemove(m_moduleAccesser);
            if (isActive() == true) {
                deactivate(false);
            }
        }
        break;
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

void wnSnakeC4::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog == NULL) {
        return;
    }
    // Only a charge that is not flying (statuses 1 to 4) picks up a fighter to stick to.
    if (m_moduleAccesser->getStatusModule().getStatusKind() != Status_EstablishGround && m_moduleAccesser->getStatusModule().getStatusKind() != Status_EstablishWall &&
        m_moduleAccesser->getStatusModule().getStatusKind() != Status_Have && m_moduleAccesser->getStatusModule().getStatusKind() != Status_EstablishFighter) {
        return;
    }
    // Not the fighter it is already stuck to.
    if (m_moduleAccesser->getLinkModule().isLink(LinkNo_Parent) == true &&
        collisionLog->m_taskId == static_cast<u32>(m_moduleAccesser->getLinkModule().getParentTaskId(LinkNo_Parent))) {
        return;
    }
    gfTask* task = gfTask::getTask(collisionLog->m_taskId);
    if (task == NULL) {
        return;
    }
    switch (task->m_taskCategory) {
    case gfTask::Category_Fighter:
        // Remember the fighter and the collision part that touched it; the status update sticks the charge there.
        m_moduleAccesser->getWorkManageModule().setInt(collisionLog->m_taskId, WorkInt_TargetTaskId);
        m_moduleAccesser->getWorkManageModule().setInt(collisionLog->m_collsionIndex, WorkInt_TargetHitNo);
        break;
    }
}

bool wnSnakeC4::notifyEventCollisionSearchCheck() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == Status_Have || m_moduleAccesser->getStatusModule().getStatusKind() == Status_EstablishGround ||
        m_moduleAccesser->getStatusModule().getStatusKind() == Status_EstablishWall || m_moduleAccesser->getStatusModule().getStatusKind() == Status_EstablishFighter) {
        // No fighter was found by the search.
        if (m_moduleAccesser->getWorkManageModule().getInt(WorkInt_TargetTaskId) == -1) {
            return false;
        }
        m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_Established);
        if (m_moduleAccesser->getLinkModule().isLink(LinkNo_Parent) == true) {
            m_moduleAccesser->getLinkModule().unlink(LinkNo_Parent);
        }
        // Stick to that fighter, on the node of the hurtbox it touched.
        if (m_moduleAccesser->getLinkModule().link(LinkNo_Parent, m_moduleAccesser->getWorkManageModule().getInt(WorkInt_TargetTaskId)) == false) {
            return false;
        }
        m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(
            LinkNo_Parent, 2, ftExternalValueAccesser::getNodeIdFromHitNo(m_moduleAccesser->getWorkManageModule().getInt(WorkInt_TargetHitNo)), 3, true);
        m_moduleAccesser->getLinkModule().setAttribute(LinkNo_Parent, soLinkConnection::Attribute(4), true);
        // The fighter can shake the charge off by mashing.
        reinterpret_cast<ftControllerClatterView*>(&m_moduleAccesser->getControllerModule())
            ->startClatter(static_cast<float>(m_param->m_clatterFrames) - static_cast<float>(m_param->m_clatterFramesBase), 1.0f,
                           m_param->m_clatterRate, 0, -1, 0, false);
        m_moduleAccesser->getStatusModule().changeStatusRequest(Status_EstablishFighter, m_moduleAccesser);
        m_moduleAccesser->getWorkManageModule().offFlag(WorkFlag_Established);
        return true;
    }
    return false;
}
