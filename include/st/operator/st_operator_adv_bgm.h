#pragma once

#include <StaticAssert.h>
#include <st/operator/st_operator_bgm.h>
#include <types.h>

class stOperatorAdvBgm : public stOperatorBgm {
public:
    int m_stepBgmId;
    float m_bgmVolume;
    float m_bgmPlayOffsetFrame;

    virtual void processBegin();
    virtual ~stOperatorAdvBgm();

    void phaseNowWork();
    void phasePrepareRequest();
    void phasePrepareWait();
    void phasePlayReady();
    void phasePlaying();
    void phaseStop();
};
static_assert(sizeof(stOperatorAdvBgm) == 0x70, "Class is wrong size!");
