#include <StaticAssert.h>
#include <ft/zelda/ft_sheik_transactor.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/sheik/wn_sheik_needle.h>
#include <wn/sheik/wn_sheik_needle_have.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftSheikTransactor::ftSheikTransactor() { }
ftSheikTransactor::~ftSheikTransactor() { }

// Both articles use Sheik's Kirby resource id; the held needles' activator is inlined.
bool ftSheikTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnSheikNeedle* needle = dynamic_cast<wnSheikNeedle*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle1(needle, acc, resources.getKirbyResId(Fighter_Sheik));
    }
    case 0xe: {
        wnSheikNeedleHave* needles = dynamic_cast<wnSheikNeedleHave*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle2(needles, acc, resources.getKirbyResId(Fighter_Sheik));
    }
    default:
        return false;
    }
}

bool ftSheikTransactor::activeArticle2(wnSheikNeedleHave* weapon, soModuleAccesser* acc, u32 etcResId) {
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, etcResId, acc->getTeamModule().getTeam()->getNo());
    return true;
}
