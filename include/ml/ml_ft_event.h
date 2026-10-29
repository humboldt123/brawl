#pragma once

#include <StaticAssert.h>
#include <ft/ft_manager.h>
#include <types.h>

class gmGlobalModeMelee;
class stOperatorInfo;
class stOperatorRule;

class mlFtEvent : public ftOutsideEventObserver {
public:
    // HYPOTHESIS: the match settings, set from g_GameGlobal->m_modeMelee and by setCondition.
    gmGlobalModeMelee* m_condition;
    char unk10[0x1C];
    // HYPOTHESIS: the match operators that receive the cursor/loupe states and the knockouts.
    stOperatorInfo* m_operatorInfo;
    stOperatorRule* m_operatorRule;
    bool unk34_80 : 1;
    bool unk34_40 : 1;
    bool unk34_20 : 1;
    bool unk34_10 : 1;
    u8 unk34_0f : 4;
    u32 unk38;

    static mlFtEvent* create();
    mlFtEvent();

    int getInfoAreaNo(int playerNo);
    // HYPOTHESIS: maps the damage of one hit to a controller rumble rate.
    float getDamageInfoRumbleRate(float damage);
    // HYPOTHESIS: parameter type from the field it sets.
    void setCondition(gmGlobalModeMelee* condition);

    virtual void notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind);
    virtual void notifyEventAddDragoonParts(int entryId, int);
    virtual void notifyEventCompDragoonParts(int entryId);
    virtual void notifyEventRemoveDragoonParts(int entryId, int);
    virtual void notifyEventResetDragoonParts(int entryId);
    virtual void notifyEventReEntryRequestFighter();
    virtual void notifyEventRemoveEntry(int entryId);
    virtual void notifyEventFinalSlow(int entryId, float, int);
    virtual void notifyEventFinalSlowCancel(int entryId);
    virtual void notifyEventFinalStop(int entryId);
    virtual void notifyEventFinalStopCancel(int entryId);
    virtual void notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId);
    virtual void notifyEventSucceedHit(int entryId, u32 consecutiveHits, float totalDamage);
    virtual void notifyEventPikminMakeBloomAll(int entryId);
    virtual void notifyEventKnockout(int entryId);
    virtual void notifyEventHeartSwapStart(int entryId1, int entryId2);
    virtual void notifyEventHeartSwapEnd(int, int);
    virtual ~mlFtEvent();
    virtual void destroy();
    virtual bool isInfoManager();
};
static_assert(sizeof(mlFtEvent) == 0x3C, "Class is wrong size!");

class mlFtEventResult : public mlFtEvent {
public:
    virtual bool isInfoManager();
};
