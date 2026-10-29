#include <gf/gf_scene.h>
#include <sq/sq_button.h>
#include <sr/sr_common.h>
#include <types.h>

sqButton* sqButton::create() {
    return new (Heaps::GlobalMode) sqButton("sqButton");
}

void sqButton::start() {
    unk8 = 0;
}

void sqButton::exit() { }
