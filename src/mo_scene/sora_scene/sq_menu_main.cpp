#include <gf/gf_scene.h>
#include <sq/sq_menu_main.h>
#include <sr/sr_common.h>
#include <types.h>

sqMenuMain* sqMenuMain::create() {
    return new (Heaps::GlobalMode) sqMenuMain("sqMenuMain");
}

void sqMenuMain::start() {
    gfSceneManager::getInstance();
    unk8 = 9;
    unk10 = -1;
    unkC = -1;
}

void sqMenuMain::exit() { }
