#include <st/operator/st_operator_bgm.h>
#include <types.h>

stOperatorBgm::stOperatorBgm() :
    stOperator("StOperatorBgm"), unk4C(0), unk50(-1), unk54(0), unk58(60), m_isRequestPlay(false),
    m_isRequestStop(false), m_isSuddenDeath(false), unk60(0) {
    m_state = 0;
}

stOperatorBgm::~stOperatorBgm() { }

bool stOperatorBgm::isReady() {
    if (m_state == 3) {
        return true;
    }
    return m_state == 0;
}

void stOperatorBgm::requestPlay() {
    m_isRequestPlay = true;
    m_isSuddenDeath = false;
}

void stOperatorBgm::requestStop() {
    m_isRequestStop = true;
    m_isSuddenDeath = false;
}

void stOperatorBgm::requestPrepare() {
    if (m_state == 0) {
        m_state = 1;
        m_isSuddenDeath = false;
    }
}

void stOperatorBgm::requestPrepareSuddenDeath() {
    requestPrepare();
    m_isSuddenDeath = true;
}
