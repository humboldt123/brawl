#include <StaticAssert.h>
#include <ft/zelda/ft_sheik_transactor.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <mt/mt_prng.h>
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

// A thrown needle starts at a random height spread around the hand (offset table of nine steps).
bool ftSheikTransactor::activeArticle1(wnSheikNeedle* weapon, soModuleAccesser* acc, u32 etcResId) {
    float spread[9] = { -1.0f, -0.75f, -0.5f, -0.25f, 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
    soPostureModule& posture = acc->getPostureModule();
    soSituationModule& situation = acc->getSituationModule();
    Vec3f pos = posture.getPos();
    float scale = posture.getScale();
    float lr = posture.getLr();
    float groundX = acc->getConstantFloatKirby(0xfa0);
    float groundY = acc->getConstantFloatKirby(0xfa1);
    float airX = acc->getConstantFloatKirby(0xfa2);
    float airY = acc->getConstantFloatKirby(0xfa3);
    if (situation.getKind() == Situation_Ground) {
        pos.m_x += scale * (groundX * lr);
        pos.m_y += scale * (groundY + spread[randi(9)]);
    } else {
        pos.m_x += scale * (airX * lr);
        pos.m_y += scale * (airY + 2.0f * spread[randi(9)]);
    }
    bool flag = acc->getWorkManageModule().isFlag(0x22000012);
    weapon->activate(acc->getStageObject().getTaskId(), etcResId, acc->getTeamModule().getTeam()->getNo(), &pos, flag, situation.getKind(), lr);
    return false;
}

bool ftSheikTransactor::activeArticle2(wnSheikNeedleHave* weapon, soModuleAccesser* acc, u32 etcResId) {
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, etcResId, acc->getTeamModule().getTeam()->getNo());
    return true;
}
