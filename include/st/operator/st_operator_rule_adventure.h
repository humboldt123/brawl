#pragma once

#include <st/operator/st_operator_rule.h>
#include <types.h>

class stOperatorRuleAdventure : public stOperatorRule {
public:
    char _CC[0x2C];
    void* unkF8;

    virtual void processBegin();
    virtual ~stOperatorRuleAdventure();
    virtual bool isEnd();
    virtual void startOperator();
    virtual void setOpAppearanceHelper(void* helper);
};
