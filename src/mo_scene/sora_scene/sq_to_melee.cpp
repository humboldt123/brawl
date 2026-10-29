#include <gf/gf_scene.h>
#include <sq/sq_to_melee.h>
#include <sr/sr_common.h>
#include <types.h>

sqToMelee* sqToMelee::create() {
    return new (Heaps::GlobalMode) sqToMelee("sqToMelee");
}

void sqToMelee::exit() { }
