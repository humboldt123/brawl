#pragma once

#include <st/operator/st_operator_appearance_fighter.h>
#include <types.h>

class stOperatorAppearanceFighterResult : public stOperatorAppearanceFighter {
public:
    stOperatorAppearanceFighterResult();
    virtual ~stOperatorAppearanceFighterResult();
    virtual void startAppearance();
    virtual void processBegin();
    virtual bool isEnd();
};
