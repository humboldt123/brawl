#include <ac/ac_anim_cmd_impl.h>
#include <ft/ft_controller_module_impl.h>
#include <ip/input.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// HYPOTHESIS: soControllerModuleImpl's second constructor parameter (an int in its declaration) receives the log array.
ftControllerModuleImpl::ftControllerModuleImpl(soModuleAccesser* moduleAccesser, s16 unitId, soArray<soControllerImpl>* logs, soArray<soControllerClatter>* clatters) :
    soControllerModuleImpl(unitId, (int)logs, clatters),
    soStatusEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1),
    soTurnEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1),
    m_moduleAccesser(moduleAccesser),
    m_logs(logs),
    m_logNum(-1),
    m_logIndex(-1),
    m_prev(0),
    m_logActive(true) {
    m_currentController = &m_controller;
}

ftControllerModuleImpl::~ftControllerModuleImpl() { }

void ftControllerModuleImpl::activate() {
    m_controller.reset();
    soControllerModuleImpl::activate();
    m_reverseXFrameNext = 0;
    m_reverseXFrame = 0;
    m_logCapacity = m_logs->size();
}

void ftControllerModuleImpl::deactivate() {
    stopRumble(false);
    soControllerModuleImpl::deactivate();
}

void ftControllerModuleImpl::update(Input* input, bool p2) {
    bool isStop = m_moduleAccesser->getStopModule().isStop();
    if (p2 == true) {
        m_controller.update(input, isStop);
    }
    if (m_logActive == true) {
        if (m_logNum < m_logCapacity) {
            m_logNum++;
        }
        if (m_logNum > 0) {
            if (++m_logIndex >= m_logCapacity) {
                m_logIndex = 0;
            }
            m_logs->at(m_logIndex) = m_controller;
        }
    }
    m_controller.update(input, isStop);
    updateClatter(&m_controller);
    if (m_reverseXFrame > 0) {
        m_reverseXFrame--;
    }
}

void ftControllerModuleImpl::setPrev(s32 prev) {
    if (prev == 0) {
        m_currentController = &m_controller;
    } else if (prev > m_logNum) {
        m_currentController = &m_unkBC;
    } else {
        s32 index = m_logIndex - (prev - 1);
        if (index < 0) {
            index += m_logCapacity;
        }
        m_currentController = &m_logs->at(index);
    }
    m_prev = prev;
}

void ftControllerModuleImpl::setLogActive(bool active) {
    m_logActive = active;
    if (!active) {
        m_logNum = -1;
        m_logIndex = -1;
        m_prev = 0;
    }
}

float ftControllerModuleImpl::getClatterThreshold(u32 index) {
    return soValueAccesser::getConstantFloat(m_moduleAccesser, 0xC84, 0);
}

void ftControllerModuleImpl::setRumble(s32 kind, s32 p2, bool loop, s32 p4) {
    u8 flags = 0;
    if (loop == true) {
        flags |= 1;
    }
    soExternalValueAccesser::getInput(m_moduleAccesser->m_stageObject)->setRumble(p4, kind, p2, flags);
}

void ftControllerModuleImpl::stopRumbleKind(s32 kind, s32 p2) {
    soExternalValueAccesser::getInput(m_moduleAccesser->m_stageObject)->removeRumbleId(p2, kind);
}

void ftControllerModuleImpl::stopRumble(bool p1) {
    u8 flags = 0;
    if (p1 == true) {
        flags |= 1;
    }
    Input* input = soExternalValueAccesser::getInput(m_moduleAccesser->m_stageObject);
    if (flags) {
        input->removeRumbleMask(flags);
    } else {
        input->removeRumble();
    }
}

void ftControllerModuleImpl::setRumbleAll(s32, s32, s32) { }

void ftControllerModuleImpl::stopRumbleAll(s32, s32) { }

bool ftControllerModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (acmd->getType() <= -1 || acmd->getType() >= 13) {
        return false;
    }
    switch (acmd->getType()) {
    case 12:
        clearLog();
        return true;
    default:
        return soControllerModuleImpl::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
}

void ftControllerModuleImpl::clearLog() {
    m_logNum = m_prev - 1;
}

void ftControllerModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    stopRumble(true);
    m_logCapacity = m_logs->size();
}

void ftControllerModuleImpl::notifyEventTurn(float, float, soModuleAccesser* moduleAccesser) {
    m_reverseXFrame = m_reverseXFrameNext;
    m_reverseXFrameNext = 0;
}
