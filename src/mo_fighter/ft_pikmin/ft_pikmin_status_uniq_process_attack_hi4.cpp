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
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>

// Olimar's up smash: the start takes the first Pikmin into the hand (Pikmin status 0x1b) and keeps it turned to his facing, the
// release hands the Pikmin the charge of the attack (power multiplier); everything else is the smash attack handling of the
// common process.
// HYPOTHESIS: the status numbers 0x31 and 0x32 (the smash start and hold) and the Pikmin statuses.
void ftPikminStatusUniqProcessAttackHi4Start::initStatus(soModuleAccesser* moduleAccesser) {
    m_troops->holdPikmin(1);
    if (m_troops->getHoldPikminNum() > 0) {
        m_troops->getHoldPikminTop()->changeStatus(0x1b);
        wnPikminPikminTroopsManager* troops = m_troops;
        troops->syncLr(troops->getHoldPikminTop(), moduleAccesser->getPostureModule().getLr());
    }
}

void ftPikminStatusUniqProcessAttackHi4Start::execFixPos(soModuleAccesser* moduleAccesser) {
    if (m_troops->getHoldPikminNum() > 0) {
        wnPikminPikminTroopsManager* troops = m_troops;
        troops->syncLr(troops->getHoldPikminTop(), moduleAccesser->getPostureModule().getLr());
    }
}

void ftPikminStatusUniqProcessAttackHi4Start::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (nextStatusKind != 0x31 && nextStatusKind != 0x32) {
        if (m_troops->getHoldPikminNum() > 0) {
            m_troops->getHoldPikminTop()->changeStatus(0xa);
            ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
        }
    }
}

void ftPikminStatusUniqProcessAttackHi4Hold::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    ftStatusUniqProcessSmashAttack::exitStatus(moduleAccesser, nextStatusKind);
    if (nextStatusKind != 0x32) {
        if (m_troops->getHoldPikminNum() > 0) {
            m_troops->getHoldPikminTop()->changeStatus(0xa);
            ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
        }
    }
}

void ftPikminStatusUniqProcessAttackHi4::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessSmashAttack::initStatus(moduleAccesser);
    if (m_troops->getHoldPikminNum() > 0) {
        wnPikminPikminTroopsManager* troops = m_troops;
        troops->setPowerMulStatus(troops->getHoldPikminTop(), moduleAccesser->getCollisionAttackModule().getPowerMulStatus());
    }
}

void ftPikminStatusUniqProcessAttackHi4::execFixPos(soModuleAccesser* moduleAccesser) {
    static_cast<soStatusUniqProcess*>(&g_ftPikminStatusUniqProcessSmashAttack)->execFixPos(moduleAccesser);
}

void ftPikminStatusUniqProcessAttackHi4::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    ftStatusUniqProcessSmashAttack::exitStatus(moduleAccesser, nextStatusKind);
    if (m_troops->getHoldPikminNum() > 0) {
        m_troops->getHoldPikminTop()->changeStatus(0xa);
        ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
    }
}

// The hold Pikmin stands at the node 0 moved by the constant offset (turned to his facing, scaled with him).
// HYPOTHESIS: the constants 0xfa6/0xfa7.
void ftPikminStatusUniqProcessAttackHi4::updateHoldPikmin(soModuleAccesser* moduleAccesser) {
    if (m_troops->getHoldPikminNum() > 0) {
        Vec2f nodePos(moduleAccesser->getModelModule().getNodeGlobalPosition(0, false));
        Vec2f offset(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0), soValueAccesser::getConstantFloat(moduleAccesser, 0xfa7, 0));
        float scale = moduleAccesser->getPostureModule().getScale();
        offset.m_x *= scale;
        offset.m_y *= scale;
        if (moduleAccesser->getPostureModule().getLr() < 0.0f) {
            offset.m_x = -offset.m_x;
        }
        soArrayVector<Vec2f, 1> spots;
        spots.push(nodePos + offset);
        m_troops->updateHoldPikmin(reinterpret_cast<const soArrayFixed<wnPikminPikminOwnerInfo>*>(&spots));
    }
}
