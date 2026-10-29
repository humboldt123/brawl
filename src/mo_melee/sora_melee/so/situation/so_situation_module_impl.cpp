#include <ac/ac_anim_cmd_impl.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_system.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stop/so_stop_module_impl.h>
#include <types.h>

static inline s16 getStatusManageId(soModuleAccesser* acc, soEventObserverRegistrationDesc* desc) {
    if (desc->checkStatus() == true) {
        return static_cast<soEventManager&>(acc->getEventManageModule()).getManageId();
    }
    return -1;
}

static inline s16 getAnimCmdManageId(soModuleAccesser* acc, soEventObserverRegistrationDesc* desc) {
    if (desc->checkAnimCmd() == true) {
        return static_cast<soEventManager&>(acc->getEventManageModule()).getManageId();
    }
    return -1;
}

soSituationModuleImpl::soSituationModuleImpl(s16 unitId, soModuleAccesser* acc, soEventObserverRegistrationDesc* regDesc) :
    soStatusEventObserver(getStatusManageId(acc, regDesc), (s8)-1),
    soAnimCmdEventObserver(getAnimCmdManageId(acc, regDesc), (s8)-1),
    soEventPresenter<soSituationEventObserver>(unitId),
    m_moduleAccesser(acc), m_kind(Situation_Outfield), m_prevKind(Situation_Outfield), m_lastKind(Situation_Outfield)
{
    m_isFixed = false;
    m_isKeepAir = false;
}

soSituationModule::~soSituationModule() { }

soSituationModuleImpl::~soSituationModuleImpl() { }

void soSituationModuleImpl::deactivate() {
    m_kind = Situation_Outfield;
}

void soSituationEventObserver::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { }

void soSituationModuleImpl::update() {
    if (m_isFixed == true) {
        return;
    }
    if (m_kind == Situation_Cliff) {
        if (!m_moduleAccesser->getGroundModule().isStCliff(0)) {
            if (m_moduleAccesser->getGroundModule().isTouch(8, 0) == true) {
                setKind(Situation_Ground, false);
            } else {
                setKind(Situation_Air, false);
            }
        }
    } else if (m_moduleAccesser->getGroundModule().isTouch(8, 0) == true && !m_moduleAccesser->getGroundModule().isStCliff(0)) {
        if (m_kind == Situation_Air && (m_moduleAccesser->getStopModule().isStop() == true || m_isKeepAir == true)) {
            setKind(Situation_Air, false);
        } else {
            setKind(Situation_Ground, false);
        }
    } else if (m_kind != Situation_Ladder) {
        setKind(Situation_Air, false);
    }
}

bool soSituationModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 1) {
        return false;
    }
    switch (acmd->getType()) {
    case 0: {
        acCmdArg arg;
        if (!acmd->getArg(&arg, 0) || arg.getArgType() != 0) {
            return true;
        }
        setKind((SituationKind)arg.getIntData(), false);
        return true;
    }
    }
    return false;
}

bool soSituationModuleImpl::isObserv(char unk1) {
    return unk1 == 9;
}

SituationKind soSituationModuleImpl::getPrevKind() {
    return (SituationKind)m_prevKind;
}

bool soSituationModuleImpl::isSituationChanged() {
    return m_lastKind != m_prevKind;
}

void soSituationModuleImpl::setKeepAir(bool isKeepAir) {
    m_isKeepAir = isKeepAir;
}

SituationKind soSituationModuleImpl::getKind() {
    return (SituationKind)m_kind;
}
