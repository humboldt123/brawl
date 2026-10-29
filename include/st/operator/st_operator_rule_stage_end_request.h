#pragma once

#include <st/operator/st_operator_rule.h>
#include <types.h>

class stOperatorRuleStageEndRequest : public stOperatorRule {
public:
    stOperatorRuleStageEndRequest();
    virtual void processBegin();
    virtual ~stOperatorRuleStageEndRequest();
    virtual bool isEnd();
    virtual void startOperator();
    virtual void calcScore() { }
    virtual bool isSuddenDeath();
    virtual bool notifyEventDead(int entryId, u32 playerIndex, int) { return true; }
    virtual void notifyEventStartStage() { }
    virtual void notifyEventSetupCorps() { }
    virtual void notifyEventBeat(u32 playerIndex1, u32 playerIndex2) { }
    virtual void notifyEventYoshiEgg() { }
    virtual void notifyEventPikminBloom() { }
    virtual bool isFinishCheck();
};
