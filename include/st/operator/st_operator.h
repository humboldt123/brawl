#pragma once

// Local BrawlHeaders shadow: adds the stOperator constructor and the exit/start/pause requests of st_operator.o.

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <gm/gm_global_mode_melee.h>
#include <types.h>

class stOperator : public gfTask {
public:
    u8 m_state;
    bool m_isStart;
    bool m_isExit;
    bool m_isPause;
    gmGlobalModeMelee* m_condition;

    stOperator(const char* name);
    virtual void processBegin();
    virtual ~stOperator();

    bool isExit();
    bool isPause();
    void requestExit();
    void requestStart();
    void requestPause(bool pause);
    void setCondition(gmGlobalModeMelee* condition);
};
static_assert(sizeof(stOperator) == 72, "Class is wrong size!");
