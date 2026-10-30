#pragma once

// Local shadow of BrawlHeaders' ft/ft_outside_event_presenter.h. The presenter that sends the fighter manager's
// "outside" events (deaths, KOs, final smash, ...) to the observers of ftOutsideEventObserver. The BrawlHeaders copy
// derives from soEventPresenter<ftOutsideEventPresenter>; the observer list type is ftOutsideEventObserver
// (the REL names soEventPresenter<ftOutsideEventObserver> and its event unit wrapper).
// The constructor is defined in ft/ft_manager.h, once ftOutsideEventObserver is complete.

#include <StaticAssert.h>
#include <ft/ft_entry.h>
#include <it/it_create.h>
#include <so/event/so_event_presenter.h>
#include <so/ground/so_ground_util.h>
#include <types.h>

class ftOutsideEventObserver;

class ftOutsideEventPresenter : public soEventPresenter<ftOutsideEventObserver> {
    int m_entryId;

public:
    inline ftOutsideEventPresenter(s16 manageId, int entryId);
    virtual ~ftOutsideEventPresenter() { }

    void notifyEventPikminMakeBloomAll();
    void notifyOutsideEventKnockout();
    void notifyOutsideEventOnInput();
    void notifyOutsideEventSuicide(int entryId);
    void notifyOutsideEventBeat(int winningEntryId, int losingEntryId);
    void notifyEventKirbyCopySetup(int copyKind);
    void notifyEventKirbyCopyCancel(int copyKind);
    void notifyOutsideEventDead(int entryId, int deadCount, int deadReason, int respawnFrames);

    // HYPOTHESIS: the two flags (the fighter's added-damage hook passes whether damage was added and its negation).
    void notifyOutsideEventSetDamage(float damage, bool p2, bool p3);

    // HYPOTHESIS: parameter types follow the observer callbacks they forward (the module map has none).
    void notifyOutsideEventOutsideDeadArea(soGroundUtil::DeadAreaCheckResult result, bool* isDead);
    void notifyOutsideEventAppeal(int appealKind);
    void notifyOutsideEventChangeStart(int playerNo, int activeInstanceIndex, ftKind kind);
    void notifyOutsideEventChangeEnd(int playerNo, int activeInstanceIndex, ftKind kind);
    void notifyOutsideEventAddDragoonParts(int partsKind);
    void notifyOutsideEventRemoveDragoonParts(int partsKind);
    void notifyOutsideEventCompDragoonParts();
    void notifyOutsideEventResetDragoonParts();
    void notifyOutsideEventSetCursor(u32 index);
    void notifyOutsideEventSetNameCursor(u32 index);
    void notifyOutsideEventSetLoupe(u32 index);
    void notifyOutsideEventRemoveEntry();
    void notifyOutsideEventStartFinal();
    void notifyOutsideEventEndFinal();
    void notifyOutsideEventFinalSlow(float rate, int frames);
    void notifyOutsideEventFinalSlowCancel();
    void notifyOutsideEventFinalStop();
    void notifyOutsideEventFinalStopCancel();
    void notifyOutsideEventEntryEnd();
    void notifyOutsideEventResultEnd();
    void notifyOutsideEventGetItem(itKind kind, int itVariation, int genParamId, int instanceId);
    void notifyOutsideEventSucceedHit(u32 consecutiveHits, float totalDamage);
    void notifyOutsideEventResultWin(int winKind);
    void notifyOutsideEventYoshiEggStart();
    void notifyOutsideEventYoshiEggEnd();
    void notifyEventHeartSwapStart(int entryId1, int entryId2);
    void notifyEventHeartSwapEnd(int entryId1, int entryId2);
};
static_assert(sizeof(ftOutsideEventPresenter) == 16, "Class is wrong size!");
