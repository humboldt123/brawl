#include <gf/gf_scene.h>
#include <sq/sq_replay.h>
#include <sr/sr_common.h>
#include <types.h>

sqReplay* sqReplay::create() {
    return new (Heaps::GlobalMode) sqReplay("sqReplay");
}

void sqReplay::start() {
    unk8 = 0;
}

void sqReplay::exit() { }
