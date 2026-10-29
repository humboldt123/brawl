#pragma once

#include <st/operator/st_operator_appearance_fighter.h>
#include <types.h>

class stOperatorAppearanceFighterNull : public stOperatorAppearanceFighter {
public:
    stOperatorAppearanceFighterNull();
    virtual ~stOperatorAppearanceFighterNull();
    virtual void startAppearance();
    virtual void processBegin();
    virtual bool isEnd();
};
