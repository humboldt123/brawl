#pragma once

#include <st/operator/st_operator_rule_time.h>
#include <types.h>

class stOperatorRuleTimeHomerun : public stOperatorRuleTime {
public:
    virtual void processBegin();
    virtual ~stOperatorRuleTimeHomerun();
    virtual void startOperator();
    virtual bool isSuddenDeath();
};
