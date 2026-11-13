#define IP_INPUT_BUTTON_RAW
#define SO_BUILDER_HELPERS_WEAK
#define WN_STATUS_NO_TAIL
#define WN_WEAPON_NO_COMMON_ENERGIES
#define MT_VEC3F_IMPLICIT_ASSIGN
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <types.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/samus/wn_s_zerosuit_whip.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_activate_desc.h>
#include <gf/gf_task.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_vector.h>
#include <so/collision/so_collision_log.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_external_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <wn/wn_lasso_status_view.h>

// HYPOTHESIS: a data word of the REL (the node the whip model is attached with).
extern u32 lbl_94_data_C858;

extern soStatusUniqProcess g_wnLassoStatusUniqProcessMotionShoot;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessExhaust;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessMotionPull;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessFailure;

wnSZerosuitWhip::wnSZerosuitWhip(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitWhipData* data) :
    wnWeaponBuilder<wnSZerosuitWhipModuleAccesserBuildConfig>(
        "SZerosuitWhip", articleId, info,
        wnModuleAccesserBuildData(0x41, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uniqProcs[1] = &g_wnLassoStatusUniqProcessMotionShoot;
    uniqProcs[2] = &g_wnLassoStatusUniqProcessExhaust;
    uniqProcs[3] = &g_wnLassoStatusUniqProcessRewind;
    uniqProcs[4] = &g_wnLassoStatusUniqProcessMotionPull;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    uniqProcs[8] = &g_wnLassoStatusUniqProcessFailure;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000007);
    Weapon::deactivate(false);
}

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

// soExternalValueAccesser::getMotionKind (the map name); wn_szerosuit_paralyzer.cpp calls it by this stand-in name.
extern "C" int fn_27_8CC10(StageObject* stageObject);

void wnSZerosuitWhip::deactivate(bool flag) {
    if (m_moduleAccesser->m_enumerationStart->m_workManageModule->getInt(0x10000007) != -1) {
        m_moduleAccesser->m_enumerationStart->m_collisionAttackModule->unRefLogGroup(0);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(-1, 0x10000007);
    }
    Weapon::deactivate(flag);
}

void wnSZerosuitWhip::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* accesser, StageObject* sender, int selector) {
    soLinkModule* link = m_moduleAccesser->m_enumerationStart->m_linkModule;
    if (sender->soGetKind() == 0 && sender->soGetSubKind() == 0x18 &&
        link->isLink(3)) {
        u32 taskId = sender->m_taskId;
        u32 parentTaskId = link->getParentTaskId(3);
        if (parentTaskId == taskId) {
            switch (event->m_eventKind) {
            case 0x455:
                int kind = -1;
                switch (fn_27_8CC10(sender)) {
                case 0x1D5: kind = 3; break;
                case 0x1D6: kind = 4; break;
                case 0x1DC: kind = 5; break;
                case 0x1DD: kind = 6; break;
                }
                m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(kind, 0x10000009);
                break;
            }
        }
    }
    Weapon::notifyEventLink(event, accesser, sender, selector);
}

void wnSZerosuitWhip::processFixPosition() {
    Weapon::processFixPosition();
    int kind = m_moduleAccesser->m_enumerationStart->m_workManageModule->getInt(0x10000009);
    if (kind != -1) {
        float frame = m_moduleAccesser->m_enumerationStart->m_motionModule->getFrame();
        soMotionChangeParam param;
        param.m_kind = kind;
        param.m_frame = frame;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 1;
        param._15 = 0;
        m_moduleAccesser->m_enumerationStart->m_motionModule->changeMotionRequest(&param);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(-1, 0x10000009);
    }
}

bool wnSZerosuitWhip::notifyEventCollisionAttackCheck(u32) {
    int frame = m_moduleAccesser->m_enumerationStart->m_workManageModule->getInt(0x10000008);
    if (frame > 0) {
        m_moduleAccesser->m_enumerationStart->m_stopModule->setHitStopFrame(frame, 0);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(0.0f, 0x11000001);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(0, 0x10000008);
    }
    return false;
}

// HYPOTHESIS: isSpecial selects the neutral (side special) whip, isAerial the tilt whip.
void wnSZerosuitWhip::activate(int founderTaskId, u32 etcResId, int team, float lr, Vec3f* pos, int isSpecial, int isAerial, int logGroup) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = etcResId;
    desc.unk8 = etcResId;
    desc.unkC = etcResId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *pos;
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.flag4DBit7 = 1;
    desc.unk38 = 3;
    desc.unk3C = 0xff;
    Weapon::activate(&desc);
    m_moduleAccesser->m_enumerationStart->m_kineticModule->changeKinetic(2, m_moduleAccesser);
    m_moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(0.0f, 0x11000001);
    m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(logGroup, 0x10000007);
    if (logGroup != -1) {
        m_moduleAccesser->m_enumerationStart->m_collisionAttackModule->refLogGroup(0, logGroup);
    }
    if (isSpecial) {
        soResourceModule& resource = m_moduleAccesser->getResourceModule();
        wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
        u32 nodeId = lbl_94_data_C858;
        int noValue = 0;
        alt->unk18Attach(resource.getResourceIdAccesser()->getBinResId(), noValue, nodeId, nodeId, 1, 0, noValue, -1);
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        wnLassoGetAltStatus(m_moduleAccesser)->unkDCFloat(posture.getScale());
        wnLassoGetAltStatus(m_moduleAccesser)->unkA4Int(6);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setFlag(false, 0x22000000);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(0xa, 0x10000005);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(2, 0x10000006);
        m_moduleAccesser->m_enumerationStart->m_statusModule->changeStatusForce(1, m_moduleAccesser);
    } else if (isAerial) {
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(0xa, 0x20000000);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(25.0f, 0x21000000);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(100.0f, 0x21000001);
        m_moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(m_param->unk8, 0x21000002);
    }
    m_moduleAccesser->m_enumerationStart->m_workManageModule->setInt(-1, 0x10000009);
}

void wnSZerosuitWhip::notifyEventCollisionAttack(float power, soCollisionLog* log, soModuleAccesser* acc) {
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    float pending = work.getFloat(0x11000001);
    if (pending < power) pending = power;
    work.setFloat(pending, 0x11000001);
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById((gfTask::Category)log->m_taskCategory, log->m_taskId);
    soCollisionAttackData* attackData = acc->getCollisionAttackModule().getData(log->m_damageIndex, log->m_isAbsolute);
    int frames = soDamageUtilActor::calcHitStopFrame(acc, power, 1.0f, 1.0f, attackData, 0);
    // The hit stop of a reaction frame target replaces the computed one.
    if (log->m_kind == 1 && task != nullptr) {
        StageObject& target = dynamic_cast<StageObject&>(*task);
        soCollisionHitModule* hit = soExternalValueAccesser::getCollisionHitModule(&target);
        if (hit->isReactionFrame()) frames = hit->getReactionFrame();
    }
    int pendingFrames = work.getInt(0x10000008);
    if (pendingFrames < frames) pendingFrames = frames;
    work.setInt(pendingFrames, 0x10000008);
}
