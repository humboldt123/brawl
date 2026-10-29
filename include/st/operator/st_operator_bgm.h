#pragma once

#include <StaticAssert.h>
#include <st/operator/st_operator.h>
#include <types.h>

class stOperatorBgm : public stOperator {
public:
    int unk48;
    u8 unk4C;
    int unk50;
    u8 unk54;
    int unk58;
    bool m_isRequestPlay;
    bool m_isRequestStop;
    bool m_isSuddenDeath;
    u32 unk60;

    stOperatorBgm();
    virtual void processBegin();
    virtual ~stOperatorBgm();

    bool isReady();
    void requestPlay();
    void requestStop();
    void requestPrepare();
    void requestPrepareSuddenDeath();
};
static_assert(sizeof(stOperatorBgm) == 0x64, "Class is wrong size!");
