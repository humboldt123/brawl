#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Meta Ridley's wnemSimple weapons (its fireball and its normal bullet), registered with the
// enemy weapon manager by the Enemy unit. Names from the map and the procs' own Report strings.
class wnemMetaridleyProc {
public:
    static void FireballFly(wnemSimple* wn);
    static void FireballExplosion(wnemSimple* wn, soModuleAccesser* acc);
    static void FireballTouchGround(wnemSimple* wn, soModuleAccesser* acc);
    static void NormalBulletFly(wnemSimple* wn);
    static void NormalBulletDisappear(wnemSimple* wn);
};
