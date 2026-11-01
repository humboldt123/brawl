#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/fighter.h>
#include <ft/ft_status_uniq_process_smash_attack.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_status_uniq_process.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// Olimar's smash attacks: the Pikmin in his hand take over the charge (the power multiplier of his attack module) while he holds
// the smash, are handed the matching Pikmin status when it is released (the throw of the status 0x2c turns it to his facing) and
// go back into the line when the status ends.
// HYPOTHESIS: the statuses (0x2c, 0x2f, 0x32 are the smash release statuses) and the Pikmin status numbers 0x10, 0x13, 0x1c, 0xa.
void ftPikminStatusUniqProcessSmashAttack::initStatus(soModuleAccesser* moduleAccesser) {
    static_cast<soStatusUniqProcess*>(&g_ftStatusUniqProcessSmashAttack)->initStatus(moduleAccesser);
    ftPikmin* pikmin = dynamic_cast<ftPikmin*>(&moduleAccesser->getStageObject());
    if (pikmin != NULL) {
        wnPikminPikminTroopsManager& troops = pikmin->getTroops();
        if (troops.getHoldPikminNum() > 0) {
            troops.setPowerMulStatus(troops.getHoldPikminTop(), moduleAccesser->getCollisionAttackModule().getPowerMulStatus());
            const soArrayList<wnPikminPikmin*, 10>* hold = troops.getHoldPikminList();
            if (hold->size() > 1) {
                soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
                wnPikminPikmin* const& second = hold->at(1);
                troops.setPowerMulStatus(second, attack.getPowerMulStatus());
            }
            troops.setPowerMulStatus(troops.getHoldPikminTop(), moduleAccesser->getCollisionAttackModule().getPowerMulStatus());
        }
    }
}

void ftPikminStatusUniqProcessSmashAttack::execFixPos(soModuleAccesser* moduleAccesser) {
    ftPikmin* pikmin = dynamic_cast<ftPikmin*>(&moduleAccesser->getStageObject());
    if (pikmin != NULL) {
        wnPikminPikminTroopsManager& troops = pikmin->getTroops();
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
            moduleAccesser->getWorkManageModule().offFlag(0x22000010);
            if (troops.getHoldPikminNum() > 0) {
                switch (moduleAccesser->getStatusModule().getStatusKind()) {
                case 0x2c:
                    troops.syncLr(troops.getHoldPikminTop(), moduleAccesser->getPostureModule().getLr());
                    troops.getHoldPikminTop()->changeStatus(0x10);
                    break;
                case 0x32:
                    troops.getHoldPikminTop()->changeStatus(0x1c);
                    break;
                case 0x2f: {
                    const soArrayList<wnPikminPikmin*, 10>* hold = troops.getHoldPikminList();
                    hold->at(0)->changeStatus(0x13);
                    if (hold->size() > 1) {
                        hold->at(1)->changeStatus(0x13);
                        soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
                        wnPikminPikmin* const& second = hold->at(1);
                        troops.setPowerMulStatus(second, attack.getPowerMulStatus());
                    }
                    break;
                }
                }
                troops.setPowerMulStatus(troops.getHoldPikminTop(), moduleAccesser->getCollisionAttackModule().getPowerMulStatus());
                ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
            }
        }
    }
}

void ftPikminStatusUniqProcessSmashAttack::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    static_cast<soStatusUniqProcess*>(&g_ftStatusUniqProcessSmashAttack)->exitStatus(moduleAccesser, nextStatusKind);
    ftPikmin* pikmin = dynamic_cast<ftPikmin*>(&moduleAccesser->getStageObject());
    if (pikmin != NULL) {
        wnPikminPikminTroopsManager& troops = pikmin->getTroops();
        if (troops.getHoldPikminNum() > 0) {
            const soArrayList<wnPikminPikmin*, 10>* hold = troops.getHoldPikminList();
            for (int i = 0; i < hold->size(); i++) {
                hold->at(i)->changeStatus(0xa);
            }
            ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
        }
    }
}

ftPikminStatusUniqProcessSmashAttack g_ftPikminStatusUniqProcessSmashAttack;
