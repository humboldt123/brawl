#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_training.h>
#include <sr/sr_common.h>
#include <types.h>


sqTraining* sqTraining::create() {
    return new (Heaps::GlobalMode) sqTraining("sqTraining");
}

void sqTraining::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 0xa;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
    }
}

void sqTraining::exit() { }
