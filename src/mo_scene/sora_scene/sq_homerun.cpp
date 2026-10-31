#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_homerun.h>
#include <sr/sr_common.h>
#include <types.h>


sqHomerun* sqHomerun::create() {
    return new (Heaps::GlobalMode) sqHomerun("sqHomerun");
}

void sqHomerun::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 7;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        unk11 = g_gfApplication->unk110[0] != 0;
    }
    unk12 = 0;
}

void sqHomerun::exit() { }
