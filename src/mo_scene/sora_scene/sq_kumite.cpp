#include <gf/gf_scene.h>
#include <sq/sq_kumite.h>
#include <sr/sr_common.h>
#include <types.h>

sqKumite* sqKumite::create() {
    return new (Heaps::GlobalMode) sqKumite("sqKumite");
}

void sqKumite::exit() { }
