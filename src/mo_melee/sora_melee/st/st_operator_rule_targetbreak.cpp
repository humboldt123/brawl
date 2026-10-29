#include <st/operator/st_operator_rule_targetbreak.h>
#include <types.h>

stOperatorRuleTargetBreak::stOperatorRuleTargetBreak() : stOperatorRuleMelee(0) { }

stOperatorRuleTargetBreak::~stOperatorRuleTargetBreak() { }

void stOperatorRuleTargetBreak::decisionExtraRule() {
    if (m_isComplete) {
        m_decisionKind = Decision_Complete;
        m_resultInfo->m_decisionKind = gmResultInfo::Decision_Complete;
    } else {
        m_decisionKind = Decision_Failure;
        m_resultInfo->m_decisionKind = gmResultInfo::Decision_Failure;
    }
}

bool stOperatorRuleTargetBreak::isSuddenDeath() {
    return false;
}
