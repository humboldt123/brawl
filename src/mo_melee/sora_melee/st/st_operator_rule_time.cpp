#include <st/operator/st_operator_rule_time.h>
#include <types.h>

stOperatorRuleTime::stOperatorRuleTime() {
    m_state = 0;
    unkCC = 0.0f;
}

stOperatorRuleTime::~stOperatorRuleTime() { }

bool stOperatorRuleTime::isEnd() {
    return m_state == 5;
}
