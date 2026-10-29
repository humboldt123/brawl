#include <ip/ip_padrec.h>
#include <ip/ip_switch.h>
#include <st/operator/st_operator_controller.h>
#include <types.h>

void stOperatorController::processBegin() { }

void stOperatorController::stopController() {
    ipSwitch::getInstance()->stopPadRec();
    ipSwitch::getInstance()->stopPadPlay();
}

void stOperatorController::setMeleeProcessCounter(u16 count) {
    ipSwitch::getInstance()->m_keyRecorder->setMeleeInitialProcessCount(count);
}

u16 stOperatorController::getMeleeProcessCounter() {
    return ipSwitch::getInstance()->m_keyRecorder->getMeleeInitialProcessCount();
}

bool stOperatorController::isPlaying() {
    bool isPlaying = true;
    if (ipSwitch::getInstance()->m_keyRecorder->m_mode != ipKeyRecorder::Playing &&
        !ipSwitch::getInstance()->m_flags.unk7)
    {
        isPlaying = false;
    }
    return isPlaying;
}
