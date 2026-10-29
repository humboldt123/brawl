#include <st/operator/st_operator_rule_edit_protocol.h>
#include <types.h>

bool stOperatorRuleEditProtocol::isEnd() {
    return m_state == 4;
}

void stOperatorRuleEditProtocol::startOperator() {
    m_state = 1;
}
