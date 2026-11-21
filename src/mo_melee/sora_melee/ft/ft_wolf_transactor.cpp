#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/wolf/ft_wolf_transactor.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/wolf/wn_wolf_blaster.h>
#include <wn/wolf/wn_wolf_blaster_bullet.h>

ftWolfTransactor::ftWolfTransactor() { }
ftWolfTransactor::~ftWolfTransactor() { }

bool ftWolfTransactor::activeArticle1(wnWolfBlaster* weapon, soModuleAccesser* acc, u32 mdlResId, u32 texResId, bool unk1, bool unk2) {
    weapon->activate(acc->getStageObject().getTaskId(), mdlResId, texResId, unk1, unk2, acc->getTeamModule().getTeam()->getNo());
    return true;
}

// Fires a bullet from the blaster node with the air or ground muzzle offset and the fixed shot angle (Fox's without the
// aimed status).
bool ftWolfTransactor::activeArticle2(wnWolfBlasterBullet* weapon, soModuleAccesser* acc, int nodeId, u32 etcResId, u32 effectId) {
    if (acc->getSituationModule().getKind() == 2) {
        float offsetZ = acc->getConstantFloatKirby(0xfa2);
        Vec3f offset(0.0f, acc->getConstantFloatKirby(0xfa3), offsetZ);
        Vec3f pos = acc->getModelModule().getNodeGlobalPosition(nodeId, &offset, false, false);
        pos.m_z = 0.0f;
        weapon->activate(acc->getStageObject().getTaskId(), etcResId, acc->getTeamModule().getTeam()->getNo(), &pos, 0,
                         static_cast<EfID>(effectId), acc->getPostureModule().getLr(), acc->getConstantFloatKirby(0xfa5),
                         acc->getConstantFloatKirby(0xfa4));
    } else {
        float offsetZ = acc->getConstantFloatKirby(0xfa0);
        Vec3f offset(0.0f, acc->getConstantFloatKirby(0xfa1), offsetZ);
        Vec3f pos = acc->getModelModule().getNodeGlobalPosition(nodeId, &offset, false, false);
        pos.m_z = 0.0f;
        weapon->activate(acc->getStageObject().getTaskId(), etcResId, acc->getTeamModule().getTeam()->getNo(), &pos, 0,
                         static_cast<EfID>(effectId), acc->getPostureModule().getLr(), acc->getConstantFloatKirby(0xfa5),
                         acc->getConstantFloatKirby(0xfa4));
    }
    return false;
}

// The blaster is article 0xd and its bullet 0xe; both use Wolf's Kirby resource id.
bool ftWolfTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnWolfBlaster* blaster = dynamic_cast<wnWolfBlaster*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        u32 resId = resources.getKirbyResId(Fighter_Wolf);
        return activeArticle1(blaster, acc, resId, resId, true, true);
    }
    case 0xe: {
        wnWolfBlasterBullet* bullet = dynamic_cast<wnWolfBlasterBullet*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle2(bullet, acc, 0x1b8, resources.getKirbyResId(Fighter_Wolf), 0x1280001);
    }
    default:
        return false;
    }
}
