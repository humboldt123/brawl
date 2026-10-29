#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_homerun.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);

sqHomerun* sqHomerun::create() {
    return new (Heaps::GlobalMode) sqHomerun("sqHomerun");
}

void sqHomerun::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 7;
    } else {
        unk8 = 0;
        fn_8004D010(g_GameGlobal);
        unk11 = g_gfApplication->unk110[0] != 0;
    }
    unk12 = 0;
}

void sqHomerun::exit() { }
