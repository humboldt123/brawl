#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_event.h>
#include <sr/sr_common.h>
#include <types.h>


void sqEvent::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 0xa;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        unk18 = g_gfApplication->unk110[0] != 0;
    }
}

void sqEvent::exit() { }
