#pragma once

#include <so/so_module_accesser.h>
#include <wn/wn_em_simple.h>

// The proc functions of the Bombhead's wnemSimple weapon (the head it throws), registered with the enemy weapon
// manager by the Enemy unit. Names from the map (wnemBombheadProc::HeadFly, HeadExplosion) and the procs' own Report
// strings.
class wnemBombheadProc {
public:
    static void HeadFly(wnemSimple* wn, soModuleAccesser* acc);
    static void HeadExplosion(wnemSimple* wn, soModuleAccesser* acc);
};
