#include <gf/gf_scene.h>
#include <sq/sq_adventure.h>
#include <sr/sr_common.h>
#include <types.h>

sqAdventure* sqAdventure::create() {
    return new (Heaps::GlobalMode) sqAdventure("sqAdventure");
}
