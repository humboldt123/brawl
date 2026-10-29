// MATCH-ONLY: the constructor re-extends the s16 manage id at every use.
#define SO_EVENT_OBSERVER_MANAGE_S16
#include <cm/cm_camera_controller.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <if/if_mngr.h>
#include <ml/ml_ft_event.h>
#include <st/operator/st_operator_info.h>
#include <st/operator/st_operator_rule.h>
#include <st/stage.h>
#include <types.h>

extern "C" u8 fn_27_111D70(ftOwner* owner);
extern "C" void fn_8009C9B8(CameraController* controller, int playerNo);
extern "C" void fn_800DD0CC(IfMngr* mngr, int areaNo1, int areaNo2);
extern "C" void fn_800DD200(IfMngr* mngr);

bool mlFtEvent::isInfoManager() {
    return g_IfMngr != NULL;
}

mlFtEvent::mlFtEvent() : ftOutsideEventObserver(-1, 0) {
    if (m_sendID <= -1) {
        soEventManageModule* module = &g_ftManager->m_eventManageModule;
        addObserverSubS16(module->getManageId(), this, -1);
    }
    m_condition = g_GameGlobal->m_modeMelee;
    m_operatorInfo = NULL;
    m_operatorRule = NULL;
    unk38 = 0;
    unk34_80 = true;
    unk34_40 = false;
    unk34_20 = true;
    unk34_10 = false;
}

void mlFtEvent::destroy() {
    delete this;
}

int mlFtEvent::getInfoAreaNo(int playerNo) {
    return m_condition->m_playersInitData[playerNo].m_playerId;
}

float mlFtEvent::getDamageInfoRumbleRate(float damage) {
    float min = 3.0f;
    float max = 15.0f;
    float rate = 0.6f;
    if (damage >= min && damage <= max) {
        return rate * ((damage - min) / (max - min));
    }
    if (damage < min) {
        return 0.0f;
    }
    return 1.0f;
}

void mlFtEvent::notifyEventKnockout(int entryId) {
    if (m_operatorRule != NULL) {
        m_operatorRule->notifyEventKnockout(g_ftManager->getScoreEntryId(entryId));
    }
    int playerNo = g_ftManager->getPlayerNo(entryId);
    fn_8009C9B8(CameraController::getInstance(), playerNo);
}

void mlFtEvent::setCondition(gmGlobalModeMelee* condition) {
    m_condition = condition;
}

void mlFtEvent::notifyEventAddDragoonParts(int entryId, int part) {
    if (isInfoManager()) {
        g_IfMngr->addPlayerDragoonParts(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)), part);
    }
}

void mlFtEvent::notifyEventRemoveDragoonParts(int entryId, int part) {
    if (isInfoManager()) {
        int playerNo = g_ftManager->getPlayerNo(entryId);
        g_IfMngr->deletePlayerDragoonParts(getInfoAreaNo(playerNo), part);
        if (fn_27_111D70(g_ftManager->getOwner(entryId)) == 2) {
            g_IfMngr->setPlayerDragoonPartsComplete(getInfoAreaNo(playerNo), 0);
        }
    }
}

void mlFtEvent::notifyEventCompDragoonParts(int entryId) {
    if (isInfoManager()) {
        g_IfMngr->setPlayerDragoonPartsComplete(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)), 1);
    }
}

void mlFtEvent::notifyEventResetDragoonParts(int entryId) {
    if (isInfoManager()) {
        g_IfMngr->resetPlayerDragoonParts(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)));
    }
}

void mlFtEvent::notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) { }

void mlFtEvent::notifyEventRemoveEntry(int entryId) {
    if (isInfoManager() && unk34_80 && m_operatorInfo != NULL) {
        int areaNo = getInfoAreaNo(g_ftManager->getPlayerNo(entryId));
        m_operatorInfo->setPlayerCursorClear(areaNo);
        m_operatorInfo->setPlayerLoupeStatus(areaNo, 0);
    }
}

void mlFtEvent::notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId) { }

void mlFtEvent::notifyEventReEntryRequestFighter() { }

void mlFtEvent::notifyEventSucceedHit(int entryId, u32 consecutiveHits, float totalDamage) { }

void mlFtEvent::notifyEventFinalSlow(int entryId, float rate, int frames) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(rate, frames);
    }
}

void mlFtEvent::notifyEventFinalSlowCancel(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(1.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventFinalStop(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(0.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventFinalStopCancel(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(1.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventPikminMakeBloomAll(int entryId) {
    if (m_operatorRule != NULL) {
        m_operatorRule->notifyEventPikminBloom();
    }
}

void mlFtEvent::notifyEventHeartSwapStart(int entryId1, int entryId2) {
    if (isInfoManager()) {
        int areaNo1 = getInfoAreaNo(g_ftManager->getPlayerNo(entryId1));
        fn_800DD0CC(g_IfMngr, areaNo1, getInfoAreaNo(g_ftManager->getPlayerNo(entryId2)));
    }
}

void mlFtEvent::notifyEventHeartSwapEnd(int entryId1, int entryId2) {
    if (isInfoManager()) {
        fn_800DD200(g_IfMngr);
    }
}

bool mlFtEventResult::isInfoManager() {
    return false;
}
