#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_outside_event_presenter.h>
#include <types.h>

void ftOutsideEventPresenter::notifyOutsideEventSetDamage(float damage, bool p2, bool p3) {
    ftOwner* entryOwner = g_ftManager->getOwner(m_entryId);
    float value;
    if (g_ftManager->isStamina() == true) {
        value = entryOwner->getHitPoint();
    } else {
        value = entryOwner->getDamage();
    }
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        ftOutsideEventObserver*& observer = list->atIndex(i);
        observer->notifyEventSetDamage(m_entryId, damage, value, p2, p3);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventOutsideDeadArea(soGroundUtil::DeadAreaCheckResult result, bool* isDead) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventOutsideDeadArea(m_entryId, result, isDead);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventDead(int entryId, int deadCount, int deadReason, int respawnFrames) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventDead(entryId, deadCount, (Fighter::Dead::Reason)deadReason, respawnFrames);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventAppeal(int appealKind) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventAppeal(m_entryId, appealKind);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventBeat(int winningEntryId, int losingEntryId) {
    if (g_ftManager->isValidEntryId(winningEntryId) == true) {
        soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
        s32 num = list->size();
        for (s32 i = 0; i < num; i++) {
            list->atIndex(i)->notifyEventBeat(winningEntryId, losingEntryId);
        }
    }
}

void ftOutsideEventPresenter::notifyOutsideEventSuicide(int entryId) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventSuicide(entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventChangeStart(int playerNo, int activeInstanceIndex, ftKind kind) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventChangeStart(m_entryId, playerNo, activeInstanceIndex, kind);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventChangeEnd(int playerNo, int activeInstanceIndex, ftKind kind) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventChangeEnd(m_entryId, playerNo, activeInstanceIndex, kind);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventAddDragoonParts(int partsKind) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventAddDragoonParts(m_entryId, partsKind);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventRemoveDragoonParts(int partsKind) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventRemoveDragoonParts(m_entryId, partsKind);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventCompDragoonParts() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventCompDragoonParts(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventResetDragoonParts() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventResetDragoonParts(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventSetCursor(u32 index) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventSetCursor(m_entryId, index);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventSetNameCursor(u32 index) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventSetNameCursor(m_entryId, index);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventSetLoupe(u32 index) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventSetLoupe(m_entryId, index);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventRemoveEntry() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventRemoveEntry(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventStartFinal() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventStartFinal(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventEndFinal() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventEndFinal(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventFinalSlow(float rate, int frames) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventFinalSlow(m_entryId, rate, frames);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventFinalSlowCancel() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventFinalSlowCancel(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventFinalStop() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventFinalStop(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventFinalStopCancel() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventFinalStopCancel(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventEntryEnd() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventEntryEnd(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventResultEnd() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventResultEnd(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventGetItem(itKind kind, int itVariation, int genParamId, int instanceId) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventGetItem(m_entryId, kind, itVariation, genParamId, instanceId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventSucceedHit(u32 consecutiveHits, float totalDamage) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventSucceedHit(m_entryId, consecutiveHits, totalDamage);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventResultWin(int winKind) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventResultWin(m_entryId, winKind);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventYoshiEggStart() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventYoshiEggStart(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventYoshiEggEnd() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventYoshiEggEnd(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventOnInput() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventOnInput(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyOutsideEventKnockout() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventKnockout(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyEventPikminMakeBloomAll() {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventPikminMakeBloomAll(m_entryId);
    }
}

void ftOutsideEventPresenter::notifyEventKirbyCopySetup(int copyKind) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventKirbyCopySetup(m_entryId, copyKind);
    }
}

void ftOutsideEventPresenter::notifyEventKirbyCopyCancel(int copyKind) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventKirbyCopyCancel(m_entryId, copyKind);
    }
}

void ftOutsideEventPresenter::notifyEventHeartSwapStart(int entryId1, int entryId2) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventHeartSwapStart(entryId1, entryId2);
    }
}

void ftOutsideEventPresenter::notifyEventHeartSwapEnd(int entryId1, int entryId2) {
    soInstanceManagerFullProperty<ftOutsideEventObserver*>* list = getObserverList();
    s32 num = list->size();
    for (s32 i = 0; i < num; i++) {
        list->atIndex(i)->notifyEventHeartSwapEnd(entryId1, entryId2);
    }
}
