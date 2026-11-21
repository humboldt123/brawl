#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/fox/ft_fox_transactor.h>
#include <so/so_module_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module_impl.h>
#include <math.h>
#include <wn/fox/wn_fox_blaster.h>
#include <wn/fox/wn_fox_blaster_bullet.h>

ftFoxTransactor::ftFoxTransactor() { }
ftFoxTransactor::~ftFoxTransactor() { }

bool ftFoxTransactor::activeArticle1(wnFoxBlaster* weapon, soModuleAccesser* acc, u32 mdlResId, u32 texResId, bool unk1, bool unk2) {
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, mdlResId, texResId, unk1, unk2, acc->getTeamModule().getTeam()->getNo());
    return true;
}

// The blaster is article 0xd and its bullet 0xe; both use Fox's Kirby resource id.
bool ftFoxTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    switch (article->getArticleId()) {
    case 0xd: {
        wnFoxBlaster* blaster = dynamic_cast<wnFoxBlaster*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        u32 resId = resources.getKirbyResId(Fighter_Fox);
        return activeArticle1(blaster, acc, resId, resId, true, true);
    }
    case 0xe: {
        wnFoxBlasterBullet* bullet = dynamic_cast<wnFoxBlasterBullet*>(article);
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle2(bullet, acc, 0x1b8, resources.getKirbyResId(Fighter_Fox), 0x10a0001);
    }
    default:
        return false;
    }
}

// Fires a bullet from the blaster node. In status 0x3c the muzzle offset is aimed from the node (mode 1, the angle follows
// the offset); otherwise the air or ground muzzle offset and the fixed shot angle are used.
bool ftFoxTransactor::activeArticle2(wnFoxBlasterBullet* weapon, soModuleAccesser* acc, int nodeId, u32 etcResId, u32 effectId) {
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
