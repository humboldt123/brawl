#include <StaticAssert.h>
#include <ft/diddy/ft_diddy_transactor.h>
#include <wn/diddy/wn_diddy_gun.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_event_presenter.h>
#include <so/stageobject.h>
#include <so/so_module_accesser.h>

ftDiddyTransactor::ftDiddyTransactor() { }
ftDiddyTransactor::~ftDiddyTransactor() { }

// The popgun is article 0xd; it is started with Diddy's Kirby resource id.
bool ftDiddyTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnDiddyGun* weapon = dynamic_cast<wnDiddyGun*>(article);
        if (weapon == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle(weapon, acc, resources.getKirbyResId(Fighter_Diddy));
    }
    return false;
}

// Link event sent to the held popgun (soLinkEventArgs keeps its constructor protected).
class ftDiddyGunLinkEvent : public soLinkEventArgs {
public:
    ftDiddyGunLinkEvent(int eventKind) : soLinkEventArgs(eventKind) {}
};

// Leaving the popgun: the charge statuses keep the gun; the two shoot statuses tell it to fire (link events 0x838 /
// 0x839), any other status removes it.
void ftDiddyTransactor::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, statusKind, moduleAccesser, true)) {
    case 0x10c:
    case 0x112:
    case 0x117:
    case 0x118:
        return;
    case 0x119: {
        ftDiddyGunLinkEvent event(0x838);
        moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        break;
    }
    case 0x11a: {
        ftDiddyGunLinkEvent event(0x839);
        moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        break;
    }
    default:
        moduleAccesser->getGenerateArticleManageModule().removeExist(0xd, 0);
        break;
    }
}

// Started from the popgun statuses the gun is held in the hand the draw animation flagged (left or right hand
// node); otherwise it is free.
bool ftDiddyTransactor::activeArticle(wnDiddyGun* weapon, soModuleAccesser* acc, u32 etcResId) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, acc->getStatusModule().getStatusKind(), acc, true);
    if (status == 0x116) {
        int node;
        if (acc->getWorkManageModule().isFlag(0x22000010)) {
            node = acc->getModelModule().getNodeId("LHaveN");
            acc->getWorkManageModule().offFlag(0x22000010);
        } else if (acc->getWorkManageModule().isFlag(0x22000011)) {
            node = acc->getModelModule().getNodeId("RHaveN");
            acc->getWorkManageModule().offFlag(0x22000011);
        }
        weapon->activate(acc->getStageObject().m_taskId, etcResId, true, node);
    } else if (status == 0x10f) {
        int node;
        if (acc->getWorkManageModule().isFlag(0x22000010)) {
            node = acc->getModelModule().getNodeId("LHaveN");
            acc->getWorkManageModule().offFlag(0x22000010);
        } else if (acc->getWorkManageModule().isFlag(0x22000011)) {
            node = acc->getModelModule().getNodeId("RHaveN");
            acc->getWorkManageModule().offFlag(0x22000011);
        }
        weapon->activate(acc->getStageObject().m_taskId, etcResId, true, node);
    } else {
        weapon->activate(acc->getStageObject().m_taskId, etcResId, false, -1);
    }
    return true;
}
