#pragma once

#include <StaticAssert.h>
#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorDebugFighterReset : public stOperator {
public:
    bool unk48; // 0x48 HYPOTHESIS: the debug request flag that startOperator raises

    void startOperator();
};
