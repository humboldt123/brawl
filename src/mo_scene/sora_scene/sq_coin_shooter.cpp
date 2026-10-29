#include <gf/gf_scene.h>
#include <sq/sq_coin_shooter.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_800515E8();
extern "C" void fn_80051680();

sqCoinshooter* sqCoinshooter::create() {
    return new (Heaps::GlobalMode) sqCoinshooter("sqCoinShooter");
}

void sqCoinshooter::start() {
    unk8 = 0;
}

void sqCoinshooter::setNext() {
    gfSceneManager* mgr = gfSceneManager::getInstance();
    bool loop = true;
    while (loop) {
        loop = false;
        switch (unk8) {
        default:
            unk8 = 1;
            loop = true;
            unkC = 3;
            unk10 = 0x10;
            break;
        case 1:
            mgr->setNextScene("scMemoryChange", unk10);
            unk8 = 2;
            break;
        case 2:
            unk8 = unkC;
            loop = true;
            break;
        case 3:
            fn_800515E8();
            fn_80051680();
            mgr->setNextScene("scCoinShooter", 0);
            unk8 = 4;
            break;
        case 4:
            unk8 = 5;
            loop = true;
            break;
        case 5:
            mgr->setNextSequence("sqPrizeCheck", 0x11);
            unk8 = 6;
            break;
        }
    }
}

void sqCoinshooter::exit() { }
