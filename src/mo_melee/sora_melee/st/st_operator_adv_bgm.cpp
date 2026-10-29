#include <snd/snd_system.h>
#include <st/operator/st_operator_adv_bgm.h>
#include <st/stage.h>
#include <types.h>

extern "C" {
bool fn_80074018(sndSystem* system, int bgmId, int arg);
void fn_80073F84(sndSystem* system, int fade);
bool fn_80074184(sndSystem* system, int bgmId);
void fn_800741EC(sndSystem* system, int bgmId, int frame);
void fn_27_23B36C(const char* format, ...);
}

void stOperatorAdvBgm::processBegin() {
    void (stOperatorAdvBgm::*phases[])() = {
        &stOperatorAdvBgm::phaseNowWork,     &stOperatorAdvBgm::phasePrepareRequest,
        &stOperatorAdvBgm::phasePrepareWait, &stOperatorAdvBgm::phasePlayReady,
        &stOperatorAdvBgm::phasePlaying,     &stOperatorAdvBgm::phaseStop,
    };
    (this->*phases[m_state])();
}

void stOperatorAdvBgm::phaseNowWork() { }

void stOperatorAdvBgm::phasePrepareRequest() {
    sndSystem* system = g_sndSystem;
    Stage* stage = g_Stage;
    if (stage != NULL) {
        m_stepBgmId = stage->getNowStepBgmID();
        m_bgmVolume = stage->getBgmVolume();
        m_bgmPlayOffsetFrame = stage->getBgmPlayOffsetFrame();
        if (m_stepBgmId != -1) {
            if (!fn_80074018(system, m_stepBgmId, 1)) {
                fn_27_23B36C("ADV BGM \x83\x8A\x83\x4E\x83\x47\x83\x58\x83\x67\x82\xC9\x8E\xB8\x94\x73 BgmId=%d \n", m_stepBgmId);
                m_state = 0;
            } else {
                system->setBGMVol(false, m_bgmVolume);
                m_state = 2;
            }
        } else {
            fn_27_23B36C("ADV BGM ID \x82\xAA\x83\x65\x81\x5B\x83\x75\x83\x8B\x82\xC9\x8C\xA9\x82\xC2\x82\xA9\x82\xE7\x82\xC8\x82\xA2 BgmId=%d \n");
            fn_80073F84(system, 0);
            m_state = 0;
        }
    }
}

void stOperatorAdvBgm::phasePrepareWait() {
    if (fn_80074184(g_sndSystem, m_stepBgmId)) {
        m_state = 3;
    }
}

void stOperatorAdvBgm::phasePlayReady() {
    sndSystem* system = g_sndSystem;
    if (m_isRequestPlay) {
        fn_800741EC(system, m_stepBgmId, m_bgmPlayOffsetFrame);
        m_state = 4;
    }
}

void stOperatorAdvBgm::phasePlaying() {
    sndSystem* system = g_sndSystem;
    if (m_isRequestStop) {
        fn_80073F84(system, 30);
        m_isRequestPlay = false;
        m_state = 5;
    }
}

void stOperatorAdvBgm::phaseStop() {
    if (m_isRequestStop) {
        m_isRequestStop = false;
        m_state = 0;
    }
}

stOperatorAdvBgm::~stOperatorAdvBgm() { }
