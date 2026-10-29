#include <sr/sr_common.h>
#include <so/so_world.h>
#include <types.h>

void soWorld::initialize() {
    g_soWorld = new (Heaps::System) soWorld();
}
