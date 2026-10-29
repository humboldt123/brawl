#include <st/operator/st_operator_sudden_death.h>
#include <types.h>

stOperatorSuddenDeath::stOperatorSuddenDeath() : stOperatorReadyGo(true) {
    m_state = 0;
    unk4C = 62.0f;
}

stOperatorSuddenDeath::~stOperatorSuddenDeath() { }

bool stOperatorSuddenDeath::isEnd() {
    return m_state == 5;
}

void stOperatorSuddenDeath::startOperator() {
    m_state = 1;
}
