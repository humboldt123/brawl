#define FT_FIGHTER_ANIMCMD_LONG // MATCH-ONLY: Fighter::notifyEventAnimCmd(long) overrides StageObject's slot (thunk at -0x48 for soAnimCmdEventObserver)
#define FT_KINETIC_MEDIATOR_MODE_ARG // MATCH-ONLY: the REL passes (mode, pools, accesser) to the sora_melee dispatcher
#define FT_MARTH_RUNTIME_HELPERS
#define FT_MARTH_PHOTO_CALLBACK_NOINLINE
#define FT_MARTH_COLLISION_VEC3F_NOINLINE
#include <ft/ft_class_info_impl.h>
#include <ft/marth/ft_marth.h>
#include <ft/marth/ft_marth_extend_param_accesser.h>
#include <if/if_marth_final.h>
#include <so/so_value_accesser.h>
#include <so/so_external_value_accesser.h>
#include <gf/gf_task_scheduler.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_external_value_accesser.h>
#include <mt/mt_prng.h>
#include <math.h>
#include <ft/marth/ft_marth_status_uniq_process.h>


#define FT_BC ftMarthBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftMarthExtendParamAccesser g_ftMarthExtendParamAccesser;

ftClassInfoImpl<Fighter_Marth, ftMarth> g_ftClassInfoMarth;

ftMarth::ftMarth(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftMarthBuildConfig>(entryId,
                                         Fighter_Marth,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Marth)) {
    // Register the fifteen character-specific status processes in action order.
    soStatusUniqProcess* processes[15] = {0};
    processes[0] = &g_ftMarthStatusUniqProcessSpecialNStart;
    processes[1] = &g_ftMarthStatusUniqProcessSpecialS;
    processes[2] = &g_ftMarthStatusUniqProcessSpecialHi;
    processes[3] = &g_ftMarthStatusUniqProcessSpecialLw;
    processes[4] = &g_ftMarthStatusUniqProcessFinal;
    processes[5] = &g_ftMarthStatusUniqProcessSpecialNLoop;
    processes[6] = &g_ftMarthStatusUniqProcessSpecialNEnd;
    processes[8] = &g_ftMarthStatusUniqProcessSpecialS;
    processes[9] = &g_ftMarthStatusUniqProcessSpecialS;
    processes[10] = &g_ftMarthStatusUniqProcessSpecialS;
    processes[11] = &g_ftMarthStatusUniqProcessSpecialLw;
    processes[12] = &g_ftMarthStatusUniqProcessFinal;
    processes[13] = &g_ftMarthStatusUniqProcessFinal;
    processes[14] = &g_ftMarthStatusUniqProcessFinal;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 15);

    const ftMarthExtendParamSpecialLwShield* param =
        static_cast<const ftMarthExtendParamSpecialLwShield*>(m_data->extendParam[4]);
    soCollisionShieldData shield;
    shield.setOffset(0, param->offsetX);
    shield.setOffset(1, param->offsetY);
    shield.setOffset(2, param->offsetZ);
    shield.setOffset(3, param->offsetX);
    shield.setOffset(4, param->offsetY);
    shield.setOffset(5, param->offsetZ);
    shield.m_size = param->radius;
    shield.m_nodeIndex = param->nodeId;
    shield.m_shapeType = 1;
    soCollisionShieldGroupData group;
    group.m_flags = 0;
    group.m_shieldDataSet = soSet<soCollisionShieldData>(&shield, 1);
    m_moduleAccesser->getCollisionShieldModule().add(&group, 1);
}

ftMarth::~ftMarth() { }

ftKineticEnergyController::~ftKineticEnergyController() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

void ftMarth::processUpdate() { Fighter::processUpdate(); }

#pragma dont_inline off

void ftMarth::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    // Counter records the first hit on its counter shield for the retaliation.
    if (m_moduleAccesser->getStatusModule().getStatusKind() == ftMarthStatus::SpecialLw && groupIndex == 1) {
        if (!m_moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialLw_Hit)) {
            m_moduleAccesser->getWorkManageModule().onFlag(ftMarthWork::SpecialLw_Hit);
            m_moduleAccesser->getWorkManageModule().setFloat(power, ftMarthWork::SpecialLw_Power);
            soCollisionAttackData* attackData = attackModule->getData(collisionLog->m_collsionIndex, collisionLog->m_isAbsolute);
            float lr = m_moduleAccesser->getDamageModule().getDamageLr(posX, posY, attackModule, attackData, collisionLog->m_collsionIndex, collisionLog->m_isAbsolute);
            m_moduleAccesser->getWorkManageModule().setFloat(lr, ftMarthWork::SpecialLw_Lr);
        }
        StageObject& attacker = dynamic_cast<StageObject&>(*gfTaskScheduler::getInstance()->getTask(collisionLog->m_taskId));
        soStopModule* stop = soExternalValueAccesser::getStopModule(&attacker);
        stop->setHitStopFrameFix(soValueAccesser::getConstantInt(m_moduleAccesser, ftMarthParam::SpecialLw_HitStopFrame, 0));
    }
    Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftMarth::notifyEventCollisionShieldCheck() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == ftMarthStatus::SpecialLw &&
        m_moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::SpecialLw_Hit)) {
        m_moduleAccesser->getWorkManageModule().offFlag(ftMarthWork::SpecialLw_Hit);
        m_moduleAccesser->getStatusModule().changeStatusRequest(ftMarthStatus::SpecialLwHit, m_moduleAccesser);
        m_moduleAccesser->getPostureModule().setLr(m_moduleAccesser->getWorkManageModule().getFloat(ftMarthWork::SpecialLw_Lr));
        m_moduleAccesser->getPostureModule().updateRotYLr();
        int hitStop = soValueAccesser::getConstantInt(m_moduleAccesser, ftMarthParam::SpecialLw_HitStopFrame, 0);
        m_moduleAccesser->getStopModule().setHitStopFrame(hitStop, false);
        return true;
    }
    return Fighter::notifyEventCollisionShieldCheck();
}

void ftMarth::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    int count;
    float sine;
    if (m_moduleAccesser->getStatusModule().getStatusKind() == ftMarthStatus::FinalDash) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, collisionLog->m_taskId);
        if (task != NULL) {
            Fighter& target = dynamic_cast<Fighter&>(*task);
            if (ftExternalValueAccesser::getsoCollisionHitModule(&target)->getTotalStatus(0) != 3) {
                m_moduleAccesser->getWorkManageModule().onFlag(ftMarthWork::Final_HitConnected);
            }
        }
    } else if (m_moduleAccesser->getStatusModule().getStatusKind() == ftMarthStatus::FinalHit) {
        gfTask* task = gfTaskScheduler::getInstance()->getTask(collisionLog->m_taskId);
        if (task->m_taskCategory == gfTask::Category_Fighter) {
            Fighter& target = dynamic_cast<Fighter&>(*task);
            if (ftExternalValueAccesser::getsoCollisionHitModule(&target)->getTotalStatus(0) == 0) {
                count = m_moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowCount);
                if (count < 6) {
                    // Temporarily link the target to place its numbered HP window.
                    moduleAccesser->getLinkModule().link(6, collisionLog->m_taskId);
                    ftKind kind = ftKind(m_moduleAccesser->getStageObject().soGetSubKind());
                    u32 resId = g_ftCommonDataAccesser.getFinalResId(kind);
                    void* resourceData = m_moduleAccesser->getResourceModule().getBinFile(resId, 0, -1);
                    IfMarthFinalTask* window = IfMarthFinalTask::create(resourceData, Heaps::HeapType(0x1f), count + 1);
                    if (gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, collisionLog->m_taskId) != NULL) {
                        float angle = randi(360);
                        float radius = soValueAccesser::getConstantFloat(moduleAccesser, ftMarthParam::Final_WindowStep, 0);
                        sine = sin(angle);
                        float cosine = cos(angle);
                        // The original computes this offset but places the window without it.
                        Vec3f unusedOffset(radius * cosine, radius * sine, 0.0f);
                        Vec3f pos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false);
                        pos.m_y += -2.0f;
                        window->setPosConv(&pos);
                    }
                    window->dispOn(0);
                    window->setAnim(0);
                    m_moduleAccesser->getWorkManageModule().setInt(window->m_taskId, ftMarthWork::Final_WindowTaskId + count);
                    m_moduleAccesser->getWorkManageModule().offFlag(ftMarthWork::Final_WindowHit);
                    m_moduleAccesser->getWorkManageModule().addInt(1, ftMarthWork::Final_WindowCount);
                    moduleAccesser->getLinkModule().unlink(6);
                }
                m_moduleAccesser->getWorkManageModule().onFlag(ftMarthWork::Final_WindowHit);
            }
        }
    }
}

bool ftMarth::notifyEventCollisionAttackCheck(u32 flags) {
    // A confirmed Final Smash hit requests the next attack phase.
    if (m_moduleAccesser->getStatusModule().getStatusKind() == ftMarthStatus::FinalDash &&
        m_moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::Final_HitConnected)) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(ftMarthStatus::FinalHit, m_moduleAccesser);
        return true;
    }
    if (m_moduleAccesser->getStatusModule().getStatusKind() == ftMarthStatus::FinalHit &&
        m_moduleAccesser->getWorkManageModule().isFlag(ftMarthWork::Final_WindowHit)) {
        m_moduleAccesser->getWorkManageModule().offFlag(ftMarthWork::Final_WindowHit);
    }
    return Fighter::notifyEventCollisionAttackCheck(flags);
}

void ftMarth::photoMoved() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getStatusModule().getStatusKind() == ftMarthStatus::FinalHit) {
        int count = moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowCount);
        if (count > 0) {
            for (int i = 0; i < count; i++) {
                IfMarthFinalTask* window = dynamic_cast<IfMarthFinalTask*>(gfTask::getTask(moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowTaskId + i)));
                if (window != NULL) {
                    window->setVisibilityWhole(false);
                }
            }
        }
    }
}

void ftMarth::photoExit() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getStatusModule().getStatusKind() == ftMarthStatus::FinalHit) {
        int count = moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowCount);
        if (count > 0) {
            for (int i = 0; i < count; i++) {
                IfMarthFinalTask* window = dynamic_cast<IfMarthFinalTask*>(gfTask::getTask(moduleAccesser->getWorkManageModule().getInt(ftMarthWork::Final_WindowTaskId + i)));
                if (window != NULL) {
                    window->setVisibilityWhole(true);
                }
            }
        }
    }
}
