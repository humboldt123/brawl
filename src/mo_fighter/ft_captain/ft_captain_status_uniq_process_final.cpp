#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_SUB_ASCENDING
#define SO_COLLISION_ATTACK_DATA_DTOR
#define SO_COLLISION_ATTACK_DATA_PLAIN_BITFIELDS
#include <ft/captain/ft_captain_final_sub_routine.h>
#include <ft/fighter.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_prng.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/collision/so_collision_log.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/slow/so_slow_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/so_world.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <st/stage.h>
#include <types.h>

// Captain Falcon's Final Smash (Blue Falcon): the start, the ride, the three cut-in scenes and the end, plus the link
// message handler of the hit fighters. The scene work lives in ft_captain_final_sub_routine.cpp.

class ftCaptainStatusUniqProcessFinalStart : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalStart() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    // Called by ftCaptain's collision attack events while the start status runs (map names).
    void notifyEventCollisionAttackFighter(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    bool notifyEventCollisionAttackCheck(u32 flags, soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFinalRide : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalRide() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFinalScene01 : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalScene01() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftCaptainStatusUniqProcessFinalScene02 : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalScene02() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

class ftCaptainStatusUniqProcessFinalScene03 : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalScene03() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFinalEnd : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

// A fighter the Blue Falcon hits (unless it is under damage lock or invincible) joins the target table.
void ftCaptainStatusUniqProcessFinalStart::notifyEventCollisionAttackFighter(float power, soCollisionLog* collisionLog,
                                                                             soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManageModule = &moduleAccesser->getWorkManageModule();
    StageObject* target = dynamic_cast<StageObject*>(gfTask::getTask(collisionLog->m_taskId));
    if (target->soGetKind() != StageObject_Fighter) {
        return;
    }
    soModuleAccesser* targetModuleAccesser = soExternalValueAccesser::getModuleAccesser(target);
    if (targetModuleAccesser->getDamageModule().isDamageLock() ||
        targetModuleAccesser->getCollisionHitModule().getInvincibleFrameGlobal(0) >= 1) {
        return;
    }
    workManageModule->onFlag(0x22000010);
    int count = workManageModule->getInt(0x20000000);
    workManageModule->incInt(0x20000000);
    ftCaptainFinalTargetWork* work = &ftCaptainFinalSubRoutine::s_targetWork[count];
    u32 taskId = collisionLog->m_taskId;
    work->m_taskId = taskId;
    work->m_linkNo = ftCaptainFinalSubRoutine::set_link(moduleAccesser, taskId);
    work->m_statusKind = soExternalValueAccesser::getStatusKind(target);
    work->m_situationKind = soExternalValueAccesser::getSituationKind(target);
    work->m_pos = soExternalValueAccesser::getPos(target);
    work->m_lr = soExternalValueAccesser::getLr(target);
    work->m_moduleAccesser = targetModuleAccesser;
    work->m_isTarget = true;
}

// Once a fighter was hit, the other fighters are linked, everyone is told the ride starts and the stage pauses.
bool ftCaptainStatusUniqProcessFinalStart::notifyEventCollisionAttackCheck(u32 flags, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        ftCaptainFinalSubRoutine::link_other_target(moduleAccesser);
        ftCaptainFinalLinkEvent targetEvent(0x44f, 0);
        ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &targetEvent);
        ftCaptainFinalLinkEvent otherEvent(0x450, 0xa);
        ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &otherEvent);
        ftCaptainFinalSubRoutine::set_pause_category(moduleAccesser, true);
        ftCaptainFinalSubRoutine::invisible_item();
        ftCaptainFinalSubRoutine::invisible_weapon();
    }
    return true;
}

void ftCaptainStatusUniqProcessFinalRide::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManageModule = &moduleAccesser->getWorkManageModule();
    soPostureModule* postureModule = &moduleAccesser->getPostureModule();
    soSlowModule* slowModule = &moduleAccesser->getSlowModule();
    dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
    Vec3f pos;
    pos = postureModule->getPos();
    float lr = postureModule->getLr();
    workManageModule->setFloat(pos.m_x, 0x21000004);
    workManageModule->setFloat(pos.m_y, 0x21000005);
    workManageModule->setFloat(lr, 0x21000006);
    slowModule->clearWhole();
    slowModule->setWhole(4, 0);
    ftCaptainFinalLinkEvent event(0x44f, 9);
    ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &event);
}

void ftCaptainStatusUniqProcessFinalRide::execStatus(soModuleAccesser* moduleAccesser) {
    ftCaptainFinalLinkEvent event(0x44f, 0);
    ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &event);
}

void ftCaptainStatusUniqProcessFinalRide::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (ftCaptainFinalSubRoutine::check_status_transition(moduleAccesser, nextStatus)) {
        ftCaptainFinalLinkEvent targetEvent(0x44f, 2);
        ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &targetEvent);
        ftCaptainFinalLinkEvent posEvent(0x44f, 6);
        posEvent.m_x = 0.0f;
        posEvent.m_y = 3000.0f + soValueAccesser::getConstantFloat(moduleAccesser, 0xfbd, 0);
        posEvent.m_z = 0.0f;
        posEvent.m_lr = -1.0f;
        posEvent.unk1C = 360.0f * randf();
        posEvent.unk20 = false;
        moduleAccesser->getLinkModule().sendEventParents(ftCaptainFinalSubRoutine::s_targetWork[0].m_linkNo, posEvent);
        ftCaptainFinalLinkEvent otherEvent(0x450, 0xb);
        ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &otherEvent);
        ftCaptainFinalSubRoutine::set_layer_disp_status(false);
        moduleAccesser->getEffectModule().removeAll(8);
        g_soWorld->m_13 = false;
    } else {
        ftCaptainFinalSubRoutine::restart_other_fighter(moduleAccesser);
        ftCaptainFinalSubRoutine::unlink_all(moduleAccesser);
        ftCaptainFinalSubRoutine::setup_equipment_all();
        ftCaptainFinalSubRoutine::set_pause_category(moduleAccesser, false);
        ftCaptainFinalSubRoutine::visible_item();
        ftCaptainFinalSubRoutine::visible_weapon();
        ftCaptainFinalSubRoutine::free_target_work();
    }
    moduleAccesser->getSlowModule().clearWhole();
}

void ftCaptainStatusUniqProcessFinalScene01::initStatus(soModuleAccesser* moduleAccesser) {
    soPostureModule* postureModule = &moduleAccesser->getPostureModule();
    moduleAccesser->getWorkManageModule().offFlag(0x12000019);
    postureModule->setPos(&Vec3f(0.0f, 3000.0f, 0.0f));
    postureModule->setLr(0.0f);
    postureModule->setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
    moduleAccesser->getStageObject().updateNodeSRT();
    ftCaptainFinalSubRoutine::set_camera(moduleAccesser, 10);
    ftCaptainFinalSubRoutine::set_visibility_stage_effect(false);
    ftCaptainFinalSubRoutine::push_stage_motion_rate();
    ftCaptainFinalSubRoutine::lock_photo_camera(reinterpret_cast<void*>(true));
}

void ftCaptainStatusUniqProcessFinalScene01::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (ftCaptainFinalSubRoutine::check_status_transition(moduleAccesser, nextStatus)) {
        ftCaptainFinalLinkEvent event(0x44f, 3);
        ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &event);
    } else {
        ftCaptainFinalSubRoutine::set_layer_disp_status(true);
        ftCaptainFinalSubRoutine::visible_item();
        ftCaptainFinalSubRoutine::visible_weapon();
        ftCaptainFinalSubRoutine::reset_camera();
        ftCaptainFinalSubRoutine::set_pause_category(moduleAccesser, false);
        g_soWorld->m_13 = true;
        ftCaptainFinalSubRoutine::restart_other_fighter(moduleAccesser);
        ftCaptainFinalSubRoutine::abort_captain_final(moduleAccesser);
        ftCaptainFinalSubRoutine::unlink_all(moduleAccesser);
        ftCaptainFinalSubRoutine::setup_equipment_all();
        ftCaptainFinalSubRoutine::set_visibility_stage_effect(true);
        ftCaptainFinalSubRoutine::pop_stage_motion_rate();
        ftCaptainFinalSubRoutine::lock_photo_camera(reinterpret_cast<void*>(false));
        ftCaptainFinalSubRoutine::free_target_work();
    }
}

void ftCaptainStatusUniqProcessFinalScene02::initStatus(soModuleAccesser* moduleAccesser) {
    soPostureModule* postureModule = &moduleAccesser->getPostureModule();
    moduleAccesser->getWorkManageModule().offFlag(0x12000019);
    postureModule->setPos(&Vec3f(0.0f, 3000.0f, 0.0f));
    postureModule->setLr(0.0f);
    postureModule->setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
    moduleAccesser->getStageObject().updateNodeSRT();
    ftCaptainFinalSubRoutine::set_camera(moduleAccesser, 20);
    ftCaptainFinalLinkEvent event(0x44f, 6);
    event.m_x = 0.0f;
    event.m_y = 3000.0f;
    event.m_z = -2.5f;
    event.m_lr = -1.0f;
    event.unk1C = 0.0f;
    event.unk20 = false;
    moduleAccesser->getLinkModule().sendEventParents(ftCaptainFinalSubRoutine::s_targetWork[0].m_linkNo, event);
}

void ftCaptainStatusUniqProcessFinalScene02::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (!ftCaptainFinalSubRoutine::check_status_transition(moduleAccesser, nextStatus)) {
        ftCaptainFinalSubRoutine::set_layer_disp_status(true);
        ftCaptainFinalSubRoutine::visible_item();
        ftCaptainFinalSubRoutine::visible_weapon();
        ftCaptainFinalSubRoutine::reset_camera();
        ftCaptainFinalSubRoutine::set_pause_category(moduleAccesser, false);
        g_soWorld->m_13 = true;
        ftCaptainFinalSubRoutine::restart_other_fighter(moduleAccesser);
        ftCaptainFinalSubRoutine::abort_captain_final(moduleAccesser);
        ftCaptainFinalSubRoutine::unlink_all(moduleAccesser);
        ftCaptainFinalSubRoutine::setup_equipment_all();
        ftCaptainFinalSubRoutine::set_visibility_stage_effect(true);
        ftCaptainFinalSubRoutine::pop_stage_motion_rate();
        ftCaptainFinalSubRoutine::lock_photo_camera(reinterpret_cast<void*>(false));
        ftCaptainFinalSubRoutine::free_target_work();
    }
}

void ftCaptainStatusUniqProcessFinalScene03::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().offFlag(0x12000019);
    ftCaptainFinalSubRoutine::set_camera(moduleAccesser, 30);
}

// The first target in front of Captain Falcon's hips takes the hit of the third scene.
void ftCaptainStatusUniqProcessFinalScene03::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManageModule = &moduleAccesser->getWorkManageModule();
    soModelModule* modelModule = &moduleAccesser->getModelModule();
    soLinkModule* linkModule = &moduleAccesser->getLinkModule();
    Vec3f dir(1.0f, 0.0f, 0.0f);
    Vec3f hipPos;
    const char* nodeName = "HipN";
    hipPos = modelModule->getNodeGlobalPosition(modelModule->getNodeId(nodeName), false);
    int count = workManageModule->getInt(0x20000000);
    int i;
    ftCaptainFinalTargetWork* work = ftCaptainFinalSubRoutine::s_targetWork;
    for (i = 0; i < count; i++, work++) {
        if (work->m_isTarget && !work->unk24_6 && work->m_taskId != -1) {
            Vec3f targetPos;
            targetPos = linkModule->getParentModelNodeGlobalPosition(work->m_linkNo, "HipN", false);
            Vec3f diff;
            diff = targetPos - hipPos;
            if (dir.m_z * diff.m_z + (dir.m_x * diff.m_x + dir.m_y * diff.m_y) <= 0.0f) {
                ftCaptainFinalLinkEvent hitEvent(0x44f, 8);
                hitEvent.m_value = 1;
                linkModule->sendEventParents(work->m_linkNo, hitEvent);
                moduleAccesser->getCollisionAttackModule().hitAbsolute(0, &hipPos, work->m_taskId, 0, 0);
                ftCaptainFinalLinkEvent event(0x44f, 4);
                linkModule->sendEventParents(work->m_linkNo, event);
                work->unk24_6 = true;
                break;
            }
        }
    }
}

void ftCaptainStatusUniqProcessFinalScene03::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    ftCaptainFinalSubRoutine::set_layer_disp_status(true);
    ftCaptainFinalSubRoutine::reset_camera();
    moduleAccesser->getEffectModule().killAll(8, true, true);
    moduleAccesser->getEffectModule().reqEmit(EfID(0x48), 8);
    moduleAccesser->getWorkManageModule().onFlag(0x12000019);
    g_soWorld->m_13 = true;
    ftCaptainFinalSubRoutine::set_visibility_stage_effect(true);
    ftCaptainFinalSubRoutine::lock_photo_camera(reinterpret_cast<void*>(false));
    if (!ftCaptainFinalSubRoutine::check_status_transition(moduleAccesser, nextStatus)) {
        ftCaptainFinalSubRoutine::visible_item();
        ftCaptainFinalSubRoutine::visible_weapon();
        ftCaptainFinalSubRoutine::set_pause_category(moduleAccesser, false);
        ftCaptainFinalSubRoutine::restart_other_fighter(moduleAccesser);
        ftCaptainFinalSubRoutine::abort_captain_final(moduleAccesser);
        ftCaptainFinalSubRoutine::unlink_all(moduleAccesser);
        ftCaptainFinalSubRoutine::setup_equipment_all();
        ftCaptainFinalSubRoutine::pop_stage_motion_rate();
        ftCaptainFinalSubRoutine::free_target_work();
    }
}

// Captain Falcon goes back to the position kept by the ride; the targets are put next to him.
void ftCaptainStatusUniqProcessFinalEnd::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManageModule = &moduleAccesser->getWorkManageModule();
    soGroundModule* groundModule = &moduleAccesser->getGroundModule();
    soPostureModule* postureModule = &moduleAccesser->getPostureModule();
    soLinkModule* linkModule = &moduleAccesser->getLinkModule();
    Vec3f pos;
    pos.m_x = workManageModule->getFloat(0x21000004);
    pos.m_y = workManageModule->getFloat(0x21000005);
    pos.m_z = groundModule->getZ();
    float lr = workManageModule->getFloat(0x21000006);
    postureModule->setPos(&pos);
    postureModule->setLr(lr);
    postureModule->initRotYLr();
    moduleAccesser->getStageObject().updateNodeSRT();
    int count = workManageModule->getInt(0x20000000);
    ftCaptainFinalTargetWork* work = ftCaptainFinalSubRoutine::s_targetWork;
    for (int i = 0; i < count; i++, work++) {
        if (work->m_isTarget && work->m_taskId != -1) {
            ftCaptainFinalLinkEvent event(0x44f, 6);
            Vec2f centerPos;
            centerPos = groundModule->getCenterPos(0);
            event.m_x = workManageModule->getFloat(0x21000004);
            event.m_y = workManageModule->getFloat(0x21000005);
            event.m_z = groundModule->getZ();
            event.m_lr = workManageModule->getFloat(0x21000006);
            event.unk20 = true;
            event.unk24 = workManageModule->getFloat(0x21000004);
            event.unk28 = workManageModule->getFloat(0x21000005);
            event.unk2C = groundModule->getZ();
            linkModule->sendEventParents(work->m_linkNo, event);
        }
    }
    ftCaptainFinalLinkEvent targetEvent(0x44f, 5);
    ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &targetEvent);
    ftCaptainFinalLinkEvent otherEvent(0x450, 0xc);
    ftCaptainFinalSubRoutine::send_msg(moduleAccesser, &otherEvent);
    workManageModule->setInt(0, 0x20000002);
}

// MATCH-ONLY: a plain image of soCollisionAttackData. The original copies the offset position word by word (a Vec3f
// without the user assignment this unit's Vec3f has) and the other floats as floats.
struct ftCaptainAttackDataWords {
    u32 m_x;
    u32 m_y;
    u32 m_z;
    ftCaptainAttackDataWords(const ftCaptainAttackDataWords& o) : m_x(o.m_x), m_y(o.m_y), m_z(o.m_z) { }
    ftCaptainAttackDataWords& operator=(const ftCaptainAttackDataWords& o) {
        m_x = o.m_x;
        m_y = o.m_y;
        m_z = o.m_z;
        return *this;
    }
};
struct ftCaptainAttackDataImage {
    int m_power;
    ftCaptainAttackDataWords m_offsetPos;
    float m_size;
    int m_vector;
    int m_reactionEffect;
    int m_reactionFix;
    int m_reactionAdd;
    float m_slipChance;
    float m_hitStopFrame;
    float m_hitStopDelay;
    u32 m_flags0;
    u32 m_flags1;
    u32 m_flags2;
    u32 m_flags3;
};

// HYPOTHESIS: an inline helper keeps the attack data of the end motion.
static inline void keepAbsAttackData(soCollisionAttackModule* collisionAttackModule) {
    soCollisionAttackData attackData = *collisionAttackModule->getData(0, true);
    *reinterpret_cast<ftCaptainAttackDataImage*>(&ftCaptainFinalSubRoutine::s_absAttackData) =
        *reinterpret_cast<ftCaptainAttackDataImage*>(&attackData);
}

// The attack data of the end motion is kept once; from the second frame on, each frame hits the next target.
void ftCaptainStatusUniqProcessFinalEnd::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManageModule = &moduleAccesser->getWorkManageModule();
    soCollisionAttackModule* collisionAttackModule = &moduleAccesser->getCollisionAttackModule();
    if (!workManageModule->isFlag(0x22000012) && workManageModule->isFlag(0x22000011)) {
        keepAbsAttackData(collisionAttackModule);
        workManageModule->onFlag(0x22000012);
    }
    int frame = workManageModule->getInt(0x20000002);
    workManageModule->incInt(0x20000002);
    if (frame >= 1 && workManageModule->isFlag(0x22000012)) {
        int count = workManageModule->getInt(0x20000000);
        int i;
        ftCaptainFinalTargetWork* work = ftCaptainFinalSubRoutine::s_targetWork;
        for (i = 0; i < count; i++, work++) {
            if (work->m_isTarget && !work->unk24_5 && work->m_taskId != -1) {
                ftCaptainFinalLinkEvent event(0x44f, 8);
                event.m_value = 1;
                moduleAccesser->getLinkModule().sendEventParents(work->m_linkNo, event);
                int targetNo = event.m_value;
                ftCaptainFinalSubRoutine::setup_abs_attack(moduleAccesser);
                Vec3f pos;
                pos.m_x = workManageModule->getFloat(0x21000004);
                pos.m_y = workManageModule->getFloat(0x21000005);
                pos.m_z = moduleAccesser->getGroundModule().getZ();
                collisionAttackModule->hitAbsolute(0, &pos, work->m_taskId, 0, targetNo);
                work->unk24_5 = true;
                break;
            }
        }
    }
}

void ftCaptainStatusUniqProcessFinalEnd::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    ftCaptainFinalSubRoutine::visible_item();
    ftCaptainFinalSubRoutine::visible_weapon();
    ftCaptainFinalSubRoutine::restart_other_fighter(moduleAccesser);
    ftCaptainFinalSubRoutine::unlink_all(moduleAccesser);
    ftCaptainFinalSubRoutine::setup_equipment_all();
    ftCaptainFinalSubRoutine::set_pause_category(moduleAccesser, false);
    ftCaptainFinalSubRoutine::pop_stage_motion_rate();
    ftCaptainFinalSubRoutine::free_target_work();
}

// HYPOTHESIS: the link message handler of the Final Smash; message 0x3d carries the task id of a fighter at 0x28 of the
// sender. Its vtable has the size of a status process's.
class ftCaptainStatusUniqProcessFinalMessageHandler : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFinalMessageHandler() { }
    static void linkMessageProcedure(soLinkEventArgs* message, soModuleAccesser* moduleAccesser, StageObject* sender, int unk);
    static void update(soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: the sender's task id goes through a small by-value accessor (the same idiom as the article task id in
// ft_captain.cpp's activeArticle); it keeps the module accesser in its own register across the read.
static inline int getSenderTaskId(StageObject* sender) {
    int taskId = sender->m_taskId;
    return taskId;
}

void ftCaptainStatusUniqProcessFinalMessageHandler::linkMessageProcedure(soLinkEventArgs* message,
                                                                         soModuleAccesser* moduleAccesser, StageObject* sender, int) {
    switch (message->m_eventKind) {
    case 0x3d:
        ftCaptainFinalSubRoutine::invalidate_work(moduleAccesser, getSenderTaskId(sender));
        break;
    }
}

// The stage's shadows are cleared while the stage task has its flag at 0x2C (bit 5) set.
void ftCaptainStatusUniqProcessFinalMessageHandler::update(soModuleAccesser* moduleAccesser) {
    gfTask* task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Stage, 0);
    if (task && task->unk2C_b5) {
        g_Stage->clearShadow();
    }
}

ftCaptainStatusUniqProcessFinalStart g_ftCaptainStatusUniqProcessFinalStart;
ftCaptainStatusUniqProcessFinalRide g_ftCaptainStatusUniqProcessFinalRide;
ftCaptainStatusUniqProcessFinalScene01 g_ftCaptainStatusUniqProcessFinalScene01;
ftCaptainStatusUniqProcessFinalScene02 g_ftCaptainStatusUniqProcessFinalScene02;
ftCaptainStatusUniqProcessFinalScene03 g_ftCaptainStatusUniqProcessFinalScene03;
ftCaptainStatusUniqProcessFinalEnd g_ftCaptainStatusUniqProcessFinalEnd;
ftCaptainStatusUniqProcessFinalMessageHandler g_ftCaptainStatusUniqProcessFinalMessageHandler;
