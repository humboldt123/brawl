#include <st/operator/st_operator_rule_kumite.h>
#include <types.h>

stOperatorRuleKumite::stOperatorRuleKumite(u8 arg) : stOperatorRuleMelee(arg) { }

stOperatorRuleKumite::~stOperatorRuleKumite() { }

bool stOperatorRuleKumite::isSuddenDeath() {
    return false;
}

void stOperatorRuleKumite::calcScore() {
    setNormalResultInfo();
}
