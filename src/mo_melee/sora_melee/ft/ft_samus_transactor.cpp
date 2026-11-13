#include <StaticAssert.h>
#include <ft/samus/ft_samus_transactor.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/samus/wn_samus_cshot.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftSamusTransactor::ftSamusTransactor() { }
ftSamusTransactor::~ftSamusTransactor() { }

// HYPOTHESIS: the three work ints hold the Charge Shot effect handles (exit removes the first one).
void ftSamusTransactor::init(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setInt(0, 0x10000040);
    work.setInt(0, 0x10000041);
    work.setInt(0, 0x10000042);
}

void ftSamusTransactor::exit(soModuleAccesser* moduleAccesser) {
    int effectHandle = moduleAccesser->getWorkManageModule().getInt(0x10000040);
    if (effectHandle != 0) {
        moduleAccesser->getEffectModule().remove(effectHandle);
    }
}

inline bool ftSamusTransactor::activeArticle1(wnSamusCShot* shot, soModuleAccesser* acc, u32 etcResId) {
    soTeamModule& teamModule = acc->getTeamModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    shot->activate(founderTaskId, etcResId, teamModule.getTeam()->getNo(), acc->getStageObject().soGetSubKind());
    return true;
}

// The Charge Shot is article 0xd; its kind is the fighter's sub kind (Samus or Kirby).
bool ftSamusTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnSamusCShot* shot = dynamic_cast<wnSamusCShot*>(article);
        if (shot == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle1(shot, acc, resources.getKirbyResId(Fighter_Samus));
    }
    return false;
}

void ftSamusTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    processUpdateSpecialNTransact(moduleAccesser);
}
