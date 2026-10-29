#pragma once

#include <StaticAssert.h>
#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorFighterRebirth : public stOperator {
public:
    float m_rebirthFrames;
    float m_rebirthCounter;
    void* unk50;

    stOperatorFighterRebirth();
    virtual void processBegin();
    virtual ~stOperatorFighterRebirth();
    virtual void startOperator();
    virtual void resetRebirth();
};
static_assert(sizeof(stOperatorFighterRebirth) == 0x54, "Class is wrong size!");
