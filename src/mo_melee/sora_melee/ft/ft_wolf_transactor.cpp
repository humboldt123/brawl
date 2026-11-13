#include <StaticAssert.h>
#include <ft/wolf/ft_wolf_transactor.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/wolf/wn_wolf_blaster.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftWolfTransactor::ftWolfTransactor() { }
ftWolfTransactor::~ftWolfTransactor() { }

bool ftWolfTransactor::activeArticle1(wnWolfBlaster* weapon, soModuleAccesser* acc, u32 mdlResId, u32 texResId, bool unk1, bool unk2) {
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, mdlResId, texResId, unk1, unk2, acc->getTeamModule().getTeam()->getNo());
    return true;
}