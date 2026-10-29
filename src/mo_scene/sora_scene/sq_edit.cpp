#include <gf/gf_scene.h>
#include <sq/sq_edit.h>
#include <sr/sr_common.h>
#include <types.h>

sqEdit* sqEdit::create() {
    return new (Heaps::GlobalMode) sqEdit("sqEdit");
}

void sqEdit::start() {
    unk8 = 2;
}

void sqEdit::exit() { }
