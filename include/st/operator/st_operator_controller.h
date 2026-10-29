#pragma once

#include <StaticAssert.h>
#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorController : public stOperator {
public:
    stOperatorController(const char* name);
    virtual void processBegin();
    virtual ~stOperatorController();

    void startController();
    void stopController();
    void setControllerSource();
    void setMeleeProcessCounter(u16 count);
    u16 getMeleeProcessCounter();
    bool isPlaying();
};
static_assert(sizeof(stOperatorController) == 0x48, "Class is wrong size!");
