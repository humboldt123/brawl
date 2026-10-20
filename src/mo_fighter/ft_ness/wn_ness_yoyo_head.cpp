#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/wn_module_accesser_builder.h>
#include <wn/ness/wn_ness_yoyo_events.h>
#include <wn/ness/wn_ness_yoyo_head.h>
#include <gf/gf_task.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/collision/so_collision_log.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/stop/so_stop_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_activate_desc.h>

extern soStatusUniqProcess g_wnNessYoyoHeadStatusUniqProcessShot;
extern soStatusUniqProcess g_wnNessYoyoHeadStatusUniqProcessShotFall;
extern soStatusUniqProcess g_wnNessYoyoHeadStatusUniqProcessTakeup;

// HYPOTHESIS: the event the owner sends to the head: the task id of the string to link (0x455) and a work value; 0x456 sets
// the damage multiplier of the head.
struct wnNessYoyoHeadOwnerLinkEvent : soLinkEventArgs {
    union {
        int unk8;
        float power;
    };
    int unkC;
    wnNessYoyoHeadOwnerLinkEvent() : soLinkEventArgs(0x455) { }
};

wnNessYoyoHead::wnNessYoyoHead(s32 articleId, const wnArticleBuildInfo& info, const wnNessYoyoHeadData* data) :
    wnWeaponBuilder<wnNessYoyoHeadModuleAccesserBuildConfig>(
        "NessYoyoHead", articleId, info,
        wnModuleAccesserBuildData(108, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[0] = &g_wnNessYoyoHeadStatusUniqProcessShot;
    uniqProcs[1] = &g_wnNessYoyoHeadStatusUniqProcessShotFall;
    uniqProcs[2] = &g_wnNessYoyoHeadStatusUniqProcessTakeup;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    Weapon::deactivate(false);
}

void wnNessYoyoHead::activate(int founderTaskId, int kind, int team, Vec3f* pos, float lr, bool flag) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = kind;
    desc.unk8 = kind;
    desc.unkC = kind;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 1;
    desc.unk1C = 0;
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getCollisionAttackModule().setPowerMul(1.0f);
    m_moduleAccesser->getWorkManageModule().setFlag(flag, 0x22000000);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    m_moduleAccesser->getKineticModule().changeKinetic(2, m_moduleAccesser);
    updateNodeSRT();
}

void wnNessYoyoHead::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc, StageObject* sender, int unk4) {
    if (sender->soGetKind() == 0 && sender->soGetSubKind() == 10) {
        switch (event->m_eventKind) {
        case 0x455: {
            wnNessYoyoHeadOwnerLinkEvent* owner = (wnNessYoyoHeadOwnerLinkEvent*)event;
            soWorkManageModule& work = acc->getWorkManageModule();
            soLinkModule& link = acc->getLinkModule();
            if (link.isLink(5) == false) {
                link.link(5, owner->unk8);
                work.setInt(owner->unk8, 0x20000000);
                work.setInt(owner->unkC, 0x20000001);
            }
            return;
        }
        case 0x456:
            acc->getCollisionAttackModule().setPowerMul(((wnNessYoyoHeadOwnerLinkEvent*)event)->power);
            return;
        }
    } else if (sender->soGetKind() == 2 && sender->soGetSubKind() == 0x6b && event->m_eventKind == 0x838) {
        wnNessYoyoPullEvent* pull = (wnNessYoyoPullEvent*)event;
        soWorkManageModule& work = acc->getWorkManageModule();
        work.onFlag(0x22000004);
        soKineticEnergyNormal* energy = dynamic_cast<soKineticEnergyNormal*>(acc->getKineticModule().getEnergy(0));
        work.setFloat(pull->unk8, 0x21000000);
        Vec2f::copy(pull->unkC, energy->getSpeed());
        Vec2f::copy(pull->unk14, energy->m_accel);
        Vec2f::copy(pull->unk1C, energy->m_speedLimit);
        acc->getKineticModule().changeKinetic(2, acc);
        return;
    }
    Weapon::notifyEventLink(event, acc, sender, unk4);
}

void wnNessYoyoHead::notifyEventCollisionAttack(float power, soCollisionLog* log, soModuleAccesser* acc) {
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById((gfTask::Category) * (u8*)((u8*)log + 0x22), log->m_taskId);
    soCollisionAttackData* attackData = acc->getCollisionAttackModule().getData(log->m_damageIndex, log->m_isAbsolute);
    float one = 1.0f;
    int frames = soDamageUtilActor::calcHitStopFrame(power, one, one, acc, attackData, 0);
    // The hit stop of a reaction frame target replaces the computed one.
    if (*(u8*)((u8*)log + 0x21) == 1 && task != nullptr) {
        StageObject* target = dynamic_cast<StageObject*>(task);
        soCollisionHitModule* hit = soExternalValueAccesser::getCollisionHitModule(target);
        if (hit->isReactionFrame()) frames = hit->getReactionFrame();
    }
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    int pending = work.getInt(0x10000005);
    if (pending < frames) pending = frames;
    work.setInt(pending, 0x10000005);
}

bool wnNessYoyoHead::notifyEventCollisionAttackCheck(u32 flags) {
    int frames = m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
    if (frames > 0) {
        wnNessYoyoHeadLinkEvent event;
        event.unk8 = frames;
        event.unkC = false;
        m_moduleAccesser->getLinkModule().sendEventParents(3, event);
        m_moduleAccesser->getLinkModule().sendEventParents(5, event);
        m_moduleAccesser->getStopModule().setHitStopFrame(frames, 0);
        m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000005);
    }
    return false;
}
