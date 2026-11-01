#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/fighter.h>
#include <ft/ft_status_uniq_process_catch_pull_impl.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_status_uniq_process.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// The common catch wait process (ft_status_uniq_process_catch_wait.cpp in sora_melee) as its derived classes see it.
struct ftPikminCatchWaitLinkArgs : soLinkEventArgs {
    ftPikminCatchWaitLinkArgs(int eventKind) : soLinkEventArgs(eventKind) { }
};

// Olimar holds an enemy: the Pikmin in his hand is told when the grab hits (work flag 0x22000010 for the catch kind 1), and the link
// event 0x457 is sent to it when the status leaves the catch statuses (0x39..0x3c and 0xf1 keep it).
// HYPOTHESIS: the catch kind meaning and the event number.
void ftPikminStatusUniqProcessCatchWait::initStatus(soModuleAccesser* moduleAccesser) {
    if (m_troops->getHoldPikminNum() > 0) {
        wnPikminPikminTroopsManager* troops = m_troops;
        if (troops->getCatchKind(troops->getHoldPikminTop()) == 1) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        }
    }
}

void ftPikminStatusUniqProcessCatchWait::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    switch (nextStatusKind) {
    case 0x39:
    case 0x3a:
    case 0x3b:
    case 0x3c:
    case 0xf1:
        break;
    default:
        if (m_troops->getHoldPikminNum() > 0) {
            ftPikminCatchWaitLinkArgs args(0x457);
            int taskId = m_troops->getHoldPikminTop()->m_taskId;
            moduleAccesser->getLinkModule().sendEventNodes(1, taskId, args);
        }
        break;
    }
}
