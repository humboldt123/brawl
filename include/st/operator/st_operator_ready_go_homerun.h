#pragma once

#include <st/operator/st_operator_ready_go.h>
#include <types.h>

class stOperatorReadyGoHomerun : public stOperatorReadyGo {
public:
    stOperatorReadyGoHomerun();
    virtual void processBegin();
    virtual ~stOperatorReadyGoHomerun();
};
