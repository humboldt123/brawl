#include <StaticAssert.h>
#include <ft/falco/ft_falco_transactor.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <math.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <wn/falco/wn_falco_blaster.h>
#include <wn/falco/wn_falco_blaster_bullet.h>

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
// Same as Fox's: fires a bullet from the blaster node. In status 0x3c the muzzle offset is aimed from the node (mode 1, the angle follows
// the offset); otherwise the air or ground muzzle offset and the fixed shot angle are used.
bool ftFalcoTransactor::activeArticle2(wnFalcoBlasterBullet* weapon, soModuleAccesser* acc, int nodeId, u32 etcResId, u32 effectId) {
    if (acc->getStatusModule().getStatusKind() == 0x3c) {
        float offsetZ = acc->getConstantFloatKirby(0xfa0);
        Vec3f offset(0.0f, acc->getConstantFloatKirby(0xfa1), offsetZ);
        Vec3f nodePos = acc->getModelModule().getNodeGlobalPosition(nodeId, false);
        Vec3f pos = acc->getModelModule().getNodeGlobalPosition(nodeId, &offset, false, false);
        pos.m_z = 0.0f;
        float dx = (pos.m_x - nodePos.m_x) * acc->getPostureModule().getLr();
        float dy = pos.m_y - nodePos.m_y;
        float angle = atan2(dy, dx);
        soPostureModule& posture = acc->getPostureModule();
        soTeamModule& team = acc->getTeamModule();
        weapon->activate(acc->getStageObject().getTaskId(), etcResId, team.getTeam()->getNo(), &pos, 1,
                         static_cast<EfID>(effectId), posture.getLr(), acc->getConstantFloatKirby(0xfa5), angle);
    } else if (acc->getSituationModule().getKind() == 2) {
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
