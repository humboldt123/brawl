#include <gf/gf_scene.h>
#include <sq/sq_prize_check.h>
#include <sr/sr_common.h>
#include <types.h>

sqPrizeCheck* sqPrizeCheck::create() {
    return new (Heaps::GlobalMode) sqPrizeCheck("sqPrizeCheck");
}

void sqPrizeCheck::exit() { }

bool sqPrizeCheck::checkModeMeleeClear() {
    bool result = false;
    switch (unk14) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 21:
        result = true;
        break;
    }
    return result;
}
