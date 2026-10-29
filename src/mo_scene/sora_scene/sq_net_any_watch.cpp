#include <gf/gf_scene.h>
#include <sq/sq_net_any_watch.h>
#include <sr/sr_common.h>
#include <types.h>

sqNetAnyWatch* sqNetAnyWatch::create() {
    return new (Heaps::GlobalMode) sqNetAnyWatch("sqNetAnyWatch");
}

void sqNetAnyWatch::exit() { }
