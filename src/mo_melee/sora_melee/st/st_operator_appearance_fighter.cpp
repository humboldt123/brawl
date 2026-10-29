#include <st/operator/st_operator_appearance_fighter.h>
#include <types.h>

stOperatorAppearanceFighter::~stOperatorAppearanceFighter() { }

bool stOperatorAppearanceFighter::isEnd() {
    return m_state == 7;
}
