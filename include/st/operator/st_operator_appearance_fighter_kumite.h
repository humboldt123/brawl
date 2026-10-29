#pragma once

#include <st/operator/st_operator_appearance_fighter.h>
#include <types.h>

class stOperatorAppearanceFighterKumite : public stOperatorAppearanceFighter {
public:
    stOperatorAppearanceFighterKumite(u8 arg);
    virtual ~stOperatorAppearanceFighterKumite();
    virtual void startAppearance();
    virtual void processBegin();
    virtual bool isEnd();
};
