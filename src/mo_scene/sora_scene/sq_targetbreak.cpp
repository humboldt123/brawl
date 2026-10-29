#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_targetbreak.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);

sqTargetBreak* sqTargetBreak::create() {
    return new (Heaps::GlobalMode) sqTargetBreak("sqTargetBreak");
}

void sqTargetBreak::start() {
    unk14 = gfSceneManager::getInstance()->unk31C;
    if (unk14 == 0xff) {
        unk8 = 9;
        unk14 = 0;
    } else {
        unk8 = 0;
        fn_8004D010(g_GameGlobal);
        unk18 = g_gfApplication->unk110[0] != 0;
    }
    unk19 = 0;
}

void sqTargetBreak::exit() { }
