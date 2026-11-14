#define FT_LOG_ACTION_INFO_COPY_CTOR
#include <ft/ft_log_transactor.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <mt/mt_common.h>
#include <mt/mt_vector.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage_attacker_info.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/param/so_param_customize_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the log fields of the common status flag word (ftStatusFlagData) read by this unit.
// logActionKind/logActionId/logActionRestart are passed straight to resetLogActionInfo;
// moveKind 1 marks statuses whose travelled distance is logged separately.
struct ftStatusLogFlagData {
    u32 : 6;
    u32 logInfo : 20;
    u32 : 6;

    int getLogActionKind() const { return logInfo & 0xf; }
    int getLogActionId() const { return (logInfo >> 4) & 0xff; }
    bool isLogActionRestart() const { return (logInfo >> 12) & 1; }
    u32 getMoveKind() const { return (logInfo >> 13) & 7; }
    bool isLogFlag1D() const { return (logInfo >> 16) & 1; }
    bool isLogFlag19() const { return (logInfo >> 17) & 1; }
    bool isLogFlag1B() const { return (logInfo >> 18) & 1; }
};

static inline const ftStatusLogFlagData* getStatusLogFlag(soModuleAccesser* moduleAccesser, int status) {
    return reinterpret_cast<const ftStatusLogFlagData*>(g_ftCommonDataAccesser.getFtStatusData(static_cast<ftKind>(moduleAccesser->m_stageObject->soGetSubKind()), status));
}

void ftLogTransactor::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser, ftOwner* owner) {
    const ftStatusLogFlagData* data = getStatusLogFlag(moduleAccesser, statusKind);
    const ftStatusLogFlagData* prevData = getStatusLogFlag(moduleAccesser, prevStatusKind);
    int kind = data->getLogActionKind();
    int prevKind = prevData->getLogActionKind();
    int id = data->getLogActionId();
    if (kind == 2) {
        u32 prevId = prevData->getLogActionId();
        bool restart = data->isLogActionRestart();
        if (id == 0x3d && (prevKind != 2 || (int)prevId != 0x3d || restart == true)) {
            owner->addLogInt(1, 0x15, 0);
        }
    } else if (kind == 6) {
        owner->addLogInt(1, 0x15, 0);
    }
    if (prevKind != 3 && kind == 3) {
        owner->addLogInt(1, 0x16, 0);
    }
    if (prevKind != 5 && kind == 5) {
        owner->addLogInt(1, 0x1a, 0);
    }
    int restart = data->isLogActionRestart();
    resetLogActionInfo(kind, id, restart, owner, moduleAccesser);
    if (data->isLogFlag1D() && (!prevData->isLogFlag1D() || restart != 0)) {
        owner->addLogInt(1, 0x1d, 0);
    }
    if (data->isLogFlag19() && (!prevData->isLogFlag19() || restart != 0)) {
        owner->addLogInt(1, 0x19, 0);
    }
    if (data->isLogFlag1B() && (!prevData->isLogFlag1B() || restart != 0)) {
        owner->addLogInt(1, 0x1b, 0);
    }
}

void ftLogTransactor::resetLogActionInfo(int kind, int id, int restart, ftOwner* owner, soModuleAccesser* moduleAccesser) {
    if (kind == 1) {
        return;
    }
    ftLogActionInfo& current = owner->getLog().m_logActionInfo;
    int currentKind = current.unk0;
    int currentId = current.unk4;
    if (currentKind != kind || currentId != id) {
        owner->setLogActionInfo(ftLogActionInfo(kind, id));
        moduleAccesser->getParamCustomizeModule().setAttackPowerMulPattern(1.0f);
    }
    if (kind == 2 && (currentKind != kind || currentId != id || restart != 0)) {
        owner->addLogInt(1, 0xc, 0);
        int serial = owner->getLog().m_logDataAccesser.getInt(0xc, 0);
        moduleAccesser->getWorkManageModule().setInt(serial, 0x10000035);
        if (moduleAccesser->getSituationModule().getKind() == Situation_Air) {
            owner->addLogInt(1, 0xe, 0);
        }
        owner->setLogInt(0, 0x21, 0);
        soLogAttackInfo attackInfo(owner->getLog().m_logDataAccesser.getInt(0xc, 0), id);
        owner->addAttackInfo(attackInfo);
        float powerMul = owner->getLog().m_logPatternModule.getPowerMul(attackInfo);
        moduleAccesser->getParamCustomizeModule().setAttackPowerMulPattern(powerMul);
    }
}

int ftLogTransactor::intrudeLogAttackInfo(int id, int unused, ftOwner* owner) {
    owner->addLogInt(1, 0xc, 0);
    int serial = owner->getLog().m_logDataAccesser.getInt(0xc, 0);
    soLogAttackInfo attackInfo(serial, id);
    owner->addAttackInfo(attackInfo);
    return serial;
}

void ftLogTransactor::processFixPosition(soModuleAccesser* moduleAccesser, ftOwner* owner) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    if (status == 0x10b || moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true) {
        return;
    }
    int moveKind = getStatusLogFlag(moduleAccesser, status)->getMoveKind();

    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(2));
    float length = speed.length();
    if (length != 0.0f) {
        owner->addLogFloat(length, 0xf, 0);
        if (length > owner->getLog().m_logDataAccesser.getFloat(0x10, 0)) {
            owner->setLogFloat(length, 0x10, 0);
        }
        soDamageAttackerInfo attackerInfo;
        moduleAccesser->getDamageModule().getAttackerInfo(&attackerInfo);
        if (attackerInfo.m_indirectTaskCategory == 0xa && g_ftManager->isValidEntryId(attackerInfo.m_indirectEntryId) == true) {
            int entryId = g_ftManager->getEntryIdFromTaskId(attackerInfo.m_indirectTaskId, NULL);
            if (entryId != -1) {
                ftOwner* attacker = g_ftManager->getOwner(entryId);
                if (length > attacker->getLog().m_logDataAccesser.getFloat(0x11, 0)) {
                    attacker->setLogFloat(length, 0x11, 0);
                }
            }
        }
    }

    Vec2f moveSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if (moveSpeed.lengthSq() != 0.0f) {
        if (moveKind == 1) {
            owner->addLogFloat(moveSpeed.length(), 0x14, 0);
        } else if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            owner->addLogFloat(moveSpeed.length(), 0xc, 0);
        } else if (moduleAccesser->getSituationModule().getKind() == Situation_Air) {
            if (moveSpeed.m_y < 0.0f) {
                owner->addLogFloat(__fabs(moveSpeed.m_y), 0xe, 0);
            } else if (moveSpeed.m_y > 0.0f) {
                owner->addLogFloat(__fabs(moveSpeed.m_y), 0xd, 0);
            }
        }
    }

    switch (moduleAccesser->getSituationModule().getKind()) {
        case Situation_Water:
            owner->addLogInt(1, 0x29, 0);
            break;
        case Situation_Ground:
        case Situation_Ladder:
            owner->addLogInt(1, 0x28, 0);
            break;
        case Situation_Cliff:
        case Situation_Air:
            owner->addLogInt(1, 0x27, 0);
            break;
    }

    int frame = owner->getLog().m_logDataAccesser.getInt(0x21, 0);
    if (++frame > owner->getLog().m_logDataAccesser.getInt(0x20, 0)) {
        owner->setLogInt(frame, 0x20, 0);
    }
    owner->setLogInt(frame, 0x21, 0);

    if (moduleAccesser->getWorkManageModule().isFlag(0x12000014) || moduleAccesser->getWorkManageModule().isFlag(0x12000015) ||
        moduleAccesser->getWorkManageModule().isFlag(0x12000012) || moduleAccesser->getWorkManageModule().isFlag(0x12000013)) {
        owner->addLogInt(1, 0x3a, 0);
    }

    float stickThreshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xda5, 0);
    float lrThreshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xda6, 0);
    float bonusThreshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xda7, 0);
    soControllerModule& controller = moduleAccesser->getControllerModule();
    float stickX = controller.getStickX();
    float stickY = controller.getStickY();
    if (mtSqrtf(stickX * stickX + stickY * stickY) >= stickThreshold) {
        if (controller.getFlickBonus() < bonusThreshold) {
            owner->addLogInt(1, 0x1f, 0);
            controller.resetFlickBonus();
        }
    }
    if (__fabs(controller.getLr()) >= lrThreshold) {
        if (controller.getFlickBonusLr() < bonusThreshold) {
            owner->addLogInt(1, 0x1f, 0);
            controller.resetFlickBonusLr();
        }
    }
}

void ftLogTransactor::notifyEventCollisionReflector(soModuleAccesser* moduleAccesser, ftOwner* owner) {
    owner->addLogInt(1, 0x18, 0);
}

// HYPOTHESIS: soLogAttackInfo::_8 counts how often the attack connected. MATCH-ONLY: read through a const view,
// which keeps the native reload of the counter before the increment.
static inline int getHitCount(const soLogAttackInfo* attackInfo) {
    return attackInfo->_8;
}

// Counts the attacks that connected: log int 0xd follows the attack serial 0xc.
static inline void addHitCount(ftOwner* owner) {
    int serial = owner->getLog().m_logDataAccesser.getInt(0xc, 0);
    if (owner->getLog().m_logDataAccesser.getInt(0xd, 0) < serial) {
        owner->addLogInt(1, 0xd, 0);
    }
}

void ftLogTransactor::notifyEventCurrentAttack(ftOwner* owner, StageObject* target) {
    if ((int)owner->getLog().m_logActionInfo.unk0 != 2) {
        return;
    }
    soLogAttackInfo* attackInfo = owner->getLog().m_logAttackInfoModule.getAttackInfo(0);
    if (attackInfo == NULL) {
        return;
    }
    if (getHitCount(attackInfo) > 0) {
        attackInfo->_8++;
    } else {
        owner->addAttackPattern(*attackInfo);
        attackInfo->_8++;
        addHitCount(owner);
    }
}

void ftLogTransactor::notifyEventAttack(ftOwner* owner, u32 serial, StageObject* target) {
    if (serial == 0) {
        return;
    }
    soLogAttackInfo* attackInfo = owner->getLog().m_logAttackInfoModule.getAttackInfo(serial);
    if (attackInfo == NULL) {
        return;
    }
    if (getHitCount(attackInfo) > 0) {
        attackInfo->_8++;
    } else {
        owner->addAttackPattern(*attackInfo);
        attackInfo->_8++;
        addHitCount(owner);
    }
}

void ftLogTransactor::notifyEventHit(ftOwner* owner, int kind, void* unused) {
    if (kind == owner->getLog().m_logDataAccesser.getInt(9, 0)) {
        return;
    }
    owner->setLogInt(kind, 9, 0);
    if (kind == 0x3d) {
        owner->onLogFlag(0x12, 0);
    } else if (kind == 0x6a) {
        owner->onLogFlag(0x13, 0);
    }
}

void ftLogTransactor::notifyEventChangeSituation(ftOwner* owner, u32 kind) {
    if (kind != 1) {
        return;
    }
    if (owner->getLog().m_logDataAccesser.isFlag(0x12, 0) == true) {
        owner->offLogFlag(0x12, 0);
    }
    if (owner->getLog().m_logDataAccesser.isFlag(0x13, 0) == true) {
        owner->offLogFlag(0x13, 0);
    }
}
