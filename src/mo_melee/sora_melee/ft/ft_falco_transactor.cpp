#include <StaticAssert.h>
#include <ft/falco/ft_falco_transactor.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/falco/wn_falco_blaster.h>
#include <wn/falco/wn_falco_blaster_bullet.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftFalcoTransactor::ftFalcoTransactor() { }
ftFalcoTransactor::~ftFalcoTransactor() { }

bool ftFalcoTransactor::activeArticle1(wnFalcoBlaster* weapon, soModuleAccesser* acc, u32 mdlResId, u32 texResId, bool unk1, bool unk2) {
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, mdlResId, texResId, unk1, unk2, acc->getTeamModule().getTeam()->getNo());
    return true;
}

// The blaster is article 0xd and its bullet 0xe; both use Falco's Kirby resource id.
bool ftFalcoTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnFalcoBlaster* blaster = dynamic_cast<wnFalcoBlaster*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        u32 resId = resources.getKirbyResId(Fighter_Falco);
        return activeArticle1(blaster, acc, resId, resId, true, true);
    }
    case 0xe: {
        wnFalcoBlasterBullet* bullet = dynamic_cast<wnFalcoBlasterBullet*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle2(bullet, acc, 0x1b8, resources.getKirbyResId(Fighter_Falco), 0x1160001);
    }
    default:
        return false;
    }
}