#pragma once

#include <st/operator/st_operator_rule.h>
#include <types.h>

class stOperatorRuleTime : public stOperatorRule {
public:
    float unkCC;

    stOperatorRuleTime();
    virtual void processBegin();
    virtual ~stOperatorRuleTime();
    virtual bool isEnd();
    virtual void startOperator();
    virtual void calcScore();
    virtual bool isSuddenDeath();
};
static_assert(sizeof(stOperatorRuleTime) == 0xD0, "Class is wrong size!");
