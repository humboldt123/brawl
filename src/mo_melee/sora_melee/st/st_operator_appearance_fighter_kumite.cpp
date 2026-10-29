#include <st/operator/st_operator_appearance_fighter_kumite.h>
#include <types.h>

stOperatorAppearanceFighterKumite::stOperatorAppearanceFighterKumite(u8 arg) {
    unk1B6 = arg;
    unk1B8 = NULL;
    unk1B5 = false;
}

stOperatorAppearanceFighterKumite::~stOperatorAppearanceFighterKumite() { }

bool stOperatorAppearanceFighterKumite::isEnd() {
    return unk1B5;
}
