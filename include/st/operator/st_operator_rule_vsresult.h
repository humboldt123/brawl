#pragma once

#include <st/operator/st_operator_rule.h>
#include <types.h>

class stOperatorRuleVsResult : public stOperatorRule {
public:
    u32 unkCC;

    stOperatorRuleVsResult();
    virtual ~stOperatorRuleVsResult();
    virtual bool isEnd();
    virtual void startOperator();
};
