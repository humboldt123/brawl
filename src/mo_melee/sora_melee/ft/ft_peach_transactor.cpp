#include <ft/peach/ft_peach_transactor.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_log.h>
#include <so/article/so_article.h>
#include <wn/peach/wn_peach_kinopio.h>
#include <wn/weapon.h>

ftPeachTransactor::ftPeachTransactor() { }
ftPeachTransactor::~ftPeachTransactor() { }

void ftPeachTransactor::notifyEventChangeSituation(int kind, int prevKind, soModuleAccesser* acc) {
    if (kind == 2) return;
    acc->getWorkManageModule().offFlag(0x1200003D);
}

// While Peach is in status 0x112 the work int 0x20000002 is reset to -1 (the same reset as updateStatus112).
void ftPeachTransactor::processUpdate(soModuleAccesser* acc) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
        0, acc->getStatusModule().getStatusKind(), acc, true);
    if (status == 0x112) {
        acc->getWorkManageModule().setInt(-1, 0x20000002);
    }
}

void ftPeachTransactor::updateStatus112(soModuleAccesser* acc) {
    acc->getWorkManageModule().setInt(-1, 0x20000002);
}

// A shield hit with a positive power stores the group index in the work int 0x20000002 while Peach is in status 0x112.
// HYPOTHESIS: the meaning of the group index and of the work int.
void ftPeachTransactor::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* acc) {
    if (collisionLog && power > 0.0f) {
        int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
            0, acc->getStatusModule().getStatusKind(), acc, true);
        if (status == 0x112) {
            acc->getWorkManageModule().setInt(groupIndex, 0x20000002);
        }
    }
}

// HYPOTHESIS: the stored group is compared with the id of the status 1 (converted for Kirby); on a match the fighter goes
// to the converted status 0x11d and the stop module is set from the constant 0x5dc0.
bool ftPeachTransactor::notifyEventCollisionShieldCheck(soModuleAccesser* acc) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
        0, acc->getStatusModule().getStatusKind(), acc, true);
    int shieldStatus = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(2, 1, acc, true);
    if (status == 0x112 && static_cast<u32>(shieldStatus) == acc->getWorkManageModule().getInt(0x20000002)) {
        int next = g_ftKirbyCopyAbilityIdConverter.convOrigToCorrectId(0, 0x11d, acc, false);
        acc->getStatusModule().changeStatusRequest(next, acc);
        soStopModule& stop = acc->getStopModule();
        stop.setHitStop(acc->getConstantIntKirby(0x5dc0), 0);
        return true;
    }
    return false;
}

// Kirby's copy of the Toad: the article of id 0xd is started with the Kirby resource id of Peach (fighter kind 0xc).
// activeArticle1 is inlined here.
bool ftPeachTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnPeachKinopio* weapon = dynamic_cast<wnPeachKinopio*>(article);
        if (weapon == NULL) return false;
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle1(weapon, acc, resources.getKirbyResId(Fighter_Peach));
    }
    return false;
}

bool ftPeachTransactor::activeArticle1(wnPeachKinopio* weapon, soModuleAccesser* acc, u32 etcResId) {
    soPostureModule& posture = acc->getPostureModule();
    soTeamModule& teamModule = acc->getTeamModule();
    int founderTaskId = acc->getStageObject().getTaskId();
    weapon->activate(founderTaskId, etcResId, teamModule.getTeam()->getNo(), posture.getLr());
    return true;
}
