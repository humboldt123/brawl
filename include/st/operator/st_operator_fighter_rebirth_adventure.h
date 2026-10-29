#pragma once

#include <st/operator/st_operator_fighter_rebirth.h>
#include <types.h>

class stOperatorFighterRebirthAdventure : public stOperatorFighterRebirth {
public:
    u8 unk54;
    int unk58;
    u32 unk5C;
    u32 unk60;
    u8 unk64;

    stOperatorFighterRebirthAdventure();
    virtual void processBegin();
    virtual ~stOperatorFighterRebirthAdventure();
    virtual void startOperator();
    virtual void resetRebirth();
};
