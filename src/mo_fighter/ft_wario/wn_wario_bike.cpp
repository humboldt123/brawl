#define SO_BUILDER_HELPERS_WEAK
#define IP_INPUT_BUTTON_RAW
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: this unit copies Vec3f as an aggregate (word moves), see mt_vector.h
// MATCH-ONLY: retain native vector assignment calls.
#define MT_VEC2F_ASSIGN_NOINLINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_KINETIC_GENERIC_BUILDER
#define WN_DAMAGE_ACTOR_INTERFACE
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/wario/wn_wario_bike_kinetic_transactor.h>
#include <wn/wario/wn_wario_bike.h>
#include <so/stop/so_stop_module_impl.h>
#include <ft/ft_entry.h>
#include <it/item.h>
#include <gf/gf_task_scheduler.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/so_external_value_accesser.h>
#include <wn/wn_kinetic_transactor.h>
#include <wn/wn_activate_desc.h>
#include <wn/wario/wn_wario_bike_link_event.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <wn/wario/wn_wario_bike_status_uniq_process.h>
#include <it/it_manager.h>

wnWarioBikeParamAccesser::wnWarioBikeParamAccesser(const wnWarioBikeParam* param) :
    wnExtendParamAccesser<3999, 34, 23999, 5, 0, 0>(0x60) {
    const u8* p = reinterpret_cast<const u8*>(param);
    // HYPOTHESIS: the float table follows the parameter block's offsets (0x00-0x28 by 4, 0x30-0x6C by 4, then 0x78, 0x84-0x98).
    // BYTES: the original stores each table entry directly (no loop).
    m_floats[0] = reinterpret_cast<const float*>(p + 0x0);
    m_floats[1] = reinterpret_cast<const float*>(p + 0x4);
    m_floats[2] = reinterpret_cast<const float*>(p + 0x8);
    m_floats[3] = reinterpret_cast<const float*>(p + 0xC);
    m_floats[4] = reinterpret_cast<const float*>(p + 0x10);
    m_floats[5] = reinterpret_cast<const float*>(p + 0x14);
    m_floats[6] = reinterpret_cast<const float*>(p + 0x18);
    m_floats[7] = reinterpret_cast<const float*>(p + 0x1C);
    m_floats[8] = reinterpret_cast<const float*>(p + 0x20);
    m_floats[9] = reinterpret_cast<const float*>(p + 0x24);
    m_floats[10] = reinterpret_cast<const float*>(p + 0x28);
    m_floats[11] = reinterpret_cast<const float*>(p + 0x30);
    m_floats[12] = reinterpret_cast<const float*>(p + 0x34);
    m_floats[13] = reinterpret_cast<const float*>(p + 0x38);
    m_floats[14] = reinterpret_cast<const float*>(p + 0x3C);
    m_floats[15] = reinterpret_cast<const float*>(p + 0x40);
    m_floats[16] = reinterpret_cast<const float*>(p + 0x44);
    m_floats[17] = reinterpret_cast<const float*>(p + 0x48);
    m_floats[18] = reinterpret_cast<const float*>(p + 0x4C);
    m_floats[19] = reinterpret_cast<const float*>(p + 0x50);
    m_floats[20] = reinterpret_cast<const float*>(p + 0x54);
    m_floats[21] = reinterpret_cast<const float*>(p + 0x58);
    m_floats[22] = reinterpret_cast<const float*>(p + 0x5C);
    m_floats[23] = reinterpret_cast<const float*>(p + 0x60);
    m_floats[24] = reinterpret_cast<const float*>(p + 0x64);
    m_floats[25] = reinterpret_cast<const float*>(p + 0x68);
    m_floats[26] = reinterpret_cast<const float*>(p + 0x6C);
    m_floats[27] = reinterpret_cast<const float*>(p + 0x78);
    m_floats[28] = reinterpret_cast<const float*>(p + 0x84);
    m_floats[29] = reinterpret_cast<const float*>(p + 0x88);
    m_floats[30] = reinterpret_cast<const float*>(p + 0x8C);
    m_floats[31] = reinterpret_cast<const float*>(p + 0x90);
    m_floats[32] = reinterpret_cast<const float*>(p + 0x94);
    m_floats[33] = reinterpret_cast<const float*>(p + 0x98);
    m_ints[0] = reinterpret_cast<const s32*>(p + 0x2C);
    m_ints[1] = reinterpret_cast<const s32*>(p + 0x70);
    m_ints[2] = reinterpret_cast<const s32*>(p + 0x74);
    m_ints[3] = reinterpret_cast<const s32*>(p + 0x7C);
    m_ints[4] = reinterpret_cast<const s32*>(p + 0x80);
}

#pragma dont_inline on // BYTES: the native constructor calls its helpers out of line
wnWarioBike::wnWarioBike(s32 articleId, const wnArticleBuildInfo& info, const wnWarioBikeData* data) :
    wnWeaponBuilder<wnWarioBikeModuleAccesserBuildConfig>(
        "WarioBike", articleId, info,
        wnModuleAccesserBuildData(0x60, data, info.getHeapModule()->getInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(), info.getHeapModule()->getNWMotionInstanceHeap(),
                                  data->m_resGroupNo, -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(const_cast<wnWarioBikeParam*>(data->m_param)), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[19] = {};
    uniqProcs[0] = wnWarioBikeStatusUniqProcessStart::getInstance();
    uniqProcs[1] = wnWarioBikeStatusUniqProcessStart::getInstance();
    uniqProcs[2] = wnWarioBikeStatusUniqProcessDrive::getInstance();
    uniqProcs[3] = wnWarioBikeStatusUniqProcessWheelie::getInstance();
    uniqProcs[4] = wnWarioBikeStatusUniqProcessBump::getInstance();
    uniqProcs[5] = wnWarioBikeStatusUniqProcessTurnStart::getInstance();
    uniqProcs[6] = wnWarioBikeStatusUniqProcessTurnLoop::getInstance();
    uniqProcs[7] = wnWarioBikeStatusUniqProcessTurnEnd::getInstance();
    uniqProcs[8] = wnWarioBikeStatusUniqProcessTurnDown::getInstance();
    uniqProcs[9] = wnWarioBikeStatusUniqProcessUtility::getInstance();
    uniqProcs[10] = wnWarioBikeStatusUniqProcessEscape::getInstance();
    uniqProcs[11] = wnWarioBikeStatusUniqProcessDown::getInstance();
    uniqProcs[13] = wnWarioBikeStatusUniqProcessItem::getInstance();
    uniqProcs[14] = wnWarioBikeStatusUniqProcessAppeal::getInstance();
    getModuleAccesser()->getStatusModule().addRangeUniqProc(uniqProcs, 19);
    getModuleAccesser()->getCollisionHitModule().add((soSet<soCollisionHitData::Simple>*)data->m_hitData, 0);
    if (!itManager::getInstance()->isCompItemKindArchive(static_cast<itKind>(0x5C), 0, true))
        itManager::getInstance()->preloadItemKindArchive(static_cast<itKind>(0x5C), 0, static_cast<itArchive::Type>(0), true);
    Weapon::deactivate(false);
}
#pragma dont_inline off

void wnWarioBikeKineticTransactor::changeKinetic(
    int kineticType, wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    if (kineticType <= 31) {
        wnKineticTransactor::changeKinetic(kineticType, pools, accesser);
        return;
    }
    if (kineticType > 40) return;

    wnWarioBikeKineticTransactor self = {};
    switch (kineticType) {
    case 32: self.changeKineticSub(pools, accesser); break;
    case 33: self.changeKineticSub1(pools, accesser); break;
    case 34: self.changeKineticSub2(pools, accesser); break;
    case 35: self.changeKineticSub3(pools, accesser); break;
    case 36: self.changeKineticSub4(pools, accesser); break;
    case 37: self.changeKineticSub5(pools, accesser); break;
    case 38: self.changeKineticSub6(pools, accesser); break;
    case 39: self.changeKineticSub7(pools, accesser); break;
    case 40: self.changeKineticSub8(pools, accesser); break;
    }
}

void wnWarioBikeKineticTransactor::changeKineticSub(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* energy =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    float brake = soValueAccesser::getConstantFloat(accesser, 4004, 0);
    float target = soValueAccesser::getConstantFloat(accesser, 4003, 0);
    Vec2f zero(0.0f, 0.0f);
    Vec2f brakeVector(brake, 0.0f);
    Vec2f targetVector(target, 0.0f);
    Vec2f::copy(energy->m_accel, zero);
    Vec2f::copy(energy->m_brake, brakeVector);
    Vec2f::copy(energy->m_speedTarget, targetVector);
    Vec2f::copy(energy->m_speedLimit, targetVector);
    energy->enable();
}

void wnWarioBikeKineticTransactor::changeKineticSub1(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* normal =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    wnKineticEnergyGravity* gravity =
        static_cast<wnWarioBikeGravityPool&>(*pools).getSub().getInstanceAt(0);
    Vec2f speed = accesser->getKineticModule().getSumSpeed(
        soKineticEnergy::AttributeFlag(1));
    float brake = soValueAccesser::getConstantFloat(accesser, 4007, 0);
    float gravityAcceleration = soValueAccesser::getConstantFloat(accesser, 4009, 0);
    float target = soValueAccesser::getConstantFloat(accesser, 4006, 0);
    float gravitySpeedLimit = soValueAccesser::getConstantFloat(accesser, 4010, 0);
    Vec2f zero(0.0f, 0.0f);
    Vec2f brakeVector(brake, 0.0f);
    Vec2f targetVector(target, 0.0f);
    Vec2f::copy(normal->m_speed, speed);
    Vec2f::copy(normal->m_accel, zero);
    Vec2f::copy(normal->m_brake, brakeVector);
    Vec2f::copy(normal->m_speedTarget, targetVector);
    Vec2f::copy(normal->m_speedLimit, targetVector);
    normal->enable();
    gravity->m_speedY = speed.m_x;
    gravity->m_gravity = -gravityAcceleration;
    gravity->m_speedLimit = gravitySpeedLimit;
    gravity->enable();
}

void wnWarioBikeKineticTransactor::changeKineticSub2(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* energy =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    float value = soValueAccesser::getConstantFloat(accesser, 4003, 0);
    Vec2f zero(0.0f, 0.0f);
    Vec2f paired(value, value);
    Vec2f::copy(energy->m_accel, zero);
    Vec2f::copy(energy->m_brake, zero);
    Vec2f::copy(energy->m_speedTarget, paired);
    Vec2f::copy(energy->m_speedLimit, paired);
    energy->enable();
}

void wnWarioBikeKineticTransactor::changeKineticSub3(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* energy =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    float value = soValueAccesser::getConstantFloat(accesser, 4027, 0);
    float limit = soValueAccesser::getConstantFloat(accesser, 4003, 0);
    Vec2f acceleration(-value, 0.0f);
    Vec2f brake(value, 0.0f);
    Vec2f zero(0.0f, 0.0f);
    Vec2f speedLimit(limit, 0.0f);
    Vec2f::copy(energy->m_accel, acceleration);
    Vec2f::copy(energy->m_brake, brake);
    Vec2f::copy(energy->m_speedTarget, zero);
    Vec2f::copy(energy->m_speedLimit, speedLimit);
    energy->enable();
}

void wnWarioBikeKineticTransactor::changeKineticSub4(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* energy =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    float brake = soValueAccesser::getConstantFloat(accesser, 4028, 0);
    float limit = soValueAccesser::getConstantFloat(accesser, 4003, 0);
    Vec2f zero(0.0f, 0.0f);
    Vec2f brakeVector(brake, 0.0f);
    Vec2f limitVector(limit, 0.0f);
    Vec2f::copy(energy->m_accel, zero);
    Vec2f::copy(energy->m_brake, brakeVector);
    Vec2f::copy(energy->m_speedTarget, zero);
    Vec2f::copy(energy->m_speedLimit, limitVector);
    energy->enable();
}

void wnWarioBikeKineticTransactor::changeKineticSub5(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* normal =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    wnKineticEnergyGravity* gravity =
        static_cast<wnWarioBikeGravityPool&>(*pools).getSub().getInstanceAt(0);
    Vec2f speed = accesser->getKineticModule().getSumSpeed(
        soKineticEnergy::AttributeFlag(1));
    float brake = soValueAccesser::getConstantFloat(accesser, 4029, 0);
    float gravityAcceleration = soValueAccesser::getConstantFloat(accesser, 4009, 0);
    // HYPOTHESIS: the native queries constant 4008 and discards its result (the function scores lower without the call).
    soValueAccesser::getConstantFloat(accesser, 4008, 0);
    float limit = soValueAccesser::getConstantFloat(accesser, 4006, 0);
    float gravitySpeedLimit = soValueAccesser::getConstantFloat(accesser, 4010, 0);
    Vec2f zero(0.0f, 0.0f);
    Vec2f brakeVector(brake, 0.0f);
    Vec2f limitVector(limit, 0.0f);
    Vec2f::copy(normal->m_speed, speed);
    Vec2f::copy(normal->m_accel, zero);
    Vec2f::copy(normal->m_brake, brakeVector);
    Vec2f::copy(normal->m_speedTarget, zero);
    Vec2f::copy(normal->m_speedLimit, limitVector);
    normal->enable();
    gravity->m_speedY = speed.m_x;
    gravity->m_gravity = -gravityAcceleration;
    gravity->m_speedLimit = gravitySpeedLimit;
    gravity->enable();
}

void wnWarioBikeKineticTransactor::changeKineticSub6(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* normal =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    Vec2f speed = accesser->getKineticModule().getSumSpeed(
        soKineticEnergy::AttributeFlag(1));
    float brake = soValueAccesser::getConstantFloat(accesser, 4028, 0);
    float target = soValueAccesser::getConstantFloat(accesser, 4003, 0);
    Vec2f zero(0.0f, 0.0f);
    Vec2f brakeVector(brake, 0.0f);
    Vec2f targetVector(target, 0.0f);
    Vec2f::copy(normal->m_speed, speed);
    Vec2f::copy(normal->m_accel, zero);
    Vec2f::copy(normal->m_brake, brakeVector);
    Vec2f::copy(normal->m_speedTarget, targetVector);
    Vec2f::copy(normal->m_speedLimit, targetVector);
    normal->enable();
}

void wnWarioBikeKineticTransactor::changeKineticSub7(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* normal =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    wnKineticEnergyGravity* gravity =
        static_cast<wnWarioBikeGravityPool&>(*pools).getSub().getInstanceAt(0);
    Vec2f speed = accesser->getKineticModule().getSumSpeed(
        soKineticEnergy::AttributeFlag(1));
    float brake = soValueAccesser::getConstantFloat(accesser, 4029, 0);
    float gravityAcceleration = soValueAccesser::getConstantFloat(accesser, 4009, 0);
    float limit = soValueAccesser::getConstantFloat(accesser, 4006, 0);
    float gravitySpeedLimit = soValueAccesser::getConstantFloat(accesser, 4010, 0);
    Vec2f zero(0.0f, 0.0f);
    Vec2f brakeVector(brake, 0.0f);
    Vec2f limitVector(limit, 0.0f);
    Vec2f::copy(normal->m_speed, speed);
    Vec2f::copy(normal->m_accel, zero);
    Vec2f::copy(normal->m_brake, brakeVector);
    Vec2f::copy(normal->m_speedTarget, zero);
    Vec2f::copy(normal->m_speedLimit, limitVector);
    normal->enable();
    gravity->m_speedY = speed.m_x;
    gravity->m_gravity = -gravityAcceleration;
    gravity->m_speedLimit = gravitySpeedLimit;
    gravity->enable();
}

void wnWarioBikeKineticTransactor::changeKineticSub8(
    wnWarioBikeKineticPools* pools, soModuleAccesser* accesser) {
    soKineticEnergyNormal* normal =
        static_cast<wnWarioBikeNormal0Pool&>(*pools).getSub().getInstanceAt(0);
    wnKineticEnergyGravity* gravity =
        static_cast<wnWarioBikeGravityPool&>(*pools).getSub().getInstanceAt(0);
    Vec2f speed = accesser->getKineticModule().getSumSpeed(
        soKineticEnergy::AttributeFlag(1));
    float brake = soValueAccesser::getConstantFloat(accesser, 4029, 0);
    float gravityAcceleration = soValueAccesser::getConstantFloat(accesser, 4009, 0);
    float limit = soValueAccesser::getConstantFloat(accesser, 4006, 0);
    float gravitySpeedLimit = soValueAccesser::getConstantFloat(accesser, 4010, 0);
    Vec2f zero(0.0f, 0.0f);
    Vec2f brakeVector(brake, 0.0f);
    Vec2f limitVector(limit, 0.0f);
    Vec2f::copy(normal->m_speed, speed);
    Vec2f::copy(normal->m_accel, zero);
    Vec2f::copy(normal->m_brake, brakeVector);
    Vec2f::copy(normal->m_speedTarget, zero);
    Vec2f::copy(normal->m_speedLimit, limitVector);
    normal->enable();
    gravity->m_speedY = speed.m_x;
    gravity->m_gravity = -gravityAcceleration;
    gravity->m_speedLimit = gravitySpeedLimit;
    gravity->enable();
}

float wnWarioBikeKineticTransactor::ABS(float value) {
    return __fabsf(value);
}

void wnWarioBikeKineticTransactor::updateEnergy1(
    wnKineticEnergyGravity* energy, soModuleAccesser* accesser) {
    if (energy->isEnable() == true && energy->isSuspend() == false)
        energy->updateEnergy(accesser);
}


void wnWarioBike::processUpdate() {
    if (m_moduleAccesser->getWorkManageModule().isFlag(0x2200000B)) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x2200000B);
        deactivate(false);
        return;
    }

    Weapon::processUpdate();
    m_moduleAccesser->getWorkManageModule().setFloat(
        m_moduleAccesser->getPostureModule().getLr(), 0x21000008);
}

bool wnWarioBike::notifyEventCollisionAttackCheck(u32 flags) {
    (void)flags;
    int count = m_moduleAccesser->getWorkManageModule().getInt(0x10000007);
    if (count > 0) {
        struct CollisionAttackEvent : soLinkEventArgs {
            int m_count;
            u8 m_unk0C;

            explicit CollisionAttackEvent(int value)
                : soLinkEventArgs(2115), m_count(value), m_unk0C(0) {}
        } event(count);

        m_moduleAccesser->getLinkModule().sendEventParents(3, event);
        m_moduleAccesser->getStopModule().setHitStopFrame(count, false);
        m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000007);
    }
    return false;
}

// Rider events govern release, throws, and the transfer of the bike's speed.
void wnWarioBike::notifyEventLink(soLinkEventArgs* eventInfo,
                                  soModuleAccesser* accesser,
                                  StageObject* other, int unk4) {
    soWorkManageModule& work = accesser->getWorkManageModule();
    soKineticModule& kinetic = accesser->getKineticModule();
    soPostureModule& posture = accesser->getPostureModule();
    soStatusModule& status = accesser->getStatusModule();
    soLinkModule& link = accesser->getLinkModule();
    if (other->m_taskCategory == gfTask::Category_Fighter &&
        (other->soGetSubKind() == Fighter_Wario || other->soGetSubKind() == Fighter_WarioMan) &&
        link.isLink(3)) {
        u32 parentTaskId;
        u32 taskId = other->m_taskId;
        parentTaskId = link.getParentTaskId(3);
        if (parentTaskId == taskId) {
            switch (eventInfo->m_eventKind) {
            case 1109: {
                soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(0));
                wnKineticEnergyGravity& gravity = dynamic_cast<wnKineticEnergyGravity&>(*kinetic.getEnergy(1));
                const Vec2f& speed = static_cast<ftWarioBikeSpeedEvent*>(eventInfo)->speed;
                normal.m_speed = Vec2f(speed.m_x, 0.0f);
                gravity.m_speedY = speed.m_y;
                Vec3f parentPosition = link.getParentModelNodeGlobalPosition(3, static_cast<u32>(0), false);
                posture.setPos(&parentPosition);
                posture.setLr(link.getParentLr(3));
                posture.updateRotYLr();
                link.removeModelConstraint(true);
                link.setModelConstraintAttribute(3, true);
                link.setAttribute(3, static_cast<soLinkConnection::Attribute>(3), false);
                gfTaskScheduler::getInstance()->changeTaskPriorityRequest(m_taskId, 5);
                accesser->getWorkManageModule().onFlag(0x12000002);
                work.onFlag(0x22000000);
                status.changeStatusRequest(2, accesser);
                return;
            }
            case 1110:
                status.changeStatusRequest(9, accesser);
                return;
            case 1111:
                static_cast<ftWarioBikeSpeedEvent*>(eventInfo)->speed =
                    kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
                if (accesser->getSituationModule().getKind() == Situation_Ground)
                    status.changeStatusRequest(10, accesser);
                else
                    status.changeStatusRequest(13, accesser);
                return;
            case 1112:
                static_cast<ftWarioBikeSpeedEvent*>(eventInfo)->speed =
                    kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
                return;
            case 1114: {
                if (link.isModelConstraint()) link.removeModelConstraint(true);
                soKineticModule& releaseKinetic = accesser->getKineticModule();
                soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*releaseKinetic.getEnergy(0));
                // HYPOTHESIS: this parameter is the multiplier for the release velocity.
                Vec2f speed = static_cast<ftWarioBikeSpeedEvent*>(eventInfo)->speed * m_param->unk90;
                normal.m_speed = speed;
                normal.enable();
                accesser->getKineticModule().unableEnergy(1);
                status.changeStatusRequest(13, accesser);
                return;
            }
            case 1113:
                status.changeStatusRequest(11, accesser);
                return;
            case 1115:
                status.changeStatusRequest(14, accesser);
                return;
            case 60:
                link.setModelConstraintAttribute(3, false);
                if (link.isModelConstraint()) {
                    link.removeModelConstraint(true);
                    status.changeStatusRequest(13, accesser);
                }
                break;
            case 0:
                work.onFlag(0x2200000B);
                break;
            }
            goto baseEvent;
        }
    }
    if (link.isLink(5)) {
        u32 parentTaskId;
        u32 taskId = other->m_taskId;
        parentTaskId = link.getParentTaskId(5);
        if (parentTaskId == taskId) {
            switch (eventInfo->m_eventKind) {
            case 0:
                work.onFlag(0x2200000B);
                break;
            }
            goto baseEvent;
        }
    }
    switch (eventInfo->m_eventKind) {
    case StageObject::Link::Event_Touch_Item:
        if (link.isLinked(6)) {
            StageObject* item = static_cast<soLinkTouchItemEventArgs*>(eventInfo)->m_item;
            switch (item->soGetSubKind()) {
            case 5:
            case 15:
                accesser->getLinkModule().sendEventParents(3, *eventInfo);
                break;
            }
        }
        break;
    }
baseEvent:
    Weapon::notifyEventLink(eventInfo, accesser, other, unk4);
}

void wnWarioBike::notifyEventCollisionAttack(float power, soCollisionLog* log,
                                             soModuleAccesser* accesser) {
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(
        static_cast<gfTask::Category>(log->m_taskCategory), log->m_taskId);
    soCollisionAttackData* attackData = accesser->getCollisionAttackModule().getData(
        log->m_damageIndex, log->m_isAbsolute);
    int frames = soDamageUtilActor::calcHitStopFrame(
        accesser, power, 1.0f, 1.0f, attackData, 0);

    // Native only substitutes the target reaction frame for the log's
    // reaction-frame case; the task is looked up from the collision log.
    if (log->m_kind == 1 && task != nullptr) {
        StageObject& target = dynamic_cast<StageObject&>(*task);
        soCollisionHitModule* hit =
            soExternalValueAccesser::getCollisionHitModule(&target);
        if (hit->isReactionFrame()) frames = hit->getReactionFrame();
    }

    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    int pending = work.getInt(0x10000007);
    if (pending < frames) pending = frames;
    work.setInt(pending, 0x10000007);
}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_110_102F8(u8* p) { return *(void**)(*(u8**)(p + 0xd8) + 0x70); }
int fn_110_10304(u8* p) { return *(int*)(p + 0x60); }
int fn_110_12900(u8* p) { return *(int*)(p + 0xC0); }
int fn_110_12908(u8* p) { return *(int*)(p + 0x28); }
int fn_110_12A84(u8* p) { return *(int*)(p + 0xb8); }
} // extern "C"

// Link 5 identifies the rider selected by side-special entry. When a bike item
// is reclaimed, this can be its previous rider rather than its new founder.
void wnWarioBike::activate(int founderTaskId, int riderTaskId, int resourceId,
                           int team, Vec3f* position, float lr) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *position;
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.flag4DBit7 = true;
    desc.unk44 = 0x50;
    Weapon::activate(&desc);

    soLinkModule& link = m_moduleAccesser->getLinkModule();
    link.setAttribute(3, static_cast<soLinkConnection::Attribute>(3), false);
    link.setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Unknown_4, true);
    link.setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Stop, true);
    link.setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Shake, true);
    link.setAttribute(3, static_cast<soLinkConnection::Attribute>(15), true);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    m_moduleAccesser->getWorkManageModule().setInt(resourceId, 0x10000006);
    m_moduleAccesser->getWorkManageModule().setInt(*static_cast<const int*>(m_data) + 1, 0x10000005);
    m_moduleAccesser->getKineticModule().unableEnergyAll();

    ftWarioBikeTaskEvent event(0x45C, m_taskId);
    link.link(5, riderTaskId);
    link.sendEventParents(5, event);
    m_moduleAccesser->getDamageModule().setNoReactionModePerfect(true);
    m_moduleAccesser->getGroundModule().setEventToGround(true, 0);
}

// The energy pools of the bike's kinetic mediator, innermost first; their destructors call each other out of line.
#pragma dont_inline on
template class soKineticEnergyHolder<soKineticEnergyNormal, soTypeListNullType, soKineticEnergyInitInfo<0, 1> >;
template class soInstancePoolSub<wnWarioBikeNormal0Info, soTypeListNullType>;
template class soInstancePool<wnWarioBikeNormal0Info, soInstancePoolRoot, soTypeListNullType, 0>;
template class soLineInvertHierarchy<soTypeList<wnWarioBikeNormal0Info, soTypeListNullType>, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<wnKineticEnergyGravity, soTypeListNullType, soKineticEnergyInitInfo<1, 1> >;
template class soInstancePoolSub<wnWarioBikeGravityInfo, soTypeListNullType>;
template class soInstancePool<wnWarioBikeGravityInfo, soLineInvertHierarchy<soTypeList<wnWarioBikeNormal0Info, soTypeListNullType>, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 1>;
template class soLineInvertHierarchy<soTypeList<wnWarioBikeNormal0Info, soTypeList<wnWarioBikeGravityInfo, soTypeListNullType> >, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<soKineticEnergyNormal, soTypeListNullType, soKineticEnergyInitInfo<2, 1> >;
template class soInstancePoolSub<wnWarioBikeNormal2Info, soTypeListNullType>;
template class soInstancePool<wnWarioBikeNormal2Info, soLineInvertHierarchy<soTypeList<wnWarioBikeNormal0Info, soTypeList<wnWarioBikeGravityInfo, soTypeListNullType> >, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 2>;
template class soLineInvertHierarchy<soTypeList<wnWarioBikeNormal0Info, soTypeList<wnWarioBikeGravityInfo, soTypeList<wnWarioBikeNormal2Info, soTypeListNullType> > >, soInstancePool, soInstancePoolRoot>;
#pragma dont_inline off
