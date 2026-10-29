#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <sq/sq_boot.h>
#include <sr/sr_common.h>
#include <types.h>

sqBoot* sqBoot::create() {
    return new (Heaps::GlobalMode) sqBoot("sqBoot");
}

void sqBoot::start() {
    unk14 = gfSceneManager::getInstance()->unk31C;
    if ((g_gfApplication->m_e4 >> 24) == 1) {
        unk14 = 2;
    }
    unk8 = 0;
}

void sqBoot::exit() { }
