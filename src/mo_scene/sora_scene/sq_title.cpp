#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <revolution/OS.h>
#include <sq/sq_title.h>
#include <sr/sr_common.h>
#include <types.h>

extern "C" void fn_8004D010(GameGlobal* global);

sqTitle* sqTitle::create() {
    return new (Heaps::GlobalMode) sqTitle("sqTitle");
}

void sqTitle::exit() { }
