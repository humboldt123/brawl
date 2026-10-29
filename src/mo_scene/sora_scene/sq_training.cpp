#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_training.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);

sqTraining* sqTraining::create() {
    return new (Heaps::GlobalMode) sqTraining("sqTraining");
}

void sqTraining::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 0xa;
    } else {
        unk8 = 0;
        fn_8004D010(g_GameGlobal);
    }
}

void sqTraining::exit() { }
