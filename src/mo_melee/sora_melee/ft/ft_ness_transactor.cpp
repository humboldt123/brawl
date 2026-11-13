#include <StaticAssert.h>
#include <ft/ness/ft_ness_transactor.h>
#include <ft/ft_entry.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/ness/wn_ness_pkflash.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftNessTransactor::ftNessTransactor() { }
ftNessTransactor::~ftNessTransactor() { }

// The PK Flash is article 0xd.
bool ftNessTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnNessPKFlash* weapon = dynamic_cast<wnNessPKFlash*>(article);
        return activeArticle1(weapon, acc);
    }
    default:
        return false;
    }
}

bool ftNessTransactor::shootArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnNessPKFlash* weapon = dynamic_cast<wnNessPKFlash*>(article);
        return shootArticle1(weapon, acc);
    }
    default:
        return false;
    }
}

// PK Flash: starts at the head node (Kirby's copy uses its own node), raised by 3 units scaled with the fighter.
bool ftNessTransactor::activeArticle1(wnNessPKFlash* weapon, soModuleAccesser* acc) {
    int node = acc->getStageObject().soGetSubKind() == Fighter_Kirby ? 0x198 : 0x2a;
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(node, false);
    float lr = acc->getPostureModule().getLr();
    pos.m_y += 3.0f * acc->getPostureModule().getScale();
    pos.m_z = 0.0f;
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, acc->getTeamModule().getTeam()->getNo(), &pos, lr);
    return true;
}

bool ftNessTransactor::shootArticle1(wnNessPKFlash* weapon, soModuleAccesser* acc) {
    weapon->shoot();
    return false;
}
