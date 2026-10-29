#include <st/operator/st_operator_fighter_rebirth.h>
#include <types.h>

stOperatorFighterRebirth::stOperatorFighterRebirth() : stOperator("StOperatorFighterRebirth") {
    m_state = 0;
    m_rebirthFrames = 60.0f;
}

stOperatorFighterRebirth::~stOperatorFighterRebirth() { }

void stOperatorFighterRebirth::startOperator() {
    m_state = 1;
    m_rebirthCounter = m_rebirthFrames;
}

void stOperatorFighterRebirth::resetRebirth() {
    m_state = 0;
}
