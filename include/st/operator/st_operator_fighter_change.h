#pragma once

#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorFighterChange : public stOperator {
public:
    stOperatorFighterChange();
    virtual ~stOperatorFighterChange();
    virtual void processBegin();
};
