#include <gf/gf_scene.h>
#include <sq/sq_single_boss.h>
#include <sr/sr_common.h>
#include <types.h>

sqSingleBoss* sqSingleBoss::create() {
    return new (Heaps::GlobalMode) sqSingleBoss("sqSingleBoss");
}
