#pragma once

#include <st/operator/st_operator_rule.h>
#include <types.h>

class stOperatorRuleEditProtocol : public stOperatorRule {
public:
    virtual void processBegin();
    virtual void renderXlu();
    virtual ~stOperatorRuleEditProtocol();
    virtual bool isEnd();
    virtual void startOperator();
};
