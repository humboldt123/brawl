#pragma once

#include <st/operator/st_operator_ready_go.h>
#include <types.h>

class stOperatorSuddenDeath : public stOperatorReadyGo {
public:
    stOperatorSuddenDeath();
    virtual void processBegin();
    virtual ~stOperatorSuddenDeath();
    virtual bool isEnd();
    virtual void startOperator();
};
