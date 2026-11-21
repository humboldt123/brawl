#define WN_DEDEDE_STARMISSILE_ACTIVATE_ONLY
#include <StaticAssert.h>
#include <ft/dedede/ft_dedede_transactor.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_manager.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <gf/gf_task_scheduler.h>
#include <so/collision/so_collision_log.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <wn/dedede/wn_dedede_starmissile.h>


ftDededeTransactor::ftDededeTransactor() { }
ftDededeTransactor::~ftDededeTransactor() { }

// Inhale statuses (0x112, 0x117): once the search caught an article (work flag 0x22000019), link it, tell it it was
// swallowed (link event 0x835) and change to the swallow status (Kirby's copy has its own).
bool ftDededeTransactor::notifyEventCollisionSearchCheck(soModuleAccesser* moduleAccesser) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    if ((status == 0x112 || status == 0x117) && moduleAccesser->getWorkManageModule().isFlag(0x22000019)) {
        int caughtTaskId = moduleAccesser->getWorkManageModule().getInt(0x20000006);
        moduleAccesser->getLinkModule().link(6, caughtTaskId);
        soLinkEventArgs args(0x835);
        moduleAccesser->getLinkModule().sendEventParents(6, args);
        moduleAccesser->getWorkManageModule().onFlag(0x2200001a);
        if (moduleAccesser->getStageObject().soGetSubKind() == Fighter_Kirby) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x1a3, moduleAccesser);
        } else {
            moduleAccesser->getStatusModule().changeStatusRequest(0x127, moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000019);
        return true;
    }
    return false;
}

// The star missile is article 0xd (Dedede's Kirby resource id).
bool ftDededeTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnDededeStarmissile* missile = dynamic_cast<wnDededeStarmissile*>(article);
        if (missile == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle(missile, acc, resources.getKirbyResId(Fighter_Dedede));
    }
    return false;
}

// The fighter linked as the node parent.
static inline Fighter* getLinkOwnerFighter(soModuleAccesser* acc) {
    ftManager* manager = g_ftManager;
    return manager->getFighter(manager->getEntryIdFromTaskId(acc->getLinkModule().getNodeTaskId(0), 0), -1);
}

// The star missile starts in front of the mouth node; its scale and kind come from the attribute block of the fighter
// linked as the node parent (the copy owner when Kirby spits it). Its task id is kept in work 0x20000004.
bool ftDededeTransactor::activeArticle(wnDededeStarmissile* weapon, soModuleAccesser* acc, u32 etcResId) {
    float lr = acc->getPostureModule().getLr();
    Vec3f pos;
    Vec3f offset(0.0f, 0.0f, 0.0f);
    float scale = acc->getPostureModule().getScale();
    offset.m_x = scale * acc->getConstantFloatKirby(0xfa5);
    offset.m_y = scale * acc->getConstantFloatKirby(0xfa6);
    pos = acc->getModelModule().getNodeGlobalPosition(0, false);
    pos.m_x += offset.m_x * lr;
    pos.m_y += offset.m_y;
    pos.m_z = acc->getGroundModule().getZ();
    ftParam param = *g_ftCommonDataAccesser.getParam(getLinkOwnerFighter(acc)->getFtKind());
    float starScale = param.unk1A4;
    s32 starParam = param.unk1A8;
    weapon->activate(acc->getStageObject().getTaskId(), etcResId, acc->getTeamModule().getTeam()->getNo(), &pos,
        acc->getPostureModule().getLr(), starScale, starParam, 0);
    int taskId = weapon->getTaskId();
    acc->getWorkManageModule().setInt(taskId, 0x20000004);
    return false;
}

// Inhale statuses (0x112, 0x117) before anything was caught: a search hit on an article that can be eaten marks what was hit;
// Dedede's own star (0x33) and the Waddle Doo beam (0x37, kind 2) are caught (work flag 0x22000019, task id kept).
void ftDededeTransactor::notifyEventCollisionSearch(soCollisionLog* log, soModuleAccesser* moduleAccesser) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true);
    if ((status == 0x112 || status == 0x117) && !moduleAccesser->getWorkManageModule().isFlag(0x22000019)) {
        gfTask* task = gfTaskScheduler::getInstance()->getTask(log->m_taskId);
        if (task != NULL && task->m_taskCategory == gfTask::Category_Weapon) {
            Weapon* weapon = dynamic_cast<Weapon*>(task);
            moduleAccesser->getWorkManageModule().setInt(0, 0x20000007);
            if (weapon->isCanEat()) {
                switch (weapon->getSubKind()) {
                case 0x37:
                    moduleAccesser->getWorkManageModule().setInt(2, 0x20000007);
                case 0x33:
                    moduleAccesser->getWorkManageModule().onFlag(0x22000019);
                    moduleAccesser->getWorkManageModule().setInt(log->m_taskId, 0x20000006);
                    break;
                }
            }
        }
    }
}
