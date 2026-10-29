#include <wn/fox/wn_fox_land_master.h>
#include <wn/wn_extend_param_accesser.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/damage/so_damage.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/so_value_accesser.h>
#include <so/so_module_hyp.h>
#include <mt/mt_vector.h>
#include <so/so_slow.h>
#include <so/status/so_status_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <ft/ft_entry.h>
#include <types.h>

// HYPOTHESIS: the status change notice the Landmaster sends to its link parents (event 0x83A with the new status kind).
class wnFoxLandMasterChangeStatusEvent : public soLinkEventArgs {
public:
    int m_statusKind;
    wnFoxLandMasterChangeStatusEvent(int eventKind, int statusKind) : soLinkEventArgs(eventKind), m_statusKind(statusKind) { }
};

// HYPOTHESIS: the link event 0x3E carries the stage object it concerns.
class wnFoxLandMasterStageObjectEvent : public soLinkEventArgs {
public:
    StageObject* m_stageObject;
};

void wnFoxLandMaster::processUpdate() {
    Weapon::processUpdate();
    if (soSlow::getInstance()->isEstimate() == true) {
        int statusKind = m_moduleAccesser->getStatusModule().getStatusKind();
        if (statusKind != 8) {
            m_moduleAccesser->getWorkManageModule().decInt(0x10000007);
        }
        if (statusKind != 6) {
            m_moduleAccesser->getWorkManageModule().addFloat(m_param->unk90, 0x11000001);
        } else if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
            float speedY = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL)).m_y;
            float decrease = 1.0f;
            if (speedY >= m_param->unk94) {
                decrease *= m_param->unk98;
            }
            m_moduleAccesser->getWorkManageModule().subFloat(decrease, 0x11000001);
        }
        if (m_moduleAccesser->getWorkManageModule().getFloat(0x11000001) > m_param->unk58) {
            m_moduleAccesser->getWorkManageModule().setFloat(m_param->unk58, 0x11000001);
        }
        m_moduleAccesser->getWorkManageModule().decInt(0x10000006);
        if (!m_moduleAccesser->getWorkManageModule().isFlag(0x1200000C) &&
            m_moduleAccesser->getWorkManageModule().getInt(0x10000006) <= m_param->unk50) {
            m_moduleAccesser->getWorkManageModule().onFlag(0x1200000C);
            m_moduleAccesser->getEffectModule().reqCommon(0, 0.0f);
        }
    }
}

void wnFoxLandMaster::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender,
                                      int unk4) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (sender->soGetKind() == StageObject_Fighter) {
        switch (sender->soGetSubKind()) {
        case Fighter_Fox:
        case Fighter_Falco:
        case Fighter_Wolf:
            switch (eventInfo->m_eventKind) {
            case 0x456:
                work.onFlag(0x22000001);
                return;
            case 0x457:
                work.onFlag(0x22000002);
                return;
            case 0x458:
                work.onFlag(0x22000003);
                return;
            case 0x459:
                work.onFlag(0x22000004);
                return;
            case 0:
            case 2:
            case 4:
                deactivate(false);
                break;
            }
            break;
        }
    } else {
        switch (eventInfo->m_eventKind) {
        case 0x3E:
            switch (static_cast<wnFoxLandMasterStageObjectEvent*>(eventInfo)->m_stageObject->soGetSubKind()) {
            case 5:
            case 15:
                moduleAccesser->getLinkModule().sendEventParents(3, *eventInfo);
                break;
            }
            break;
        }
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}

bool wnFoxLandMaster::notifyEventCollisionAttackCheck(u32 flags) {
    return false;
}

void wnFoxLandMaster::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk4,
                                              soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    Weapon::notifyEventCollisionHit(power, attackData, index, unk4, moduleAccesser, collisionLog);
    m_moduleAccesser->getWorkManageModule().subInt(m_param->unk40 * power, 0x10000006);
}

void wnFoxLandMaster::notifyEventOnDamage(soDamage* damage, bool isReaction, soModuleAccesser* moduleAccesser) {
    soStopModule& stop = moduleAccesser->getStopModule();
    if (damage->m_reaction > 0.0f && !stop.isStop()) {
        int frame = soDamageUtilActor::getDamageHitStopFrame(moduleAccesser, damage, 1.0f, isReaction) >> 1;
        if (frame > 0) {
            moduleAccesser->getStopModule().setHitStopFrame(frame, 1);
            Vec2f offset(0.0f, 1.0f);
            getShakeModule(m_moduleAccesser).req(0, soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC3, 0), 0, &offset);
        }
    }
}

void wnFoxLandMaster::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                              soModuleAccesser* moduleAccesser) {
    wnFoxLandMasterChangeStatusEvent event(0x83A, statusKind);
    m_moduleAccesser->getLinkModule().sendEventParents(3, event);
}

template class wnExtendParamAccesser<3999, 45, 23999, 5, 0, 0>;
